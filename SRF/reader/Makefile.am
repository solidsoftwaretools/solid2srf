bin_PROGRAMS = SRF_Reader SRF_ZtrReader SRF_FastLikeConverter SRF_FastqConverter

SRF_Reader_SOURCES = SRF_Reader.cc
SRF_Reader_LDADD = $(top_builddir)/SRF/base/libsrf.a  $(top_builddir)/ZTR/src/libztr.a @LDFLAGS@ $(LDLIB)

SRF_ZtrReader_SOURCES = SRF_ZtrReader.cc
SRF_ZtrReader_LDADD = $(top_builddir)/SRF/base/libsrf.a  $(top_builddir)/ZTR/src/libztr.a @LDFLAGS@ $(LDLIB)

SRF_FastLikeConverter_SOURCES = SRF_FastLikeConverter.cc
SRF_FastLikeConverter_LDADD = $(top_builddir)/SRF/base/libsrf.a  $(top_builddir)/ZTR/src/libztr.a @LDFLAGS@ $(LDLIB)

SRF_FastqConverter_SOURCES = SRF_FastqConverter.cc
SRF_FastqConverter_LDADD = $(top_builddir)/SRF/base/libsrf.a  $(top_builddir)/ZTR/src/libztr.a @LDFLAGS@ $(LDLIB)

INCLUDES= -I$(top_srcdir)/SRF/base -I$(top_srcdir)/ZTR/src

LDLIB=-lstdc++

