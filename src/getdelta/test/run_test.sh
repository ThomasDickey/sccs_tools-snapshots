#!/bin/sh
# $Id: run_test.sh,v 6.3 2023/01/20 23:57:50 tom Exp $'
# test-script for 'getdelta'
#
echo "** `date`"
SCCS=${SCCS_DIR-SCCS}
PATH=`cd ../bin;pwd`:$PATH;	export PATH
SCCS_TOOL="sccs"

W=dummy
S=$SCCS/s.$W
myfile=makefile
KEYS='s/[$].*[$]/%W% %E% %U%/'

rm -rf "$SCCS" "$W"
mkdir "$SCCS"

cat <<eof/
**
**	Archive a file, on top of which we will put another version:
eof/
sed -e "$KEYS" $myfile > $W
$SCCS_TOOL admin -i$W "$S"

rm -f $W
$SCCS_TOOL get -s -e "$S"
sed -e "$KEYS" -e 's/#/##/' $myfile >$W
$SCCS_TOOL delta -s -n -ycomments "$S"

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
if ( cmp -s $W $myfile )
then	diff $W $myfile
else	echo '** (no difference found)'
fi
done

rm -rf "$SCCS" "$W"
