# $Header: /users/source/archives/sccs_tools.vcs/src/putdelta/src/RCS/Makefile,v 2.1 1990/05/08 14:10:27 dickey Exp $
# Make-file for SCCS check-in utilities
#
####### (Environment) ##########################################################
B	= ../bin
I	= ../../../../cm_tools/interface
L	= ../../../../cm_tools/lib
GET	= checkout
#
####### (Command-line Options) #################################################
DEFINES	=
INCLUDES= -I$I
CPP_OPTS= $(DEFINES) $(INCLUDES)
CFLAGS	= -O
LIBS	= $L/lib.a
DATE	= echo '****'`date` >> $@
LINTOPT	= $(CPP_OPTS) -ltd
.c.o:	; $(CC) -c $(CFLAGS) $(CPP_OPTS) $*.c
#
####### (Standard Lists) #######################################################
ALL	= $B/putdelta $B/sccsput
C_SRC	= putdelta.c sccsput.c
SOURCES	= $(C_SRC)
MAIN_O	= putdelta.o $(LIBS)
TREE_O	= sccsput.o $(LIBS)
#
####### (Standard Productions) #################################################
all:		$(ALL)
#
clean:				; rm -f *.o *.bak *.out
clobber:	clean		; rm -f $(ALL)
destroy:
	sh -c 'for i in *;do case $$i in RCS);; *) rm -f $$i;;esac;done;exit 0'
sources:	$(SOURCES)
#
install:	$B/putdelta\
		$B/sccsput
#
####### (Details of Productions) ###############################################
$B/putdelta:	$(MAIN_O)	; $(CC) -o $@ $(MAIN_O)
putdelta.o:	$I/ptypes.h\
		$I/sccsdefs.h
#
$B/sccsput:	$(TREE_O)	; $(CC) -o $@ $(TREE_O)
sccsput.o:	$I/ptypes.h\
		$I/rcsdefs.h\
		$I/sccsdefs.h
#
####### (Development) ##########################################################
$(SOURCES):			; $(GET) $@
lint.out:	$(C_SRC)
	$(DATE)
	sh -c 'for i in $?;do echo $$i:>>$@;tdlint $(LINTOPT) $$i >>$@;done'
lincnt.out:	$(C_SRC)	; $(DATE); lincnt $(C_SRC) >>$@
