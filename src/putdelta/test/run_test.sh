#!/bin/sh
# $Id: run_test.sh,v 6.1 2019/12/06 18:04:36 tom Exp $
# test-script for see that 'putdelta' is working.
#
SCCS=${SCCS_DIR-SCCS}
PATH=`cd ../bin;pwd`:$PATH;	export PATH
SCCS_TOOL=sccs
myfile=makefile

echo '** '`date`
for tool in putdelta sccsput
do
cat <<eof/
**
**	Testing $tool:
eof/
rm -rf $SCCS dummy
copy $myfile dummy
if ( $tool -fs dummy )
then
	N=`ls -l dummy $myfile |\
		fgrep -v otal |\
		sed -e s/dummy/$myfile/ |\
		uniq | wc -l`
	if test $N != 1
	then
		echo '?? date was not maintained'
		ls -l dummy $myfile
	fi
	echo '** archived file is'
	ls -l $SCCS/s.dummy
	echo '** history:'
	$SCCS_TOOL prs $SCCS/s.dummy
else
	echo '?? $tool failed'
fi
rm -rf $SCCS dummy
done
