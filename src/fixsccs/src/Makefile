# $Id: Makefile,v 6.1 1994/06/07 12:39:22 dickey Exp $
# Make-file for SCCS timestamp utility

TOP	= ../../../..
include $(TOP)/td_lib/support/td_lib.mk

####### (Standard Lists) #######################################################
THIS	= fixsccs
ALL	= $B/$(THIS)
C_SRC	= $(THIS).c
SOURCES	= $(C_SRC)
MAIN_O	= $(THIS).o $(LIBS)

####### (Standard Productions) #################################################
all\
install:	$(SOURCES) $B $(ALL)
clean:				; rm -f $(CLEAN)
clobber:	clean		; rm -f $(ALL)
destroy:			; $(DESTROY)
run_test:
sources:	$(SOURCES)

####### (Details of Productions) ###############################################
$B:				; mkdir $@
$B/$(THIS):	$(MAIN_O)	; $(LINK) -o $@ $(MAIN_O)
$(THIS).o:	$(PTYPES_H) $(SCCSDEFS_H)

####### (Development) ##########################################################
$(SOURCES):			; $(GET) $@
lint.out:	$(C_SRC)	; $(DATE); $(LINT_EACH)
lincnt.out:	$(C_SRC)	; $(DATE); lincnt $(C_SRC) >>$@
