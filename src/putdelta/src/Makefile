# $Id: Makefile,v 6.4 1994/07/21 00:43:50 tom Exp $
# Make-file for SCCS check-in utilities

TOP	= ../../../..
include $(TOP)/td_lib/support/td_lib.mk

####### (Standard Lists) #######################################################
ALL	= $B/putdelta $B/sccsput
C_SRC	= putdelta.c sccsput.c
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
$B/putdelta:	putdelta.o $(LIB_DEPS)	; $(LINK) -o $@ putdelta.o $(LIB_ARGS)
putdelta.o:	$(PTYPES_H) $(SCCSDEFS_H)

$B/sccsput:	sccsput.o $(LIB_DEPS)	; $(LINK) -o $@ sccsput.o $(LIB_ARGS)
sccsput.o:	$(PTYPES_H) $(RCSDEFS_H) $(SCCSDEFS_H)

####### (Development) ##########################################################
$(SOURCES):			; $(GET) $@
lint.out:	$(C_SRC)	; $(DATE); $(LINT_EACH)
