# $Id: Makefile,v 6.3 1994/07/21 00:29:20 tom Exp $
# Make-file for SCCS timestamp utility

TOP	= ../../../..
include $(TOP)/td_lib/support/td_lib.mk

####### (Standard Lists) #######################################################
THIS	= fixsccs
ALL	= $B/$(THIS)
C_SRC	= $(THIS).c
SOURCES	= $(C_SRC)

####### (Standard Productions) #################################################
all\
install:	$(SOURCES) $B $(ALL)
clean:				; $(RM) $(CLEAN)
clobber:	clean		; $(RM) $(ALL)
destroy:			; $(DESTROY)
run_test:
sources:	$(SOURCES)

####### (Details of Productions) ###############################################
$B:				; mkdir $@
$B/$(THIS):	$(THIS).o $(LIB_DEPS)	; $(LINK) -o $@ $(THIS).o $(LIB_ARGS)
$(THIS).o:	$(PTYPES_H) $(SCCSDEFS_H)

####### (Development) ##########################################################
$(SOURCES):			; $(GET) $@
lint.out:	$(C_SRC)	; $(DATE); $(LINT_EACH)
