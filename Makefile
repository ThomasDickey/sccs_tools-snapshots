# $Id: Makefile,v 4.0 1991/10/24 09:31:03 ste_cm Rel $
# Top-level make-file for SCCS_TOOLS
#

####### (Development) ##########################################################
INSTALL_BIN = ../install_bin
INSTALL_MAN = ../install_man
COPY	= cp -p
MAKE	= make $(MFLAGS) -k$(MAKEFLAGS) CFLAGS="$(CFLAGS)" COPY="$(COPY)"
THIS	= sccs_tools

####### (Standard Lists) #######################################################
SOURCES	=\
	COPYRIGHT\
	README

MFILES	=\
	bin/Makefile\
	certificate/Makefile\
	src/Makefile\
	user/Makefile

####### (Standard Productions) #################################################
all\
sources\
install\
deinstall::	$(MFILES) $(SOURCES) bin/makefile

all\
install::
	cd src;		$(MAKE) install
install\
deinstall::
	cd user;	$(MAKE) $@ INSTALL_BIN=`cd ..;cd $(INSTALL_BIN);pwd`
	cd bin;		$(MAKE) $@ INSTALL_MAN=`cd ..;cd $(INSTALL_MAN);pwd`

clean\
clobber\
destroy\
sources::	$(MFILES)
	cd certificate;	$(MAKE) $@
	cd src;		$(MAKE) $@
	cd user;	$(MAKE) $@
	cd bin;		$(MAKE) $@

run_tests\
lint.out\
lincnt.out:	$(MFILES)
	cd src;		$(MAKE) $@

clean\
clobber::
	rm -f *.bak *.log *.out core

destroy::
	sh -c 'for i in *;do case $$i in RCS);; *) rm -f $$i;;esac;done;exit 0'

####### (Details of Productions) ###############################################
$(MFILES)\
$(SOURCES):			; checkout -x $@

# Embed default installation path in places where we want it compiled-in.
# Note that we exploit the use of lower-case makefile for this purpose.
bin/makefile:	bin/Makefile	Makefile
	rm -f $@
	sed -e s+INSTALL_BIN=.*+INSTALL_BIN=`cd $(INSTALL_BIN);pwd`+ bin/Makefile >$@
