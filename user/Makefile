# $Id: Makefile,v 5.0 1991/10/24 08:44:27 ste_cm Rel $
#

####### (Development) ##########################################################
INSTALL_MAN=../../install_man
COPY	= cp -p
GET	= checkout
PUT	= rm -f $@; $(COPY) $? $@


USE	= man
SECTION	= 1
PREFIX	= $(INSTALL_MAN)/$(USE)$(SECTION)/

.SUFFIXES:
.SUFFIXES: .man .cat
#
.man.cat:
	rm -f $@
	nroff -man $< >$@

####### (Standard Lists) #######################################################
ALL	=\
	$(PREFIX)getdelta.$(SECTION)\
	$(PREFIX)putdelta.$(SECTION)\
	$(PREFIX)sccsget.$(SECTION)\
	$(PREFIX)sccsput.$(SECTION)
SOURCES	=\
	getdelta.man\
	putdelta.man\
	sccsget.man\
	sccsput.man

####### (Standard Productions) #################################################
all:		; @echo '** use "make install"'
clean:
clobber:	clean	; rm -f *.doc_*
destroy:
	sh -c 'for i in *;do case $$i in RCS);; *) rm -f $$i;;esac;done'
run_tests:
sources:	$(SOURCES)
install:	$(ALL)
deinstall:	; rm -f $(ALL)

####### (Details of Productions) ###############################################
$(PREFIX)getdelta.$(SECTION):	getdelta.$(USE)	; $(PUT)
$(PREFIX)putdelta.$(SECTION):	putdelta.$(USE)	; $(PUT)
$(PREFIX)sccsget.$(SECTION):	sccsget.$(USE)	; $(PUT)
$(PREFIX)sccsput.$(SECTION):	sccsput.$(USE)	; $(PUT)

$(SOURCES):				; $(GET) $@
