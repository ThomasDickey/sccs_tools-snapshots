# $Id: Makefile,v 5.2 1992/10/19 11:49:51 dickey Exp $
# Top-level makefile for SCCS_TOOLS sccs/RCS conversion utility

THIS	= sccs2rcs
TOP	= ../../..
include $(TOP)/cm_library/support/cm_library.mk

####### (Standard Lists) #######################################################
B	= ../../bin

IT	=\
	$B/$(THIS)

MFILES	=\
	src/Makefile\
	test/Makefile

DIRS	=\
	bin

####### (Standard Productions) #################################################
all::	$(DIRS)

all\
clean\
clobber\
destroy\
run_tests\
sources\
lincnt.out\
lint.out::	$(MFILES)
	cd src;		$(MAKE) $@
	cd test;	$(MAKE) $@

clean\
clobber::		; rm -f $(CLEAN)

clobber\
destroy::		; rm -rf $(DIRS)

destroy::		; $(DESTROY)

install:	all $(IT)
deinstall:		; rm -f $(IT)

####### (Details of Productions) ###############################################
$(DIRS):			; mkdir $@
$(MFILES):			; $(GET) $@
$B/$(THIS):	bin/$(THIS)	; $(PUT)
