# $Header: /users/source/archives/sccs_tools.vcs/user/RCS/Makefile,v 3.0 1989/07/10 08:43:59 ste_cm Rel $
#
# $Log: Makefile,v $
# Revision 3.0  1989/07/10 08:43:59  ste_cm
# BASELINE Tue Jun 18 08:04:39 1991 -- apollo sr10.3
#
#	Revision 2.0  89/07/10  08:43:59  ste_cm
#	BASELINE Mon Jul 10 09:31:07 EDT 1989
#	
#	Revision 1.2  89/07/10  08:43:59  dickey
#	corrected 'destroy' rule
#	
#	Revision 1.1  89/03/29  15:05:35  dickey
#	Initial revision
#	

####### (Development) ##########################################################
INSTALL_PATH=/ste_site/ste/doc
B	= $(INSTALL_PATH)
GET	= checkout
PUT	= copy -v -d $@

####### (Standard Lists) #######################################################
ALL	=\
	$B/getdelta.doc\
	$B/putdelta.doc\
	$B/sccsget.doc\
	$B/sccsput.doc
SOURCES	=\
	getdelta.doc\
	putdelta.doc\
	sccsget.doc\
	sccsput.doc

####### (Standard Productions) #################################################
all:		; @echo '** use "make install"'
clean:
clobber:	clean	; rm -f *.doc_*
destroy:
	sh -c 'for i in *;do case $$i in RCS);; *) rm -f $$i;;esac;done'
run_tests:
sources:	$(SOURCES)
install:	$(ALL)
deinstall:	; rm -f $(ALL)

####### (Details of Productions) ###############################################
$B/getdelta.doc:	getdelta.doc	; $(PUT)
$B/putdelta.doc:	putdelta.doc	; $(PUT)
$B/sccsget.doc:		sccsget.doc	; $(PUT)
$B/sccsput.doc:		sccsput.doc	; $(PUT)

$(SOURCES):				; $(GET) $@
