: '$Header: /users/source/archives/sccs_tools.vcs/src/putdelta/test/RCS/run_test.sh,v 2.0 1989/03/29 13:52:18 ste_cm Rel $'
# test-script for see that 'putdelta' is working.
#
# $Log: run_test.sh,v $
# Revision 2.0  1989/03/29 13:52:18  ste_cm
# BASELINE Mon Jul 10 09:24:06 EDT 1989
#
# Revision 1.1  89/03/29  13:52:18  dickey
# Initial revision
# 
#
date
rm -rf sccs dummy
copy Makefile dummy
if ( ../bin/putdelta -s dummy )
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
	ls -l sccs/s.dummy
else
	echo '?? putdelta failed'
fi
rm -rf sccs dummy
