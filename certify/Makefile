# $Header: /users/source/archives/sccs_tools.vcs/certify/RCS/Makefile,v 5.0 1989/07/10 08:26:36 ste_cm Rel $
#
# $Log: Makefile,v $
# Revision 5.0  1989/07/10 08:26:36  ste_cm
# BASELINE Mon Jul 20 12:41:28 1992 -- CM_TOOLS #11
#
#	Revision 4.0  89/07/10  08:26:36  ste_cm
#	BASELINE Tue Dec 17 11:56:35 1991
#	
#	Revision 3.0  89/07/10  08:26:36  ste_cm
#	BASELINE Tue Jun 18 08:04:39 1991 -- apollo sr10.3
#	
#	Revision 2.0  89/07/10  08:26:36  ste_cm
#	BASELINE Mon Jul 10 09:16:36 EDT 1989
#	
#	Revision 1.2  89/07/10  08:26:36  dickey
#	corrected 'destroy' rule
#	
#	Revision 1.1  89/03/27  16:22:13  dickey
#	Initial revision
#	
#	Revision 1.1  89/03/27  16:23:45  dickey
#	RCS_BASE
#	

####### (Development) ##########################################################
GET	= checkout

####### (Standard Lists) #######################################################
SOURCES	=\
	README\
	config_sheet.txt\
	release_notes.txt

####### (Standard Productions) #################################################
all:
clean:
clobber:
destroy:
	sh -c 'for i in *;do case $$i in RCS);; *) rm -f $$i;;esac;done'
run_tests:
sources:	$(SOURCES)
install:
deinstall:

####### (Details of Productions) ###############################################
$(SOURCES):	; $(GET) $@
