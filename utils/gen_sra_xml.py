#!/usr/bin/env python

import sys, os
import glob
import md5
import re
from xml import minidom

srffiles = glob.glob('*.srf')

xmltype = 'run'
if len(sys.argv) > 1:
    xmltype = sys.argv[1]


if xmltype == 'run':

    flist = []
    if len(sys.argv) > 2:
        allmd5 = sys.argv[2]
        sys.stderr.write("Reading file: %s\n" % allmd5)

        for line in file(allmd5):
            (md5sum, srfname) = line.split(None)
            flist.append(srfname)

    else:
        flist = srffiles


    filepat = re.compile("([\w\d\_])+\.(\d{4})\.(\d{4})_(\d{4})\.")
    for srfname in flist:
        # Parse filename
        m = filepat.match(srfname)
        (name, partition, startpanel) = (m.group(0), m.group(2), m.group(3))
        name = name.rstrip('.')

        # FIXME - fake xml. Let's get some dom here.
        # Create xml node - one file per data block
        print '  <DATA_BLOCK name="%s" sector="%d" region="%d">' % (name, int(partition), int(startpanel))
        print "    <FILES>"
        print '      <FILE filetype="srf" filename="' + srfname + '"/>'
        print "    </FILES>"
        print "  </DATA_BLOCK>"

elif xmltype == 'submission':

    xmlfiles = glob.glob('*.xml')
    xmlentry = []
    print "  <ACTIONS>"
    for xmlname in xmlfiles:
        if xmlname.find('submission') >= 0:
            continue
        sys.stderr.write("XML: %s\n" % xmlname)

        m = md5.new()
        xmlfile = open(xmlname, 'r')
        for d in xmlfile.read():
            m.update(d)
        md5sum = m.hexdigest()

        # Misplaced
        sys.stdout.write('    <ACTION><ADD schema="' + xmlname[0:xmlname.index('_')] + \
                         '" source="' + xmlname + '"/></ACTION>' + "\n")
        xmlentry.append( (xmlname, md5sum) )

    print "  </ACTIONS>"
    print "  <FILES>"

    for entry in xmlentry:
        (xmlname, md5sum) = entry
        sys.stdout.write('    <FILE checksum_method="MD5" filename="' + xmlname + \
                         '" checksum="' + md5sum + '"/>' + "\n")

    md5data = []
    if len(sys.argv) > 2:
        allmd5 = sys.argv[2]
        sys.stderr.write("Reading file: %s\n" % allmd5)
        for line in file(allmd5):
            (md5sum, srfname) = line.split(None)
            md5data.append([md5sum, srfname])            
    else:
        for srf in srffiles:
            srfmd5 = srf + ".md5sum"
            if os.path.exists(srfmd5):
                data = file(srfmd5).readlines()
                (md5sum, srfname) = data[0].split(None)
                md5data.append([md5sum, srfname])


    for (md5sum, srfname) in md5data:
        sys.stdout.write('    <FILE checksum_method="MD5" filename="' + srfname + \
                         '" checksum="' + md5sum + '"/>' + "\n")

    print "  </FILES>"

else:
    sys.stderr.write("UNKNOWN run mode: %s\n" % xmltype)

# TODO xmllint validation of generated xml
# headers - get metadata from ?

