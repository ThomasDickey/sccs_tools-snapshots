# $Id: Makefile,v 6.0 1993/04/29 08:55:23 ste_cm Rel $
# Top-level makefile for SCCS_TOOLS get-delta utility

THIS	= getdelta
TOP	= ../../..
include $(TOP)/td_lib/support/td_lib.mk

####### (Standard Lists) #######################################################
B	= ../../bin

IT	=\
	$B/$(THIS)\
	$B/sccsget

MFILES	=\
	src/Makefile\
	test/Makefile

DIRS	=\
	bin

####### (Standard Productions) #################################################
all\
run_test::	$(DIRS)

all\
clean\
clobber\
destroy\
run_test\
sources\
lincnt.out\
lint.out::	$(MFILES)
	cd src;		$(MAKE) $@
	cd test;	$(MAKE) $@

clean\
clobber::		; rm -f $(CLEAN)

clobber\
destroy::		; rm -rf bin

destroy::		; $(DESTROY)

install:	all $(IT)
deinstall:		; rm -f $(IT)

####### (Details of Productions) ###############################################
$(DIRS):			; mkdir $@
$(MFILES):			; $(GET) $@
$B/$(THIS):	bin/$(THIS)	; $(PUT)
$B/sccsget:	bin/sccsget	; $(PUT)
