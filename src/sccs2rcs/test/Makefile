# $Header: /users/source/archives/sccs_tools.vcs/src/sccs2rcs/test/RCS/Makefile,v 5.0 1991/06/05 09:41:23 ste_cm Rel $
# make-file for testing sccs/RCS conversion utility
#
# $Log: Makefile,v $
# Revision 5.0  1991/06/05 09:41:23  ste_cm
# BASELINE Mon Jul 20 12:41:28 1992 -- CM_TOOLS #11
#
#	Revision 4.0  91/06/05  09:41:23  ste_cm
#	BASELINE Tue Dec 17 11:56:35 1991
#	
#	Revision 3.0  91/06/05  09:41:23  ste_cm
#	BASELINE Tue Jun 18 08:04:39 1991 -- apollo sr10.3
#	
#	Revision 2.1  91/06/05  09:41:23  dickey
#	dummied unused rules
#	
#	Revision 2.0  89/07/10  08:42:15  ste_cm
#	BASELINE Mon Jul 10 09:19:20 EDT 1989
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
lint.out\
lincnt.out:
run_tests:	$(SOURCES)	; run_tests.sh

####### (Details of Productions) ###############################################
$(SOURCES):			; $(GET) $@
