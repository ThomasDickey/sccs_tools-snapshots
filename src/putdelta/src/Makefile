# $Id: Makefile,v 5.0 1991/10/24 08:59:22 ste_cm Rel $
# Make-file for SCCS check-in utilities
#
####### (Environment) ##########################################################
B	= ../bin
I	= ../../../../cm_tools/interface
L	= ../../../../cm_tools/lib
GET	= checkout
#
####### (Command-line Options) #################################################
DEFINES	= -DADMIN_TOOL=\"`which admin`\" -DDELTA_TOOL=\"`which delta`\"
INCLUDES= -I$I
CPP_OPTS= $(DEFINES) $(INCLUDES)
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
run_tests:
sources:	$(SOURCES)
#
install:	$B/putdelta\
		$B/sccsput
#
####### (Details of Productions) ###############################################
PTYPES_H	= $I/ptypes.h $I/common.h

$B/putdelta:	$(MAIN_O)	; $(CC) -o $@ $(MAIN_O)
putdelta.o:	$(PTYPES_H)\
		$I/sccsdefs.h
#
$B/sccsput:	$(TREE_O)	; $(CC) -o $@ $(TREE_O)
sccsput.o:	$(PTYPES_H)\
		$I/rcsdefs.h\
		$I/sccsdefs.h
#
####### (Development) ##########################################################
$(SOURCES):			; $(GET) $@
lint.out:	$(C_SRC)
	$(DATE)
	sh -c 'for i in $?;do echo $$i:>>$@;tdlint $(LINTOPT) $$i >>$@;done'
lincnt.out:	$(C_SRC)	; $(DATE); lincnt $(C_SRC) >>$@
