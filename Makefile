# $Id: Makefile,v 5.3 1993/04/29 08:52:34 dickey Exp $
# Top-level make-file for SCCS_TOOLS

TOP	= ..
include $(TOP)/td_lib/support/td_lib.mk

####### (Standard Lists) #######################################################
SOURCES	=\
	COPYING\
	README

MFILES	=\
	bin/Makefile\
	certify/Makefile\
	src/Makefile\
	user/Makefile

####### (Standard Productions) #################################################
all\
sources\
install\
deinstall::	$(MFILES) $(SOURCES)

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
	cd certify;	$(MAKE) $@
	cd src;		$(MAKE) $@
	cd user;	$(MAKE) $@
	cd bin;		$(MAKE) $@

run_test\
lint.out\
lincnt.out:	$(MFILES)
	cd src;		$(MAKE) $@

clean\
clobber::			; rm -f $(CLEAN)

destroy::			; $(DESTROY)

####### (Details of Productions) ###############################################
$(MFILES)\
$(SOURCES):			; $(GET) $@
