#!/usr/bin/env python2.5

import sys, string, getopt, re
from xml.sax.saxutils import XMLGenerator
from xml.sax.xmlreader import AttributesImpl
import tables, numpy

class SpchToXml:

	def __init__(self):
		self.h5file = None
		# build xml structure
		self.xmldoc = XMLGenerator(sys.stdout, 'utf-8')
		self.xmldoc.startElement(u'SOLiD', AttributesImpl({}))
		self.haverundata = False
		self.xmlpanels = None

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
		self.buildNodeWithAttrs(name, group._v_attrs)

		# Walk leaf nodes before child groups
		for leaf in self.h5file.iterNodes(group, 'Leaf'):
			self.processLeaf(leaf)

		# Recursively walk child groups
		for subgroup in self.h5file.iterNodes(group, 'Group'):
			self.walkGroup(subgroup)

		self.xmldoc.endElement(unicode(name))

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
				log("Leaf with array: %s" % str(leaf),3)
				(s, data) = self.processArray(leaf)
				self.xmldoc.startElement(u'data', {u'shape': s})
				self.xmldoc.characters(unicode(data))
				self.xmldoc.endElement(u'data')
		else:
			sys.err.write("ERROR: Unsupported HDF5 Node type: " + str(leaf))

		self.xmldoc.endElement(unicode(leaf.name))

	def close(self):
		print '\n'
		self.xmldoc.endElement(u'Panels')
		self.xmldoc.endElement(u'SOLiD')
		self.xmldoc.endDocument()
		print '\n'
		sys.stdout.flush()


loglevel = 2

def log(msg, level=3):
	if(level >= loglevel):
		sys.stderr.write(msg + "\n")


if __name__=='__main__':

	"""
	Parse cmdline options
	Instantiate converter
	"""

	log(sys.argv[0] + " SPCH to XML metadata export", 5)
	#log(" Version: %VERSION%, Copyright 2008 Applied Biosystems")

	conv = SpchToXml()

	for spch in sys.argv[1:]:
		log("Processing SPCH: '%s'" % spch,2)
		conv.processSPCH(spch)
		print '\n'
		sys.stdout.flush()

	conv.close()

