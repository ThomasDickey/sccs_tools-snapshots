# $Id: Makefile,v 6.1 1993/09/23 20:03:11 dickey Exp $
# Make-file for sccs/RCS conversion utility

TOP	= ../../../..
include $(TOP)/td_lib/support/td_lib.mk

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
run_test:
sources:	$(SOURCES)

####### (Details of Productions) ###############################################
$B/sccs2rcs:	$(MAIN_O)	; $(LINK) -o $@ $(MAIN_O)
sccs2rcs.o:	$(PTYPES_H) $(RCSDEFS_H)
#
####### (Development) ##########################################################
$(SOURCES):			; $(GET) $@
lint.out:	$(CSRC)		; $(DATE); tdlint $(LINTOPTS) $? >>$@
lincnt.out:	$(CSRC)		; $(DATE); lincnt $(CSRC) >>$@
