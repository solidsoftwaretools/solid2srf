#!python

import sys, string, getopt, re
import tables, numpy

class BeadExport:
  def __init__(self):
    self.h5file = None
    
  def openHDF(self, fname):
    if self.h5file != None:
      self.closeHDF()
    self.h5file = tables.openFile(fname, mode = "r")
    if self.h5file == None:
      raise Exception("ERROR opening SPCH file")

  def closeHDF(self):
    self.h5file.close()
    self.h5file = None

  def processSPCH(self, spch):
    self.openHDF(spch)
    self.processPanel()    
    self.closeHDF()

  def processPanel(self):
    panelsgroup = self.getGroup("/Panels")

    # Don't want to walk panels, but do want to walk all panels.
    for panelnode in self.h5file.iterNodes(panelsgroup, 'Group'):
      panelpath = "/Panels/" + panelnode._v_name
      #beadsP2Node = self.getGroup(panelpath + "/RefData/BeadsP2")
        
      yxLocationsNode = self.getGroup(panelpath + "/RefData/BeadsP2/yxLocation");
      rawValueNode = self.getGroup(panelpath + "/RefData/BeadsP2/rawValue");

      yxLocation = self.getData(yxLocationsNode)
      rawValue = self.getData(rawValueNode)

      print yxLocation
      print rawValue
      
      for idx in xrange(1,yxLocation.shape[1]):
        name = '_'.join([panelnode._v_name, str(yxLocation[1,idx]), str(yxLocation[0,idx])])
        print ', '.join([name, str(rawValue[idx])])

  def getData(self, leaf):
    if (isinstance(leaf, tables.Table) or isinstance(leaf, tables.Array)) and leaf.shape != ():
      # Output any data fields with max size 4x4
      log("Data: %s" % ( repr(leaf) ), 1)
      return leaf
    else:
      sys.err.write("ERROR: Unsupported HDF5 Node type: " + str(leaf))
    return None

  def getGroup(self, name="/"):
    return self.h5file.getNode(name)



loglevel = 4

def log(msg, level=3):
  if(level >= loglevel):
    sys.stderr.write(msg + "\n")

import re
panelnumbers = re.compile("_\d{4}\.spch")
def panelnumsort(a, b):
  anum = panelnumbers.match(a)
  bnum = panelnumbers.match(b)
  if anum and bnum:
    return cmp(anum.group(), bnum.group())
  # else:
  return cmp(a, b)
  

if __name__=='__main__':
  log(sys.argv[0] + " SPCH Bead Data export", 5)
  #log(" Version: %VERSION%, Copyright 2008 Applied Biosystems")

  from optparse import OptionParser
  from glob import glob
  import os

  parser = OptionParser();
  parser.add_option("-v", "--verbose", dest="verbose", action="count",
            help="Verbose output")

  # TODO: Options to implement
  # output (to file, instead of stdout)

  (options, args) = parser.parse_args()

  if(options.verbose):
    # NB This is backwards. Fixme with a real logger
    loglevel -= options.verbose
    log("Loglevel: %d" % loglevel, 4)

  conv = BeadExport()

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
      allspch.sort(cmp=panelnumsort)
      [ spchfiles.append(f) for f in allspch ]
    else:
      if os.path.exists(d):
        spchfiles.append(d)
      else:
        log("ERROR: Specified file doesn't exist",5)

  for spch in spchfiles:
    log("Processing SPCH: '%s'" % spch,3)
    conv.processSPCH(spch)
    print '\n'
    sys.stdout.flush()

  conv.close()

