bin_PROGRAMS = SRF_WriteGenSOLiD

SRF_WriteGenSOLiD_SOURCES = \
                            SRF_WriteGenSOLiD.cc \
                            SRF_CommandLineArgs.hh \
                            SRF_CommandLineArgs.cc \
                            SRF_SOLiDstructs.hh \
                            SRF_SOLiDfile.hh \
                            SRF_SOLiDfile.cc \
                            SRF_SOLiDcfastaFile.hh \
                            SRF_SOLiDcfastaFile.cc \
                            SRF_SOLiDqualFile.hh \
                            SRF_SOLiDqualFile.cc \
                            SRF_SOLiDintensityFiles.hh \
                            SRF_SOLiDintensityFiles.cc \
                            SRF_SOLiDWriter.hh \
                            SRF_SOLiDWriter.cc \
                            SRF_SOLiDsingleEndWriter.hh \
                            SRF_SOLiDsingleEndWriter.cc \
                            SRF_SOLiDpairedEndWriter.hh \
                            SRF_SOLiDpairedEndWriter.cc
SRF_WriteGenSOLiD_LDADD = $(top_builddir)/SRF/base/libsrf.a  $(top_builddir)/ZTR/src/libztr.a @LDFLAGS@ $(LDLIB)

INCLUDES= -I. -I../base -I../../ZTR/src

LDLIB=-lstdc++

