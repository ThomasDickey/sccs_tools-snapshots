# $Id: Makefile,v 6.0 1993/04/29 08:35:21 ste_cm Rel $
# Top-level makefile for SCCS_TOOLS src-directory

TOP	= ../..
include $(TOP)/td_lib/support/td_lib.mk

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
run_test\
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
