# $Header: /users/source/archives/sccs_tools.vcs/RCS/Makefile,v 3.0 1991/06/05 12:41:47 ste_cm Rel $
# Top-level make-file for SCCS_TOOLS
#
# $Log: Makefile,v $
# Revision 3.0  1991/06/05 12:41:47  ste_cm
# BASELINE Tue Jun 18 08:04:39 1991 -- apollo sr10.3
#
#	Revision 2.5  91/06/05  12:41:47  dickey
#	corrected typos
#	
#	Revision 2.4  91/06/05  10:21:43  dickey
#	standardized install-rules
#	
#	Revision 2.3  91/06/04  16:57:19  dickey
#	standardized install-path
#	
#	Revision 2.2  91/04/01  08:23:59  dickey
#	changed install-path
#	
#	Revision 2.1  89/10/05  10:37:55  dickey
#	added lint.out, lincnt.out rules
#	
#	Revision 2.0  89/07/10  08:19:31  ste_cm
#	BASELINE Mon Jul 10 09:14:49 EDT 1989
#	

####### (Development) ##########################################################
INSTALL_BIN = ~ste_cm/bin/`arch`
INSTALL_DOC = /ste_site/ste/doc
COPY	= cp -p
MAKE	= make $(MFLAGS) -k$(MAKEFLAGS) CFLAGS="$(CFLAGS)"
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

FIRST	=\
	$(SOURCES)\
	$(MFILES)\
	bin/makefile

####### (Standard Productions) #################################################
all:		$(FIRST)
	cd certificate;	$(MAKE) $@
	cd src;		$(MAKE) install
	cd user;	$(MAKE) $@
	cd bin;		$(MAKE) $@

clean\
clobber:	$(MFILES)
	cd certificate;	$(MAKE) $@
	cd src;		$(MAKE) $@
	cd user;	$(MAKE) $@
	cd bin;		$(MAKE) $@

run_tests\
sources\
install\
deinstall:	$(FIRST)
	cd certificate;	$(MAKE) $@
	cd src;		$(MAKE) $@
	cd user;	$(MAKE) $@ INSTALL_PATH=$(INSTALL_DOC) COPY="$(COPY)"
	cd bin;		$(MAKE) $@ INSTALL_PATH=$(INSTALL_BIN) COPY="$(COPY)"

lint.out\
lincnt.out:	$(MFILES)
	cd src;		$(MAKE) $@

destroy:	$(MFILES)
	cd certificate;	$(MAKE) $@
	cd src;		$(MAKE) $@
	cd user;	$(MAKE) $@ INSTALL_PATH=$(INSTALL_DOC)
	cd bin;		$(MAKE) $@ INSTALL_PATH=$(INSTALL_BIN)
	sh -c 'for i in *;do case $$i in RCS);; *) rm -f $$i;;esac;done;exit 0'

####### (Details of Productions) ###############################################
.first:		$(FIRST)

$(MFILES)\
$(SOURCES):			; checkout -x $@

# Embed default installation path in places where we want it compiled-in.
# Note that we exploit the use of lower-case makefile for this purpose.
bin/makefile:	bin/Makefile	Makefile
	rm -f $@
	sed -e s+INSTALL_PATH=.*+INSTALL_PATH=$(INSTALL_BIN)+ bin/Makefile >$@
