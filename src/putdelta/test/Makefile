# $Header: /users/source/archives/sccs_tools.vcs/src/putdelta/test/RCS/Makefile,v 5.0 1989/07/10 08:39:04 ste_cm Rel $
# make-file for testing sccs delta+admin package
#
# $Log: Makefile,v $
# Revision 5.0  1989/07/10 08:39:04  ste_cm
# BASELINE Mon Jul 20 12:41:28 1992 -- CM_TOOLS #11
#
#	Revision 4.0  89/07/10  08:39:04  ste_cm
#	BASELINE Tue Dec 17 11:56:35 1991
#	
#	Revision 3.0  89/07/10  08:39:04  ste_cm
#	BASELINE Tue Jun 18 08:04:39 1991 -- apollo sr10.3
#	
#	Revision 2.0  89/07/10  08:39:04  ste_cm
#	BASELINE Mon Jul 10 09:24:06 EDT 1989
#	
#	Revision 1.2  89/07/10  08:39:04  dickey
#	corrected 'destroy' rule
#	
#	Revision 1.1  89/03/29  13:57:52  dickey
#	Initial revision
#	

####### (Development) ##########################################################
GET	= checkout

####### (Standard Lists) #######################################################
SCRIPTS	=\
	run_tests.sh

SOURCES	= $(SCRIPTS)

####### (Standard Productions) #################################################
all:
clean:				; rm -f *.out
clobber:	clean
destroy:
	sh -c 'for i in *;do case $$i in RCS);; *) rm -f $$i;;esac;done'
sources:	$(SOURCES)
run_tests:	$(SOURCES)	; run_tests.sh 2>&1 | tee -a $@.out
lint.out:
lincnt.out:

####### (Details of Productions) ###############################################
$(SOURCES):			; $(GET) $@
