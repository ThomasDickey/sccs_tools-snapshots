# $Id: Makefile,v 2.2 1991/04/01 08:23:59 dickey Exp $
# Top-level make-file for SCCS_TOOLS
#
# $Log: Makefile,v $
# Revision 2.2  1991/04/01 08:23:59  dickey
# changed install-path
#
#	Revision 2.1  89/10/05  10:37:55  dickey
#	added lint.out, lincnt.out rules
#	
#	Revision 2.0  89/07/10  08:19:31  ste_cm
#	BASELINE Mon Jul 10 09:14:49 EDT 1989
#	

####### (Development) ##########################################################
INSTALL_PATH = /local/impact/bin
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

lint.out\
lincnt.out:	$(MFILES)
	cd src;		$(MAKE) $@

destroy:	$(MFILES)
	cd certificate;	$(MAKE) $@
	cd src;		$(MAKE) $@
	cd user;	$(MAKE) $@
	cd bin;		$(MAKE) $@
	sh -c 'for i in *;do case $$i in RCS);; *) rm -f $$i;;esac;done;exit 0'

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
