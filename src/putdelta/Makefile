# $Id: Makefile,v 5.1 1992/10/19 10:49:49 dickey Exp $
# Top-level makefile for SCCS_TOOLS put-delta utility

THIS	= putdelta
TOP	= ../../..
include $(TOP)/cm_library/support/cm_library.mk

####### (Standard Lists) #######################################################
B	= ../../bin

IT	=\
	$B/$(THIS)\
	$B/sccsput

MFILES	=\
	src/Makefile\
	test/Makefile

DIRS	=\
	bin

####### (Standard Productions) #################################################
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
$B/sccsput:	bin/sccsput	; $(PUT)
