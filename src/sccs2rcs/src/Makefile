# $Id: Makefile,v 6.3 1994/07/21 00:44:22 tom Exp $
# Make-file for sccs/RCS conversion utility

TOP	= ../../../..
include $(TOP)/td_lib/support/td_lib.mk

####### (Standard Lists) #######################################################
THIS	= sccs2rcs
ALL	= $B/$(THIS)
CSRC	= $(THIS).c
SOURCES	= $(CSRC)

####### (Standard Productions) #################################################
all:	$B $(SOURCES) $(ALL)
clean:				; $(RM) $(CLEAN)
clobber:	clean		; $(RM) $(ALL)
destroy:			; $(DESTROY)
run_test:
sources:	$(SOURCES)

####### (Details of Productions) ###############################################
$B:				; mkdir $@
$B/$(THIS):	$(THIS).o $(LIB_DEPS)	; $(LINK) -o $@ $(THIS).o $(LIB_ARGS)
$(THIS).o:	$(PTYPES_H) $(RCSDEFS_H)
#
####### (Development) ##########################################################
$(SOURCES):			; $(GET) $@
lint.out:	$(CSRC)		; $(DATE); $(LINT) $(LINTOPTS) $? >>$@
