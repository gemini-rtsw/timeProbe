TOP=../..

include $(TOP)/configure/CONFIG
#----------------------------------------
#  ADD MACRO DEFINITIONS AFTER THIS LINE

#=============================
# Build a support module called <APPNAME>

LIBRARY_IOC += timeProbe

# <APPNAME>.dbd will be created and installed
DBD += timeProbe.dbd

# timeProbe.dbd will be made up from these files:
timeProbe_DBD += bancomm.dbd 
timeProbe_DBD += geminiRecords.dbd
timeProbe_DBD += timeProbeFuncs.dbd

# specify all source files to be compiled
# including sequencer (.st) source files
timeProbe_SRCS += timeProbe.c

#=============================

include $(TOP)/configure/RULES
#----------------------------------------
#  ADD RULES AFTER THIS LINE

