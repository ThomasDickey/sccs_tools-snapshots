# $Id: Makefile,v 5.0 1991/10/24 08:56:19 ste_cm Rel $
# Make-file for SCCS check-in utilities
#
####### (Environment) ##########################################################
B	= ../bin
I	= ../../../../cm_tools/interface
L	= ../../../../cm_tools/lib
GET	= checkout
#
####### (Command-line Options) #################################################
DEFINES	= -DGET_TOOL=\"`which get`\"
INCLUDES= -I$I
CPP_OPTS= $(DEFINES) $(INCLUDES)
LIBS	= $L/lib.a
DATE	= echo '****'`date` >> $@
LINTOPT	= $(CPP_OPTS) -ltd
.c.o:	; $(CC) -c $(CFLAGS) $(CPP_OPTS) $*.c
#
####### (Standard Lists) #######################################################
ALL	= $B/getdelta $B/sccsget
C_SRC	= getdelta.c sccsget.c
SOURCES	= $(C_SRC)
MAIN_O	= getdelta.o $(LIBS)
TREE_O	= sccsget.o $(LIBS)
#
####### (Standard Productions) #################################################
all:		$(ALL)
#
clean:				; rm -f *.o *.bak *.out
clobber:	clean		; rm -f $(ALL)
destroy:
	sh -c 'for i in *;do case $$i in RCS);; *) rm -f $$i;;esac;done;exit 0'
run_tests:
sources:	$(SOURCES)
#
install:	$B/getdelta\
		$B/sccsget
#
####### (Details of Productions) ###############################################
PTYPES_H	= $I/ptypes.h $I/common.h

$B/getdelta:	$(MAIN_O)	; $(CC) -o $@ $(MAIN_O)
getdelta.o:	$(PTYPES_H)\
		$I/sccsdefs.h
#
$B/sccsget:	$(TREE_O)	; $(CC) -o $@ $(TREE_O)
sccsget.o:	$(PTYPES_H)\
		$I/rcsdefs.h\
		$I/sccsdefs.h
#
####### (Development) ##########################################################
$(SOURCES):			; $(GET) $@
lint.out:	$(C_SRC)
	$(DATE)
	sh -c 'for i in $?;do echo $$i:>>$@;tdlint $(LINTOPT) $$i >>$@;done'
lincnt.out:	$(C_SRC)	; $(DATE); lincnt $(C_SRC) >>$@
