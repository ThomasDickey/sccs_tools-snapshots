# $Id: Makefile,v 6.1 1994/07/21 00:29:19 tom Exp $
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
	cd bin;		$(MAKE) $@ INSTALL_BIN=`cd ..;cd $(INSTALL_BIN);pwd`
	cd user;	$(MAKE) $@ INSTALL_MAN=`cd ..;cd $(INSTALL_MAN);pwd`

clean\
clobber\
destroy\
sources::	$(MFILES)
	cd certify;	$(MAKE) $@
	cd src;		$(MAKE) $@
	cd user;	$(MAKE) $@
	cd bin;		$(MAKE) $@

run_test\
lint.out:	$(MFILES)
	cd src;		$(MAKE) $@

clean\
clobber::			; $(RM) $(CLEAN)

destroy::			; $(DESTROY)

####### (Details of Productions) ###############################################
$(MFILES)\
$(SOURCES):			; $(GET) $@
