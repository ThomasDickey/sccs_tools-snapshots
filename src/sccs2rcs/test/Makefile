# $Header: /users/source/archives/sccs_tools.vcs/src/sccs2rcs/test/RCS/Makefile,v 2.0 1989/07/10 08:42:15 ste_cm Rel $
# make-file for testing sccs/RCS conversion utility
#
# $Log: Makefile,v $
# Revision 2.0  1989/07/10 08:42:15  ste_cm
# BASELINE Mon Jul 10 09:19:20 EDT 1989
#
#	Revision 1.3  89/07/10  08:42:15  dickey
#	corrected 'destroy' rule
#	
#	Revision 1.2  89/03/23  07:38:16  dickey
#	added testfile3 (for '-e' and '-c' test)
#	
#	Revision 1.1  89/03/22  09:34:37  dickey
#	Initial revision
#	

####### (Development) ##########################################################
GET	= checkout

####### (Standard Lists) #######################################################
SCRIPTS	=\
	do_a_test.sh\
	run_tests.sh

S_FILES	=\
	s.testfile1.c\
	s.testfile2.c\
	s.testfile3.c

SOURCES	= $(SCRIPTS) $(S_FILES)

####### (Standard Productions) #################################################
all:
clean:				; rm -f *.out *.RCS *.SCCS
clobber:	clean
destroy:
	sh -c 'for i in *;do case $$i in RCS);; *) rm -f $$i;;esac;done'
sources:	$(SOURCES)
run_tests:	$(SOURCES)	; run_tests.sh

####### (Details of Productions) ###############################################
$(SOURCES):			; $(GET) $@
