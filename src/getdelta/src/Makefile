# $Id: Makefile,v 6.4 1994/07/21 00:43:39 tom Exp $
# Make-file for SCCS check-in utilities

TOP	= ../../../..
include $(TOP)/td_lib/support/td_lib.mk

####### (Standard Lists) #######################################################
ALL	= $B/getdelta $B/sccsget
C_SRC	= getdelta.c sccsget.c
SOURCES	= $(C_SRC)

####### (Standard Productions) #################################################
all\
install:	$B $(SOURCES) $(ALL)
clean:				; $(RM) $(CLEAN)
clobber:	clean		; $(RM) $(ALL)
destroy:			; $(DESTROY)
run_test:
sources:	$(SOURCES)

####### (Details of Productions) ###############################################
$B:				; mkdir $@
$B/getdelta:	getdelta.o $(LIB_DEPS)	; $(LINK) -o $@ getdelta.o $(LIB_ARGS)
getdelta.o:	$(PTYPES_H) $(SCCSDEFS_H)

$B/sccsget:	sccsget.o $(LIB_DEPS)	; $(LINK) -o $@ sccsget.o $(LIB_ARGS)
sccsget.o:	$(PTYPES_H) $(RCSDEFS_H) $(SCCSDEFS_H)

####### (Development) ##########################################################
$(SOURCES):			; $(GET) $@
lint.out:	$(C_SRC)	; $(DATE); $(LINT_EACH)
