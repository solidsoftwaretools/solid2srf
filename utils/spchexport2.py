#!/usr/bin/env python2.5

import sys, string, getopt, re
from xml.sax.saxutils import XMLGenerator
from xml.sax.xmlreader import AttributesImpl
import tables, numpy

class SpchToXml:

  def __init__(self, outfh=sys.stdout):
    self.h5file = None
    # build xml structure
    self.outfh = outfh
    self.xmldoc = XMLGenerator(outfh, 'utf-8')
    self.xmldoc.startElement(u'SOLiD', AttributesImpl({}))
    self.haverundata = False
    self.xmlpanels = None
    self.filterTag = None

  def setFilterTag(self,tag):
    if tag != None:
      self.filterTag = tag.upper()

  def openHDF(self, fname):
    if self.h5file != None:
      self.closeHDF()
    self.h5file = tables.openFile(fname, mode = "r")

  def closeHDF(self):
    self.h5file.close()
    self.h5file = None

  def processSPCH(self, spch):
    self.openHDF(spch)
    if not self.haverundata:
      self.processHeader()
    self.processPanel()
    self.closeHDF()

  def processHeader(self):
    self.walkGroup(self.getGroup("/Header"))
    self.processLeaf(self.getGroup("/Description"))
    self.haverundata = True
    log("Processed Header and Description",2)
    # After header, start panel block
    self.xmldoc.startElement(u'Panels', AttributesImpl({}))

  def processPanel(self):
    panelsgroup = self.getGroup("/Panels")
    # Don't want to walk panels, but do want to walk all panels.
    for node in self.h5file.iterNodes(panelsgroup, 'Group'):
      self.walkGroup(node, 'panel_' + node._v_name)

  def getGroup(self, name="/"):
    return self.h5file.getNode(name)

  def walkGroup(self, group, name=None):
    log("Group: " + str(group),2)
    if name == None:
      name = group._v_name

    # SPCH specific mutation of node name to valid xml tag name
    # 01_V1 - xml node names must not begin with numeric value
    cyclere = re.compile("^\d{2}_V\d+$")
    if cyclere.match(name):
      name = "C_" + name

    # start group add add attributes
    # NB calls startElement on name
    self.buildNodeWithAttrs(name, group._v_attrs)

    # Walk leaf nodes before child groups
    for leaf in self.h5file.iterNodes(group, 'Leaf'):
      self.processLeaf(leaf)

    # Recursively walk child groups
    for subgroup in self.h5file.iterNodes(group, 'Group'):
      if(self.filterTag and subgroup._v_name in self.filterTag):
        continue
      self.walkGroup(subgroup)

    self.xmldoc.endElement(unicode(name))
    self.xmldoc._out.write("\n")


  def buildNodeWithAttrs(self, name, attrs):
    """ Returns attribute node, plus children """
    (nodeattr, childnodes) = self.processAttrs(attrs)

    xmlattr = AttributesImpl(nodeattr)
    self.xmldoc.startElement(name, xmlattr)
    for (name, arr) in childnodes.iteritems():
      (s, value) = arr
      self.xmldoc.startElement(unicode(name), AttributesImpl({u'shape': unicode(s)}))
      self.xmldoc.characters(unicode(value))
      self.xmldoc.endElement(unicode(name))
      self.xmldoc._out.write("\n")


  def processAttrs(self, attrs):
    if attrs == None:
      return ({}, {})
    xmlattr = {}
    childnodes = {}
    for attrname in attrs._g_listAttr():
      attrvalue = attrs._g_getAttr(attrname)
      log("Attr '%s': %s" % (attrname, attrvalue) ,1)

      if attrvalue.shape == () or (len(attrvalue.shape) == 1 and attrvalue.shape[0] == 1):
        xmlattr[unicode(attrname)] = unicode(str(attrvalue[0]))
      else:
        childnodes[attrname] = self.processArray(attrvalue)

    return (xmlattr, childnodes)

  def processArray(self, arr):
    s = arr.shape
    data = None
    if len(s) > 1:
      # NB - The column major data saved by matlab in hdf5 is read as row major by numpy/pytables
      # This transpose corrects for that discrepancy and converts to true row major
      ret = numpy.transpose(arr)
      if ret != None:
        # NB 2.3/numarray will return, 2.5/numpy will transpose in place and return none
        arr = ret
      data = arr.flat
    else:
      data = arr
    strdata = " ".join(map(repr,data))
    return (s, strdata)


  def processLeaf(self, leaf):
    log("Leaf: %s" % str(leaf),1)
    attrs = leaf.attrs
    if (attrs == None) and (leaf.shape == ()):
      return

    self.buildNodeWithAttrs(leaf.name, attrs)

    if (isinstance(leaf, tables.Table) or isinstance(leaf, tables.Array)) and leaf.shape != ():
      # Output any data fields with max size 4x4
      log("Data: %s" % ( repr(leaf) ), 1)
      if len(leaf.shape) == 1 and leaf.shape[0] == 1:
        self.xmldoc.characters(unicode(str(leaf[0])))
      elif len(leaf.shape) <= 2 and max(leaf.shape) <= 4:
        log("Leaf with array: %s" % str(leaf),2)
        #(s, data) = self.processArray(leaf)
        #self.xmldoc.startElement(u'data', {u'shape': unicode(s) })
        #self.xmldoc.characters(unicode(data))
        #self.xmldoc.endElement(u'data')
        # NB This is usually a mistake - only happens when numbeads = 0
    else:
      sys.err.write("ERROR: Unsupported HDF5 Node type: " + str(leaf))

    self.xmldoc.endElement(unicode(leaf.name))
    self.xmldoc._out.write("\n")


  def close(self):
    self.xmldoc._out.write("\n")
    self.xmldoc.endElement(u'Panels')
    self.xmldoc.endElement(u'SOLiD')
    self.xmldoc.endDocument()
    self.xmldoc._out.write("\n")
    self.outfh.close()

##======================================================================
    

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
      raise Exception("range not sorted or not unique")
      prev = v

    if (v >= maxv):
      yield a
      maxv += n
      a = []

    a.append(v)

  yield a


##======================================================================

loglevel = 3

def log(msg, level=3):
  if(level >= loglevel):
    sys.stderr.write(msg + "\n")

##======================================================================

if __name__=='__main__':
  log(sys.argv[0] + " SPCH to XML metadata export", 5)
  #log(" Version: %VERSION%, Copyright 2008 Applied Biosystems")


  from optparse import OptionParser
  from glob import glob
  import os

  parser = OptionParser();
  parser.add_option("-x", "--exclude", dest="tag", type="string",
                    help="Limit output to a single tag")
  parser.add_option("-v", "--verbose", dest="verbose", action="count",
                    help="Verbose output")
  parser.add_option("-s", "--start", dest="start", type="int",
                    help="Starting panel in range")
  parser.add_option("-e", "--end", dest="end", type="int",
                    help="Ending panel in range")
  parser.add_option("-c", "--chunk", dest="chunk", type="int",
                    help="Chunk size")
  parser.add_option("-r", "--resume", dest="resume", type="int",
                    help="Resume from chunk")
  parser.add_option("-o", "--output", dest="output", type="string",
                    help="Outputfile name. Use %(s) with chunks")

  (opt, args) = parser.parse_args()

  if(opt.verbose):
    # NB This is backwards. Fixme with a real logger
    loglevel -= opt.verbose
    log("Loglevel: %d" % loglevel, 4)

  if not args:
    log("ERROR: No spch files specified",5)


  spchfiles = []
  for d in args:
    if os.path.isdir(d):
      log("Expanding path: %s" % d, 4)
      spchglob = os.path.abspath(d) + os.path.sep + \
                 "*" + os.path.extsep + "spch"
      log("Using glob: %s" % spchglob ,3)
      allspch = glob(spchglob) # NB Pure alphabetical
      log("Found %d spch files" % len(allspch), 4)
      if not allspch:
        log("ERROR: NO SPCH FILES FOUND", 5)    
      [ spchfiles.append(f) for f in allspch ]
    else:
      if os.path.exists(d):
        spchfiles.append(d)
      else:
        log("ERROR: Specified file doesn't exist",5)

  panelregex = re.compile(r"""_(\d{4})\.spch""")

  dspch = {}
  for spch in spchfiles:
    pm = panelregex.search(spch)
    if pm:
      pnum = int(pm.group(1))
      dspch[pnum] = spch
    else:
      raise Exception("Unable to parse panel number: '%s' -- '%s'" % (spch, pm) )

  # lookup table
  panel2chunk = {}
  chunks = []
  ## Need to chunk purely by panel numbers, not by index of spchfile
  if opt.start or opt.end or opt.chunk:
    start = opt.start or 1
    end = opt.end or 2999
    chunk = opt.chunk or 25
    panels = xrange(start, end)
    cnum = 0
    for c in chunkInterval(panels, chunk):
      cnum = cnum + 1
      cstr = "%04d.%04d_%04d" % (cnum, c[0], c[-1])
      chunks.append( (c, cstr, cnum) )
      for p in c:
        panel2chunk[p] = cnum


  spchpanels = dspch.keys()
  spchpanels.sort()
  
  if len(chunks) == 0:
    ## ?? How to handle single set?
    chunks.append( ( spchpanels, 'ALL', 0 ) )


  for c in chunks:

    (panels, name, cnum) = c

    if opt.resume and cnum < opt.resume:
      continue

    print "Processing: " + name
    
    numspch = 0
    for p in panels:
      if p in spchpanels:
        # validation pass
        print "Panel: %d" % p
        numspch = numspch + 1

    if numspch == 0:
      print "No spch files in chunk"
      continue

    ## prepare output file
    if opt.tag:
      name = name + "_" + opt.tag
      
    if len(chunks) > 1:
      outfile = "set%04d/%s.%s.metadata.xml" % ( cnum, opt.output, name )
    else:
      outfile = opt.output + ".metadata.xml"

    print "Writing data from %d spch files to xml: %s" % (numspch, outfile)
     
    outfh = open(outfile, 'w')
    conv = SpchToXml(outfh)
    if(opt.tag):
      conv.setFilterTag(opt.tag)
    
    for p in panels:
      spch = None
      if p in spchpanels:        
        spch = dspch[int(p)]
      else:
        continue
      
      log("Processing SPCH: '%s'" % spch,3)
      try:
        conv.processSPCH(spch)
      except KeyboardInterrupt:
        raise
      except:
        log("BAD SPCH?: '%s'" % spch, 5)        

    conv.close()
    outfh.close()

print "All chunks processed"
