# $Id: Makefile,v 6.1 1994/07/21 00:29:20 tom Exp $
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
run_test\
install\
deinstall::	$(MFILES)
	cd getdelta;	$(MAKE) $@
	cd putdelta;	$(MAKE) $@
	cd sccs2rcs;	$(MAKE) $@

clean\
clobber::			; $(RM) $(CLEAN)
destroy::			; $(DESTROY)

####### (Details of Productions) ###############################################
$(MFILES):			; $(GET) $@
