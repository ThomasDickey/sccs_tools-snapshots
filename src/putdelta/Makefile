# $Id: Makefile,v 6.1 1994/07/21 00:29:52 tom Exp $
# Top-level makefile for SCCS_TOOLS put-delta utility

THIS	= putdelta
TOP	= ../../..
include $(TOP)/td_lib/support/td_lib.mk

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
run_test::	$(DIRS)

all\
clean\
clobber\
destroy\
run_test\
sources\
lint.out::	$(MFILES)
	cd src;		$(MAKE) $@
	cd test;	$(MAKE) $@

clean\
clobber::		; $(RM) $(CLEAN)

clobber\
destroy::		; $(RM) -r $(DIRS)

destroy::		; $(DESTROY)

install:	all $(IT)
deinstall:		; $(RM) $(IT)

####### (Details of Productions) ###############################################
$(DIRS):			; mkdir $@
$(MFILES):			; $(GET) $@
$B/$(THIS):	bin/$(THIS)	; $(PUT)
$B/sccsput:	bin/sccsput	; $(PUT)
