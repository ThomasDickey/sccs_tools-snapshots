#!/bin/sh
# $Id: run_test.sh,v 6.4 2023/01/20 23:45:48 tom Exp $
# test-script for see that 'putdelta' is working.
#
SCCS=${SCCS_DIR-SCCS}
PATH=`cd ../bin;pwd`:$PATH;	export PATH
SCCS_TOOL="sccs"
myfile=makefile

echo "** `date`"
for tool in putdelta sccsput
do
cat <<eof/
**
**	Testing $tool:
eof/
rm -rf "$SCCS" dummy
cp $myfile dummy
touch -r $myfile dummy
if ( $tool -fs dummy 2>/dev/null )
then
	N=`ls -l dummy $myfile |\
		grep -v otal |\
		sed -e s/dummy/$myfile/ |\
		uniq | wc -l`
	if test "$N" != 1
	then
		echo '?? date was not maintained'
		ls -l dummy $myfile
	else
		echo '** date was maintained'
	fi
	echo '** archived file is'
	ls -l "$SCCS"/s.dummy
	echo '** history:'
	$SCCS_TOOL prs "$SCCS"/s.dummy
else
	echo "?? $tool failed"
fi
rm -rf "$SCCS" dummy
done
