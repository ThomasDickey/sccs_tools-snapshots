: '$Header: /users/source/archives/sccs_tools.vcs/src/getdelta/test/RCS/run_test.sh,v 2.0 1989/03/29 14:18:22 ste_cm Rel $'
# test-script for 'getdelta'
#
# $Log: run_test.sh,v $
# Revision 2.0  1989/03/29 14:18:22  ste_cm
# BASELINE Mon Jul 10 09:27:34 EDT 1989
#
# Revision 1.1  89/03/29  14:18:22  dickey
# Initial revision
# 

W=dummy
S=sccs/s.$W
P=sccs/p.$W

rm -rf sccs $W
mkdir sccs

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

cat <<eof/
**
**	Retrieve the original file:
eof/
../bin/getdelta -fr1.1 $W
cat <<eof/
**
**	Test differences between the first-archived version and the original
**	file:
eof/
diff $W Makefile

rm -rf sccs $W
