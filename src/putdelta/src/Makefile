# $Id: Makefile,v 6.3 1994/07/15 09:57:03 tom Exp $
# Make-file for SCCS check-in utilities

TOP	= ../../../..
include $(TOP)/td_lib/support/td_lib.mk

####### (Standard Lists) #######################################################
ALL	= $B/putdelta $B/sccsput
C_SRC	= putdelta.c sccsput.c
SOURCES	= $(C_SRC)
MAIN_O	= putdelta.o $(LIBS)
TREE_O	= sccsput.o $(LIBS)

####### (Standard Productions) #################################################
all\
install:	$B $(SOURCES) $(ALL)
clean:				; rm -f $(CLEAN)
clobber:	clean		; rm -f $(ALL)
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
lincnt.out:	$(C_SRC)	; $(DATE); lincnt $(C_SRC) >>$@
