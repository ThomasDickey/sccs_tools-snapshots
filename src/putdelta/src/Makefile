# $Header: /users/source/archives/sccs_tools.vcs/src/putdelta/src/RCS/Makefile,v 2.0 1989/07/10 08:38:01 ste_cm Rel $
# Make-file for SCCS check-in utilities
#
####### (Environment) ##########################################################
B	= ../bin
I	= ../../../../cm_tools/interface
L	= ../../../../cm_tools/lib
GET	= checkout
#
####### (Command-line Options) #################################################
INCLUDES= -I$I
CFLAGS  = $(DEFINES) $(INCLUDES) -O
LIBS	= $L/lib.a
DATE	= echo '****'`date` >> $@
LINTOPT	= $(DEFINES) $(INCLUDES) -ltd
#
####### (Standard Lists) #######################################################
ALL	= $B/putdelta $B/sccsput
CSRC	= putdelta.c
SCRIPTS	= sccsput.sh
SRCS	= $(CSRC) $(SCRIPTS)
MAIN_O	= putdelta.o $(LIBS)
#
####### (Standard Productions) #################################################
all:		$(ALL)
#
clean:				; rm -f *.o *.out
clobber:	clean		; rm -f $(ALL)
destroy:
	sh -c 'for i in *;do case $$i in RCS);; *) rm -f $$i;;esac;done'
sources:	$(SRCS)
#
install:	$B/putdelta\
		$B/sccsput
#
####### (Details of Productions) ###############################################
$B/putdelta:	$(MAIN_O)	; $(CC) -o $@ $(MAIN_O)
putdelta.o:	$I/ptypes.h
#
$B/sccsput:	sccsput.sh	; copy $? $@
#
####### (Development) ##########################################################
$(SRCS):			; $(GET) $@
lint.out:	$(CSRC)		; $(DATE); tdlint $(LINTOPT) $? >>$@
lincnt.out:	$(CSRC)		; $(DATE); lincnt $(CSRC) >>$@
