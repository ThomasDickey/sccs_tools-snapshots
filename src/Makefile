# $Id: Makefile,v 5.1 1992/10/19 10:41:29 dickey Exp $
# Top-level makefile for SCCS_TOOLS src-directory

TOP	= ../..
include $(TOP)/cm_library/support/cm_library.mk

####### (Standard Lists) #######################################################
MFILES	=\
	getdelta/Makefile\
	putdelta/Makefile\
	sccs2rcs/Makefile

####### (Standard Productions) #################################################
all\
clean\
clobber\
destroy\
sources\
lint.out\
lincnt.out\
run_tests\
install\
deinstall::	$(MFILES)
	cd getdelta;	$(MAKE) $@
	cd putdelta;	$(MAKE) $@
	cd sccs2rcs;	$(MAKE) $@

clean\
clobber::			; rm -f $(CLEAN)
destroy::			; $(DESTROY)

####### (Details of Productions) ###############################################
$(MFILES):			; $(GET) $@
