# $Id: Makefile,v 6.1 1993/09/23 19:47:28 dickey Exp $
# Make-file for SCCS check-in utilities

TOP	= ../../../..
include $(TOP)/td_lib/support/td_lib.mk

####### (Standard Lists) #######################################################
DEFINES	= -DGET_TOOL=\"`which get`\"
ALL	= $B/getdelta $B/sccsget
C_SRC	= getdelta.c sccsget.c
SOURCES	= $(C_SRC)
MAIN_O	= getdelta.o $(LIBS)
TREE_O	= sccsget.o $(LIBS)

####### (Standard Productions) #################################################
all\
install:	$(SOURCES) $(ALL)
clean:				; rm -f $(CLEAN)
clobber:	clean		; rm -f $(ALL)
destroy:			; $(DESTROY)
run_test:
sources:	$(SOURCES)

####### (Details of Productions) ###############################################
$B/getdelta:	$(MAIN_O)	; $(LINK) -o $@ $(MAIN_O)
getdelta.o:	$(PTYPES_H) $(SCCSDEFS_H)
#
$B/sccsget:	$(TREE_O)	; $(LINK) -o $@ $(TREE_O)
sccsget.o:	$(PTYPES_H) $(RCSDEFS_H) $(SCCSDEFS_H)
#
####### (Development) ##########################################################
$(SOURCES):			; $(GET) $@
lint.out:	$(C_SRC)	; $(DATE); $(LINT_EACH)
lincnt.out:	$(C_SRC)	; $(DATE); lincnt $(C_SRC) >>$@
