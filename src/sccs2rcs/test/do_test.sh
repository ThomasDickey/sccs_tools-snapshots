#!/bin/sh
# $Id: do_test.sh,v 5.0 1991/07/24 13:05:40 ste_cm Rel $
# Run a specified regression test on the sccs/RCS conversion utility.
# Arguments:
#	$1 = option(s) for sccs2rcs
#	$2 = name of the working file
#	$3+ = version numbers which are defined in the file.
#
PATH=../bin:../../../bin:$PATH; export PATH
if test -d RCS
then	mv RCS RCS-
fi
#
D=.
L=run_tests.out
#
O=$1
shift
#
F=$1
shift
rm -f $D/$F,v $F.RCS $F.SCCS
#
echo '********('$F')********' | tee -a $L
RCS_DIR=$D sccs2rcs "$O" s.$F
rlog $F >>$L
#
for V in $*
do
	echo '********(version = '$V')********' | tee -a $L
	getdelta -s -f -r$V s.$F
	mv $F $F.SCCS
	RCS_DIR=$D checkout -q -r$V $F
	mv $F $F.RCS
	ls -l $F.RCS $F.SCCS
	cat   $F.RCS >>$L
	echo '******** diff ********' >>$L
	diff  $F.RCS $F.SCCS
	rm -f $F.RCS $F.SCCS
done
rm -f $D/$F,v
if test -d RCS-
then	mv RCS- RCS
fi
