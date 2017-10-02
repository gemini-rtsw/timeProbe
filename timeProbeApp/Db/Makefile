TOP=../..
include $(TOP)/configure/CONFIG
#----------------------------------------
#  ADD MACRO DEFINITIONS AFTER THIS LINE

#----------------------------------------------------
# Find any .sym files in the Db directory
vpath %.sym ..

# Install database files created from .sch files
# using TDCT (or just install .db files) into <top>/db.
# TDCT_CFG can be redefined here if needed
#TDCT_CFG = ../tdct.cfg

# Make depends on all .sch files
DEPSCHS = $(wildcard ../*.sch)

# Add target .db files below here. Each must
# correspond to a .sch file
# DB += xxx.db


# If the module provides subschematics, install them and their
# symbol files in <top>/templates/schematics
TEMPLATES_DIR = schematics
#TEMPLATES += xxx.sch xxx.sym

# If .sym files for EPICS primitives are in this support module,
# simply install them into <top>/db.
# If you have .sym files for both primitives and subschematics,
# you'll need to list them specifically.
# DB += $(wildcard ../*.sym)


include $(TOP)/configure/RULES
#----------------------------------------
#  ADD RULES AFTER THIS LINE

clean::
        $(RM) *.dat *.bak

