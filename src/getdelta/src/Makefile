# @(#)Makefile	1.2 88/08/03 16:14:27
# Make-file for SCCS checkout utilities
#
####### (Environment) ##########################################################
B	= ../../bin
I	= ../lib
GET	= getdelta
#
####### (Command-line Options) #################################################
INCLUDES= -I$I
CFLAGS  = $(DEFINES) $(INCLUDES) -O
LIBS	= ../lib/lib.a
DATE	= echo '****'`date` >> $@
LINTOPT	= $(DEFINES) $(INCLUDES) -ltd
#
####### (Standard Lists) #######################################################
ALL	= getdelta
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
destroy:			; rm -f *
sources:	$(SRCS)
#
install:	$B/getdelta\
		$B/sccsget
#
####### (Details of Productions) ###############################################
getdelta:	$(MAIN_O)	; $(CC) -o $@ $(MAIN_O)
#
getdelta.o:	$I/ptypes.h
#
$B/getdelta:	getdelta	; rm -f $@; cp $? $@; chmod +x $@
$B/sccsget:	sccsget.sh	; rm -f $@; cp $? $@; chmod +x $@
#
####### (Development) ##########################################################
$(SRCS):			; $(GET) -s sccs/s.$@
lint.out:	$(CSRC)		; $(DATE); tdlint $(LINTOPT) $? >>$@
lincnt.out:	$(CSRC)		; $(DATE); lincnt $(CSRC) >>$@
