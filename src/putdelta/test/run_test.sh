#!/bin/sh
# $Id: run_test.sh,v 3.1 1991/07/09 07:24:17 dickey Exp $
# test-script for see that 'putdelta' is working.
#
SCCS=${SCCS_DIR-SCCS}
PATH=`cd ../bin;pwd`:$PATH;	export PATH

echo '** '`date`
for tool in putdelta sccsput
do
cat <<eof/
**
**	Testing $tool:
eof/
rm -rf $SCCS dummy
copy Makefile dummy
if ( $tool -s dummy )
then
	N=`ls -l dummy Makefile |\
		fgrep -v otal |\
		sed -e s/dummy/Makefile/ |\
		uniq | wc -l`
	if test $N != 1
	then
		echo '?? date was not maintained'
		ls -l dummy Makefile
	fi
	echo '** archived file is'
	ls -l $SCCS/s.dummy
	echo '** history:'
	prs $SCCS/s.dummy
else
	echo '?? $tool failed'
fi
rm -rf $SCCS dummy
done
