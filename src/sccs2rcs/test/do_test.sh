: '$Header: /users/source/archives/sccs_tools.vcs/src/sccs2rcs/test/RCS/do_test.sh,v 2.0 1989/03/23 08:01:11 ste_cm Rel $'
# Run a specified regression test on the sccs/RCS conversion utility.
# Arguments:
#	$1 = option(s) for sccs2rcs
#	$2 = name of the working file
#	$3+ = version numbers which are defined in the file.
#
# $Log: do_test.sh,v $
# Revision 2.0  1989/03/23 08:01:11  ste_cm
# BASELINE Mon Jul 10 09:19:20 EDT 1989
#
# Revision 1.2  89/03/23  08:01:11  dickey
# quote "$O" variable so we can pass in quoted string (testfile3 case)
# 
# Revision 1.1  89/03/22  09:37:06  dickey
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
$X "$O" s.$F
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
