#!/bin/sh
# $Id: run_test.sh,v 6.0 1993/04/29 09:25:06 ste_cm Rel $'
# test-script for 'getdelta'
#
echo '** '`date`
SCCS=${SCCS_DIR-SCCS}
PATH=`cd ../bin;pwd`:$PATH;	export PATH

W=dummy
S=$SCCS/s.$W
P=$SCCS/p.$W

rm -rf $SCCS $W
mkdir $SCCS

cat <<eof/
**
**	Archive a file, on top of which we will put another version:
eof/
copy Makefile $W
admin -i$W $S

rm -f $W
get -s -e $S
sed -e s/#/##/ Makefile >$W
delta -s -n -ycomments $S

for tool in getdelta sccsget
do
cat <<eof/
**
**	Retrieve the original file with $tool:
eof/
$tool -fr1.1 $W
cat <<eof/
**
**	Test differences between the first-archived version and the original
**	file:
eof/
if ( cmp -s $W Makefile )
then	diff $W Makefile
else	echo '** (no difference found)'
fi
done

rm -rf $SCCS $W
