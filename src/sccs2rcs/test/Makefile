# $Id: Makefile,v 6.0 1993/04/29 09:46:39 ste_cm Rel $
# make-file for testing sccs/RCS conversion utility

TOP	= ../../../..
include $(TOP)/td_lib/support/td_lib.mk

####### (Standard Lists) #######################################################
SCRIPTS	=\
	do_test.sh\
	run_test.sh

S_FILES	=\
	s.testfile1.c\
	s.testfile2.c\
	s.testfile3.c

SOURCES	= $(SCRIPTS) $(S_FILES)

####### (Standard Productions) #################################################
all:	$(SOURCES) $(ALL)
clean:				; rm -f $(CLEAN) *.RCS *.SCCS
clobber:	clean		; rm -f $(ALL)
destroy:			; $(DESTROY)
sources:	$(SOURCES)
lint.out\
lincnt.out:
run_test:	$(SOURCES)	; $(RUN_TESTS)

####### (Details of Productions) ###############################################
$(SOURCES):			; $(GET) $@
