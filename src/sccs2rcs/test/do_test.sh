#!/bin/sh
# $Id: do_test.sh,v 6.0 1993/04/29 12:39:50 ste_cm Rel $
# Run a specified regression test on the sccs/RCS conversion utility.
# Arguments:
#	$1 = option(s) for sccs2rcs
#	$2 = name of the working file
#	$3+ = version numbers which are defined in the file.
#
cat <<EOF
**
**	Running test with arguments "$*"
**
EOF
#
PATH=../bin:../../../bin:$PATH; export PATH
if test -d RCS
then	mv RCS RCS-
fi
#
D=.
RCS_DIR=$D; export RCS_DIR
#
O=$1
shift
#
F=$1
shift
rm -f $D/$F,v $F.RCS $F.SCCS
#
echo '********('$F')********'
sccs2rcs "$O" s.$F
echo
#
for V in $*
do
	echo '********(version = '$V')********'
	getdelta -s -f -r$V s.$F
	mv $F $F.SCCS
	checkout -q -r$V $F
	mv $F $F.RCS
	ls -l $F.RCS $F.SCCS
	if (cmp -s $F.RCS $F.SCCS)
	then
		echo
	else
		echo '-------- diff --------'
		diff  $F.RCS $F.SCCS
	fi
	rm -f $F.RCS $F.SCCS
done
rm -f $D/$F,v
if test -d RCS-
then	mv RCS- RCS
fi
