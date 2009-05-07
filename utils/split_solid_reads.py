#!/usr/bin/python

import sys, os, mmap, re
import time
import traceback

#try:
#from cStringIO import StringIO
#except:
#from StringIO import StringIO

CHUNK = 12 * 1024 ## 12k

def auto_gz_open( filename, direction ):
  #a = os.popen('gunzip -c F3_R3.mates.9.gz');
  fn = filename
  ## Fish around for gz version if reading
  if direction[0]=='r':
    if not os.path.exists(fn):
      sys.stderr.write(fn+' does not exist.\n')
      ## Toggle gz extension
      if filename[-3:] == '.gz':
        fn = filename[:-3]
      else:
        fn = fn + ".gz"
    # This may raise an IOError
    filePtr = open( fn, "rb" )
    magic = filePtr.read(2)
    filePtr.close()
    if magic[0] == '\x1f' and magic[1] == '\x8b':
      #sys.stderr.write("opening as gzip\n")
      return os.popen("gzip -d -c '%s'" % fn, direction, CHUNK)
  elif direction[0:]=='w':
    if fn[-3:] == '.gz':
      return os.popen("gzip -c -1 '%s'" % fn, direction)
  # Otherwise, writing, or reading and not gzip
  return open( fn, direction, CHUNK )


##======================================================================
class PanelData:
  def __init__(self, pnum, numreads, start, end, data):
    self.panel = pnum
    self.numreads = numreads
    self.start = start
    self.end = end
    self.data = data

##======================================================================

class ReadFile:
  idpat = re.compile(r">(\d{1,4})_")
  defaultdirection = 'rb'

  def __init__(self, filename, direction=None):
    self.filename = filename
    self.fh = None
    self.comments = []
    self.pos = None
    self.currentpanel = None
    self.buf = None
    self.seekable = False
    self.index = ReadIndex(self.filename)
    self.direction = None ## current direction - only changed when opening file
    self.verbose = 0
    if direction:
      self.openfile(direction)

  def openfile(self, direction=None):
    """ NB: Does not auto-rewind """
    if direction and self.direction != direction:
      self.close()

    if not self.fh:        
      self.fh = auto_gz_open(self.filename, direction or self.defaultdirection)
      self.direction = direction or self.defaultdirection
      try:
        self.pos = self.fh.tell()
        self.seekable = True
      except IOError:
        self.pos = 0
        self.seekable = False
    return self.fh

  def close(self):
    if self.fh:
      self.fh.close();
      self.fh = None

  def getPanelDataByIndex(self, p):
    if self.index.isComplete():
      # p is IndexEntry
      self.seek(p.start)
      assert(self.tell() == p.start, "Unexpected file location: %d != %d" % (self.tell(), p.start))
      pdata = self.read(p.end - p.start) # Don't bother to tokenize with readline
      sys.stderr.write("Extracted bytes (%d) : %d -> %d (%d) @ %d\n" % (p.panel, p.start, p.end, len(pdata), self.tell()))
      # Update position pointer to point to next panel
      self.currentpanel = self.index.getNextPanel(p.panel)
      return PanelData(p.panel, p.reads, p.start, p.end, pdata)
    else:
      raise NotImplementedError("Can't retrieve specific panel on unindexed file")

  def __iter__(self):
    """ Return a panels worth of data at a time """

    if self.index.isComplete():
      # read each panel
      panels = self.index.getPanels()
      if not self.currentpanel is None:
        # Resume from last seen panel
        cidx = panels.index(self.currentpanel)
        sys.stderr.write("Resuming index iterator from panel: %d @ %d\n" % (self.currentpanel, cidx))
        panels = panels[cidx:]
      else:
        sys.stderr.write("Iterating through all panels\n")

      for p in panels:
        sys.stderr.write("Returning data for panel %d\n" % p)
        yield self.getPanelDataByIndex(self.index.getPanel(p))
      return

    # else:

    isEOF = True
    cpos = self.tell()

    # When reading, don't assume panel
    #self.currentpanel = None
    panelstartpos = cpos
    readcount = 0

    pdata = []
    while 1:

      if not self.buf:
        self.buf = self.fh.readlines(CHUNK)

      # Got EOF during last buf read
      if (not self.buf) or (self.buf is None):
        break

      panel = None

      # Process each line
      for line in self.buf:
        if line[0] == '#':
          #sys.stderr.write("COMMENT: '%s'" % line)
          # FIXME: Should have processed
          self.comments.append(line)
          continue

        if line[0] == '>':
          panel = int(line[1:line.index("_")])

        if panel > self.currentpanel:
          if self.currentpanel != None:
            completepanel = self.currentpanel
            self.currentpanel = panel
            if not self.index.isComplete():
                self.index.append(completepanel, readcount, panelstartpos, cpos)
                self.index.append(panel, 0, cpos, -1) ## special hint for next panel...
            sys.stderr.write("Panel: %d (%d, %d) Next: %d\n" % (completepanel, panelstartpos, cpos, panel))
            yield PanelData(completepanel, readcount, panelstartpos, cpos, pdata)
            # What about cpos? If we never return from yield, cpos is not accurate

          # reset buffer and counters
          #pdata.truncate(0)
          pdata = []
          self.currentpanel = panel
          panelstartpos = cpos
          readcount = 0

        # OK, this read counts
        readcount += 1
        # Always append line to current buffer.
        pdata.append(line)

        #finally:
        cpos += len(line)

      # buf is exhausted (but for loop doesn't alter self.buf)
      self.buf = None
      assert(cpos == self.tell(), "Disoriented in file: %d != %d" % (cpos, self.tell()))

    # Handle final panel
    if self.currentpanel != None:
      if not self.index.isComplete():
        self.index.append(self.currentpanel, readcount, panelstartpos, cpos)
      yield PanelData(self.currentpanel, readcount, panelstartpos, cpos, pdata)


  def read(self, s):
    if not self.fh:
      self.openfile()
    tmp = self.fh.read(s)
    if len(tmp) == 0:
      return None
    if self.seekable:
      self.tell()
    else:
      self.pos += len(tmp)
    return tmp

  def tell(self):
    if self.seekable:
      self.pos = self.fh.tell()
    return self.pos

  def seek(self, dest):
    if self.seekable:
      self.fh.seek(dest)
      return

    ## Emulate seek on pipe
    if dest < self.pos:
      sys.stderr.write("Must rewind on gzip: @ %d, want %d" % (self.pos, dest))
      raise IOError("Unable to seek backwards!")
      #self.reopen()

    if dest == self.pos:
      return

    while (dest > self.pos):
      #if self.verbose:
      #  sys.stderr.write("Scanning forward: %d --> %d\n" % (self.pos, dest))
      if not self.read(min(CHUNK, dest - self.pos)):
        # EOF
        break

    if dest != self.pos:
      raise IOError("Seek reached EOF before dest?")

  def write(self, s):
    self.fh.write(s)

  def writeComments(self, extra=None):
    #if len(self.comments):
    #  self.writelines(self.comments)
    if extra:
      self.writelines(extra)
      self.comments.append(extra)

  def reopen(self, direction=None):
    if self.fh:
      print "Closing and reopening file"
      traceback.print_stack()
      self.close()
    return self.openfile(direction)

  def getComments(self):
    """ Assumes comments are all at start of file. Stops at first non-comment found """
    if len(self.comments) > 0:
      return self.comments

    self.seek(0)
    for line in self.fh.readline():
      if line[0] == '#':
        self.comments.append(line)
        self.pos += len(line)
      else:
        self.seek(self.pos) # rewind to end of comments
        break

    # current position left at end of comments
    return self.comments

  def getIndex(self):
    # Force it to be loaded or generated
    index = self.index.getIndex()

    if len(index) == 0:
      #  self.buildIndex()
      # NB : Must change approach?
      return None

    return self.index

  def buildIndex(self):
    if self.tell() != 0:
      fh = self.reopen()

    t0, t1 = time.time(), time.clock()
    print " *** Building Index on: " + self.filename

    currentpanel = None
    currentpanelstart = None
    currentpanelend = None
    readcount = 0

    # Force reading line by line
    self.index.complete = False

    for pdata in self:
      # Iteration automatically indexes
      pass

    self.index.complete = True
    self.index.writeIndex()

    print "Build index: %f ms, %f ms" % ( time.time() - t0, time.clock() - t1 )
    # self.reopen()
    return self.index

#======================================================================

class IndexEntry:
  #__slots__ = [ 'panel', 'reads', 'start', 'end' ]
  def __init__(self, panel, reads, start, end=None):
    self.panel = panel
    self.reads = reads
    self.start = start
    self.end = end

  def __cmp__(self, b):
    if b is None:
      return cmp(self.panel, None)
    return cmp(self.panel, b.panel)

  def __repr__(self):
    return "IndexEntry(%s, %s, %s, %s)" % (self.panel, self.reads, self.start, self.end)

  def asTuple(self):
    return (self.panel, self.reads, self.start, self.end)

#======================================================================

class ReadIndex:
  indexextension = ".idx"

  def __init__(self, basefilename):
    self.sourcefile = basefilename
    self.indexname = self.getIndexName()
    self.index = {}
    self.complete = False

  def __iter__(self):
    for p in self.getPanels():
      yield self.index[p]

  def isEmpty(self):
    return (len(self.index) == 0)

  def isComplete(self):
    return self.complete

  def getPanels(self):
    panels = self.index.keys()
    panels.sort()
    return panels

  def getPanel(self, panel):
    if panel in self.index:
      return self.index[panel]
    else:
      return None

  def getNextPanel(self, prevpanel):
    if not prevpanel in self.index:
      return None
    panels = self.getPanels()
    previdx = panels.index(prevpanel)
    nextidx = previdx + 1
    nextpanel = panels[nextidx]
    return self.getPanel(nextpanel)

  def writeIndex(self):
    idx = open(self.indexname, "w")
    idx.write("# Index created as %s" % (self.indexname) + "\n")
    idx.write("# Generated by split_solid_reads.py\n")
    idx.write("# PanelId Num_beads_in_panel FilePositionStart FilePositionEnd\n")

    ### write index
    # (currentpanel, readcount, currentpanelstart, currentpanelend)
    self.verify()

    for entry in self.index.itervalues():
      (panel, readcount, currentpanelstart, currentpanelend) = entry.asTuple()
      idx.write("%d %d %d %d\n" % (panel, readcount, currentpanelstart, currentpanelend))
    idx.close()

  def append(self, *t):
    if len(t) == 3:
      (panel, readcount, currentpanelstart) = t
      currentpanelend = -1 # None
    elif len(t) == 4:
      (panel, readcount, currentpanelstart, currentpanelend) = t
    else:
      raise ValueError("Unknown index data format: " + str(t))

    self.index[panel] = IndexEntry(panel, readcount, currentpanelstart, currentpanelend)
    #print self.index[panel]
    return self.index[panel]

  def verify(self):
    for x in self:
      if not x.end:
        continue
      try: 
        next = self.getNextPanel(x.panel)
        x.end = next.start
      except IndexError, KeyError:
        x.end = -1

  def getIndex(self):
    if len(self.index) == 0:
      try:
        self.loadIndex()
      except IOError:
        sys.stderr.write("No index found\n")
    return self.index


  def loadIndex(self):
    indexname = self.getIndexName()
    updated = 0

    ## Load from file. Let IOError be thrown to caller
    idx = open(indexname, "r")
    print(" * Reading index from file: %s" % (indexname))
    try:
      lastdata = None
      for line in idx:
        if line[0]!='#':
          values = line.split()
          if len(values) == 4:
            ## Format with end position
            (panel, reads, start, end) = map(int, values)
            self.append(panel, reads, start, end)
          elif len(values) == 3:
            ## Format with only start positions
            ## Fake internal format with end positions too
            if lastdata != None:
              updated = 1
              (panel, reads, start) = map(int, lastdata)
              end = int(values[2])
              self.append(panel, reads, start, end)
            lastdata = values
      if lastdata != None:
        (panel, reads, start) = map(int, lastdata)
        end = -1
        self.append(panel, reads, start, end)
        lastdata = None
    finally:
      if not idx is None:
        idx.close()
    # Refresh from 3 columns to 4 column format
    if updated:
      self.writeIndex()
    self.complete = True
    return self

  def getIndexName(self):
    #(base, ext) = os.path.splitext(self.sourcefile)
    isGZ = self.sourcefile.rfind(".gz")
    if isGZ >= 0:
      """ Index is on contents, not gz, so use base name """
      f = self.sourcefile.replace(".gz","")
    else:
      f = self.sourcefile
    return f + self.indexextension



##======================================================================

def chunkInterval(r, n):
  """
  Given a range of numbers, break into n chunks at n*c, upper inclusive

  >>> [ c for c in chunkInterval([ 1,2,4,5,6,8,9 ], 3) ]
  [[1, 2], [4, 5, 6], [8, 9]]

  Range does not have to be continuous. Resulting sets may be different sizes...
  r must be sorted. It would break an iterator to sort it here.
  """
  a = [];
  prev = None
  maxv = n + 1

  for v in r:
    # Sanity check
    if(v <= prev):
      raise ValueError("range not sorted or not unique")
      prev = v

    if (v >= maxv):
      yield a
      maxv += n
      a = []

    a.append(v)

  yield a


##======================================================================
def chunkSize(r, n):
  """
    >>> [ c for c in chunkSize(xrange(1,11),2) ]
    [[1, 2], [3, 4], [5, 6], [7, 8], [9, 10]]

    >>> [ c for c in chunkSize(xrange(1,22,2),3) ]
    [[1, 3, 5], [7, 9, 11], [13, 15, 17], [19, 21]]

  """
  cur = None;
  for c in xrange(0,len(r), n):
    if not cur is None:
      yield [ r[idx] for idx in range(cur,c) ]
    cur = c
  yield [ r[idx] for idx in range(cur,len(r))]

##======================================================================
def splitFile(f, chunksize = 25, start=None, end=None, resume=None):
  read = ReadFile(f, 'rb')
  # Seed comments
  comments = read.getComments()
  
  start = start or 1
  end = end or 9999

  ## If index:
  index = read.getIndex()
  haveIndex = index.isComplete()
  if haveIndex:
    print " * Split using existing index"
    panels = index.getPanels()
    # Use start, not min(panels), so chunk numbers are consistent
    r = xrange(start, min(end, max(panels)))
  else:
    print " * Split directly, generate index for future access"
    # all inclusive range
    r = xrange(start, end)

  t0, t1 = time.time(), time.clock()
  print " * Begin splitting file on panel boundaries"

  nextPanel = None
  cNum = 0
  readiter = iter(read)
  for c in chunkInterval(r, chunksize):
    cNum += 1
    rstart, rend = c[0], c[-1]

    if resume and resume > cNum:
      continue
    
    if nextPanel and not (nextPanel.panel in c):
        # shortcircuit - Skip empty ranges
        continue

    print "Chunk: %d ( %d -> %d )" % (cNum, rstart, rend)
    #print repr(c)

    cStr = "%04d.%04d_%04d" % (cNum, rstart, rend)
    workDir = "./set%04d" % (cNum)

    (dir, base) = os.path.split(read.filename)    
    (base,ext) = os.path.splitext(base)
    if ext == '.gz':
      (base,ext) = os.path.splitext(base)

    # Need something extra for intensity files
    p = re.match(r"(.*_intensity)(\.Scaled.*)", base)
    if p:
      base, extprefix = p.groups()
      ext = extprefix + ext
  
    outfile = os.path.join(workDir, "%s.%s%s" % (base, cStr, ext))

    try:
      nextPanel = writePanels(read, readiter, c, outfile, cStr)
    except IndexError:
      traceback.print_stack()
      sys.stderr.write("No more panels in file\n")
      break

  
  sys.stderr.write("Split Complete\n")
      
  # Didn't have an index before, but it's complete now
  if not haveIndex and (start == 1 and end == 9999):
    # and read.index.isComplete():
    #read.index.complete = True
    read.index.writeIndex()
    
  print "Parse complete: %f ms, %f ms" % ( time.time() - t0, time.clock() - t1 )


def writePanels(readfile, readiter, c, outfile, name=None):

  rstart, rend = c[0], c[-1]
  
  if readfile.index.isComplete():
    # Short circuit without reading file...
    panels = readfile.index.getPanels()
    if rstart > max(panels):
      # start is outsize range.
      return
    if rend < min(panels):
      # end is before range
      return

    firstPanel = readfile.index.getPanel(rstart)
    if firstPanel:
      sys.stderr.write("Seeking to panel %d at %d (current %d) ..." % \
                       (firstPanel.panel, firstPanel.start, readfile.tell()))
      readfile.seek(firstPanel.start)
      sys.stderr.write(" OK\n")
      readfile.currentpanel = firstPanel.panel

  out = None
  #nextPanel = readfile.index.getNextPanel(rstart)
  #nextPanel = rstart
  nextPanel = None
  #if nextPanel > rend:
  #  sys.stderr.write("First panel beyond range: %d - skipping chunk\n" % (nextPanel))
  #  return

  for pdata in readiter:
    if pdata.panel < rstart:
      sys.stderr.write("Skipping panel: %d\n" % (pdata.panel))
      continue

    if pdata.panel > rend:
      ## Oops. Went to far. This could be a problem.
      ## YES - PROBLEM in empty chunks - consumes first panel of next chunk.
      sys.stderr.write("Got extra panel: %d\n" % (pdata.panel))
      break

    if not out:
      workDir, fname = os.path.split(outfile)
      if workDir and not os.path.exists(workDir):
        os.mkdir(workDir, 0775)
      sys.stderr.write("Writing chunk to: '%s'\n" % outfile)
      out = ReadFile(outfile, 'w')
      out.openfile('w')
      out.fh.writelines(readfile.getComments())
      if name:
        out.fh.writelines("# split_solid_reads.py -- %s\n" % name)

    # write panel worth of data
    sys.stderr.write("Writing data for panel %d (%d)\n" % (pdata.panel, len(pdata.data)))
    sys.stderr.write("DEBUG: %s" % pdata.data[0:10])
    out.fh.writelines(pdata.data)
    # Peek at what next panel is
    nextPanel = readfile.index.getNextPanel(pdata.panel)
    # This will raise IndexException if there is no next panel

    if nextPanel.panel > rend:
      # Panel is past wanted range. This is where we should break.
      break

  # close file
  if out:
    sys.stderr.write("... complete.\n")
    out.close()

  return nextPanel


def getPanelRange(panels, c):

  start = None
  end = None

  for n in c:
    if n in panels:
      start = n
      break

  for n in panels[panels.index(start):]:
    if n > c[-1]:
      end = n
      break

  return (start, end)


##======================================================================

##======================================================================
if __name__ == "__main__":
  from optparse import OptionParser
  parser = OptionParser();
  parser.add_option("-v", "--verbose", dest="verbose", action="count",
                    help="Verbose output")
  parser.add_option("-s", "--start", dest="start", type="int",
                    help="Starting panel in range")
  parser.add_option("-e", "--end", dest="end", type="int",
                    help="Ending panel in range")
  parser.add_option("-c", "--chunk", dest="chunk", type="int",
                    help="Chunk size for splitting")
  parser.add_option("-r", "--resume", dest="resume", type="int",
                    help="Resume from chunk")
  parser.add_option("-o", "--output", dest="output", type="string",
                    help="Outputfile name. Use %(s) with chunks")

  (opt, args) = parser.parse_args()

  print args
  cmd = args.pop(0)
  v = args
  print cmd
  print v

  print "Action: " + cmd
  if cmd == 'index':
    for f in v:
      print cmd + ": " + f
      read = ReadFile(f,'rb')
      read.buildIndex()
  elif cmd == 'test':
    print cmd + ": selftest"
    import doctest
    doctest.testmod()
  elif cmd == 'split' or cmd == 'chunk':
    if not opt.chunk:
      sys.stderr.write("Usage: split --chunk=<n>\n")
      exit(1)
    print cmd + ": chunk size " + str(opt.chunk)
    for f in v:
      if f[-4:] == '.idx':
        continue
      if f[-6:] == '.stats':
        continue
      print cmd + ": " + f
      splitFile(f, chunksize=opt.chunk, start=opt.start, end=opt.end, resume=opt.resume)
      
  elif cmd == 'range':
    startpanel = opt.start
    endpanel = opt.end
    print "Panel range: %d -> %d" % (startpanel, endpanel)
    outfile = opt.output
    if not (startpanel and endpanel and outfile):
      sys.stderr.write("Usage: range --start=<n> --end=<n> --output=<file>\n")
      exit(1)
    sys.stderr.write("Working on files: %s\n" % v)
    for f in v:
        read = ReadFile(f, 'rb')
        if opt.verbose:
            read.verbose = opt.verbose
        index = read.getIndex()
        if not index or not index.isComplete():
            sys.stderr.write("Error, must index file first to specify range\n")
            exit(1)

        panels = index.getPanels()

        c = [ n for n in xrange(startpanel, endpanel) if n in panels ]
        if opt.verbose:
            sys.stderr.write("Panels in range: %s" % c)
        writePanels(read, iter(read), c, outfile, name="_".join(sys.argv[:]))

  else:
    print "Unknown command: " + cmd
