# $Header: /users/source/archives/sccs_tools.vcs/src/getdelta/test/RCS/Makefile,v 1.1 1989/03/29 13:57:04 dickey Exp $
# make-file for testing sccs-get package
#
# $Log: Makefile,v $
# Revision 1.1  1989/03/29 13:57:04  dickey
# Initial revision
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
destroy:			; rm -f *
sources:	$(SOURCES)
run_tests:	$(SOURCES)	; run_tests.sh 2>&1 | tee -a $@.out
lint.out:
lincnt.out:

####### (Details of Productions) ###############################################
$(SOURCES):			; $(GET) $@
