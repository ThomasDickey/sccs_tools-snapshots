# $Header: /users/source/archives/sccs_tools.vcs/RCS/Makefile,v 1.1 1989/03/29 13:09:17 dickey Exp $
# Top-level make-file for SCCS_TOOLS
#
# $Log: Makefile,v $
# Revision 1.1  1989/03/29 13:09:17  dickey
# Initial revision
#

####### (Development) ##########################################################
INSTALL_PATH = /ste_site/ste/bin
GET	= checkout
THIS	= Makefile
MAKE	= make $(MFLAGS) -k$(MAKEFLAGS)	GET=$(GET)

####### (Standard Lists) #######################################################
SOURCES	=\
	COPYRIGHT\
	README

MFILES	=\
	bin/Makefile\
	bin/makefile\
	certificate/Makefile\
	src/Makefile\
	user/Makefile

FIRST	=\
	$(SOURCES)\
	$(MFILES)

####### (Standard Productions) #################################################
all:		$(FIRST)
	cd certificate;	$(MAKE) $@
	cd src;		$(MAKE) install
	cd user;	$(MAKE) $@
	cd bin;		$(MAKE) $@

clean\
clobber\
run_tests\
sources\
install\
deinstall:	$(FIRST)
	cd certificate;	$(MAKE) $@
	cd src;		$(MAKE) $@
	cd user;	$(MAKE) $@
	cd bin;		$(MAKE) $@ INSTALL_PATH=$(INSTALL_PATH)

destroy:	$(MFILES)
	cd certificate;	$(MAKE) $@
	cd src;		$(MAKE) $@
	cd user;	$(MAKE) $@
	cd bin;		$(MAKE) $@
	rm -f *

####### (Details of Productions) ###############################################
.first:		$(FIRST)

$(SOURCES):			; $(GET) $@

bin/Makefile:			; cd bin;		$(GET) Makefile
certificate/Makefile:		; cd certificate;	$(GET) Makefile
src/Makefile:			; cd src;		$(GET) Makefile
user/Makefile:			; cd user;		$(GET) Makefile

# Embed default installation path in places where we want it compiled-in.
# Note that we exploit the use of lower-case makefile for this purpose.
bin/makefile:	bin/Makefile	$(THIS)
	rm -f $@
	sed -e s+INSTALL_PATH=.*+INSTALL_PATH=$(INSTALL_PATH)+ bin/Makefile >$@
