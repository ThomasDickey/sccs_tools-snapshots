# $Header: /users/source/archives/sccs_tools.vcs/src/getdelta/src/RCS/Makefile,v 2.0 1989/07/10 08:30:12 ste_cm Rel $
# Make-file for SCCS checkout utilities
#
####### (Environment) ##########################################################
B	= ../bin
I	= ../../../../cm_tools/interface
L	= ../../../../cm_tools/lib
GET	= getdelta
#
####### (Command-line Options) #################################################
INCLUDES= -I$I
CFLAGS  = $(DEFINES) $(INCLUDES) -O
LIBS	= $L/lib.a
DATE	= echo '****'`date` >> $@
LINTOPT	= $(DEFINES) $(INCLUDES) -ltd
#
####### (Standard Lists) #######################################################
ALL	= $B/getdelta $B/sccsget
CSRC	= getdelta.c
SCRIPTS	= sccsget.sh
SRCS	= $(CSRC) $(SCRIPTS)
MAIN_O	= getdelta.o $(LIBS)
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
install:	$B/getdelta\
		$B/sccsget
#
####### (Details of Productions) ###############################################
$B/getdelta:	$(MAIN_O)	; $(CC) -o $@ $(MAIN_O)
getdelta.o:	$I/ptypes.h
#
$B/sccsget:	sccsget.sh	; copy $? $@
#
####### (Development) ##########################################################
$(SRCS):			; $(GET) $@
lint.out:	$(CSRC)		; $(DATE); tdlint $(LINTOPT) $? >>$@
lincnt.out:	$(CSRC)		; $(DATE); lincnt $(CSRC) >>$@
