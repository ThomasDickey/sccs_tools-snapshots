# $Id: Makefile,v 6.1 1994/07/21 00:33:53 tom Exp $
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
clean:				; $(RM) $(CLEAN) *.RCS *.SCCS
clobber:	clean		; -$(RM) $(ALL)
destroy:			; $(DESTROY)
sources:	$(SOURCES)
lint.out:
run_test:	$(SOURCES)	; $(RUN_TESTS)

####### (Details of Productions) ###############################################
$(SOURCES):			; $(GET) $@
