# $Id: Makefile,v 5.1 1992/10/19 11:11:25 dickey Exp $
# Make-file for sccs/RCS conversion utility

TOP	= ../../../..
include $(TOP)/cm_library/support/cm_library.mk

####### (Standard Lists) #######################################################
ALL	= $B/sccs2rcs
CSRC	= sccs2rcs.c
SOURCES	= $(CSRC)
MAIN_O	= sccs2rcs.o $(LIBS)

####### (Standard Productions) #################################################
all:	$(SOURCES) $(ALL)
clean:				; rm -f $(CLEAN)
clobber:	clean		; rm -f $(ALL)
destroy:			; $(DESTROY)
run_tests:
sources:	$(SOURCES)

####### (Details of Productions) ###############################################
$B/sccs2rcs:	$(MAIN_O)	; $(CC) -o $@ $(MAIN_O)
sccs2rcs.o:	$(PTYPES_H) $(RCSDEFS_H)
#
####### (Development) ##########################################################
$(SOURCES):			; $(GET) $@
lint.out:	$(CSRC)		; $(DATE); tdlint $(LINTOPTS) $? >>$@
lincnt.out:	$(CSRC)		; $(DATE); lincnt $(CSRC) >>$@
