#!/bin/sh
# $Id: do_test.sh,v 3.0 1991/06/05 17:43:35 ste_cm Rel $
# Run a specified regression test on the sccs/RCS conversion utility.
# Arguments:
#	$1 = option(s) for sccs2rcs
#	$2 = name of the working file
#	$3+ = version numbers which are defined in the file.
#
# $Log: do_test.sh,v $
# Revision 3.0  1991/06/05 17:43:35  ste_cm
# BASELINE Tue Jun 18 08:04:39 1991 -- apollo sr10.3
#
# Revision 2.2  91/06/05  17:43:35  dickey
# push-aside pre-existing RCS-directory so this script will work ok.
# 
# Revision 2.1  89/10/10  15:27:40  dickey
# use current directory as RCS-directory for testing
# 
# Revision 1.2  89/03/23  08:20:17  dickey
# quote "$O" variable so we can pass in quoted string (testfile3 case)
# 
# Revision 1.1  89/03/22  09:37:06  dickey
# Initial revision
# 
#
if test -d RCS
then	mv RCS RCS-
fi
#
D=.
X=../bin/sccs2rcs
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
RCS_DIR=$D $X "$O" s.$F
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
