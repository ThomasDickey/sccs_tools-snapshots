# $Header: /users/source/archives/sccs_tools.vcs/src/sccs2rcs/test/RCS/Makefile,v 1.1 1989/03/22 09:34:37 dickey Exp $
# make-file for testing sccs/RCS conversion utility
#
# $Log: Makefile,v $
# Revision 1.1  1989/03/22 09:34:37  dickey
# Initial revision
#

####### (Development) ##########################################################
GET	= checkout

####### (Standard Lists) #######################################################
SCRIPTS	=\
	do_a_test.sh\
	run_tests.sh

S_FILES	=\
	s.testfile1.c\
	s.testfile2.c

SOURCES	= $(SCRIPTS) $(S_FILES)

####### (Standard Productions) #################################################
all:
clean:				; rm -f *.out *.RCS *.SCCS
clobber:	clean
destroy:			; rm -f *
sources:	$(SOURCES)
run_tests:	$(SOURCES)	; run_tests.sh

####### (Details of Productions) ###############################################
$(SOURCES):			; $(GET) $@
