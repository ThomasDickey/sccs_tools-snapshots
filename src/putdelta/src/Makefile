# $Id: Makefile,v 6.1 1993/09/23 19:51:56 dickey Exp $
# Make-file for SCCS check-in utilities

TOP	= ../../../..
include $(TOP)/td_lib/support/td_lib.mk

####### (Standard Lists) #######################################################
DEFINES	= -DADMIN_TOOL=\"`which admin`\" -DDELTA_TOOL=\"`which delta`\"
ALL	= $B/putdelta $B/sccsput
C_SRC	= putdelta.c sccsput.c
SOURCES	= $(C_SRC)
MAIN_O	= putdelta.o $(LIBS)
TREE_O	= sccsput.o $(LIBS)

####### (Standard Productions) #################################################
all\
install:	$(SOURCES) $(ALL)
clean:				; rm -f $(CLEAN)
clobber:	clean		; rm -f $(ALL)
destroy:			; $(DESTROY)
run_test:
sources:	$(SOURCES)

####### (Details of Productions) ###############################################
$B/putdelta:	$(MAIN_O)	; $(LINK) -o $@ $(MAIN_O)
putdelta.o:	$(PTYPES_H) $(SCCSDEFS_H)

$B/sccsput:	$(TREE_O)	; $(LINK) -o $@ $(TREE_O)
sccsput.o:	$(PTYPES_H) $(RCSDEFS_H) $(SCCSDEFS_H)

####### (Development) ##########################################################
$(SOURCES):			; $(GET) $@
lint.out:	$(C_SRC)	; $(DATE); $(LINT_EACH)
lincnt.out:	$(C_SRC)	; $(DATE); lincnt $(C_SRC) >>$@
