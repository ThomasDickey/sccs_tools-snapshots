: '$Header: /users/source/archives/sccs_tools.vcs/src/sccs2rcs/test/RCS/do_test.sh,v 1.1 1989/03/22 09:37:06 dickey Exp $'
# Run a specified regression test on the sccs/RCS conversion utility.
# Arguments:
#	$1 = option(s) for sccs2rcs
#	$2 = name of the working file
#	$3+ = version numbers which are defined in the file.
#
# $Log: do_test.sh,v $
# Revision 1.1  1989/03/22 09:37:06  dickey
# Initial revision
#
#
X=../bin/sccs2rcs
L=run_tests.out
#
O=$1
shift
#
F=$1
shift
rm -f RCS/$F,v $F.RCS $F.SCCS
#
echo '********('$F')********' | tee -a $L
$X $O s.$F
rlog $F >>$L
#
for V in $*
do
	echo '********(version = '$V')********' | tee -a $L
	getdelta -s -f -r$V s.$F
	mv $F $F.SCCS
	checkout -q -r$V $F
	mv $F $F.RCS
	ls -l $F.RCS $F.SCCS
	cat   $F.RCS >>$L
	echo '******** diff ********' >>$L
	diff  $F.RCS $F.SCCS
	rm -f $F.RCS $F.SCCS
done
rm -f RCS/$F,v
