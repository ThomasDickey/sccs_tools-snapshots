# $Header: /users/source/archives/sccs_tools.vcs/user/RCS/Makefile,v 1.1 1989/03/29 15:05:35 dickey Exp $
#
# $Log: Makefile,v $
# Revision 1.1  1989/03/29 15:05:35  dickey
# Initial revision
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
destroy:	clobber	; rm -f *
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
