#!/bin/sh
# $Id: run_test.sh,v 3.0 1991/05/24 09:20:01 ste_cm Rel $
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
else
	echo '?? $tool failed'
fi
rm -rf $SCCS dummy
done
