# $Header: /users/source/archives/sccs_tools.vcs/src/RCS/Makefile,v 5.0 1989/07/10 08:10:32 ste_cm Rel $
# Top-level makefile for SCCS_TOOLS src-directory
#
# $Log: Makefile,v $
# Revision 5.0  1989/07/10 08:10:32  ste_cm
# BASELINE Mon Jul 20 12:41:28 1992 -- CM_TOOLS #11
#
#	Revision 4.0  89/07/10  08:10:32  ste_cm
#	BASELINE Tue Dec 17 11:56:35 1991
#	
#	Revision 3.0  89/07/10  08:10:32  ste_cm
#	BASELINE Tue Jun 18 08:04:39 1991 -- apollo sr10.3
#	
#	Revision 2.0  89/07/10  08:10:32  ste_cm
#	BASELINE Mon Jul 10 09:18:37 EDT 1989
#	
#	Revision 1.2  89/07/10  08:10:32  dickey
#	corrected FIRST, destroy rules
#	
#	Revision 1.1  89/03/29  13:12:06  dickey
#	Initial revision
#	

####### (Development) ##########################################################
GET	= checkout
MAKE	= make $(MFLAGS) -k$(MAKEFLAGS)	GET=$(GET)

####### (Standard Lists) #######################################################
FIRST	=\
	getdelta/Makefile\
	putdelta/Makefile\
	sccs2rcs/Makefile

####### (Standard Productions) #################################################
all\
clean\
clobber\
rdestroy\
sources\
lint.out\
lincnt.out\
run_tests\
install\
deinstall:	$(FIRST)
	cd getdelta;	$(MAKE) $@
	cd putdelta;	$(MAKE) $@
	cd sccs2rcs;	$(MAKE) $@

destroy: rdestroy
	sh -c 'for i in *;do case $$i in RCS);; *) rm -f $$i;;esac;done'

####### (Details of Productions) ###############################################
getdelta/Makefile:	; cd getdelta;	$(GET) Makefile
putdelta/Makefile:	; cd putdelta;	$(GET) Makefile
sccs2rcs/Makefile:	; cd sccs2rcs;	$(GET) Makefile
