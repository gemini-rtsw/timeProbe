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
timeProbe_DBD += base.dbd
# List local .dbd files here
# timeProbe_DBD += xxx.dbd

# Include dbd files for external support modules:
# timeProbe_DBD += genSubRecord.dbd

# specify all source files to be compiled
# including sequencer (.st) source files
#timeProbe_SRCS += xxx.c
#timeProbe_SRCS += xxx.st

#=============================

include $(TOP)/configure/RULES
#----------------------------------------
#  ADD RULES AFTER THIS LINE

