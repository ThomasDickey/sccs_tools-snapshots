: '$Header: /users/source/archives/sccs_tools.vcs/src/sccs2rcs/test/RCS/run_test.sh,v 1.2 1989/03/22 14:49:43 dickey Exp $'
# Perform tests to ensure that the sccs/RCS conversion works properly.
# $Log: run_test.sh,v $
# Revision 1.2  1989/03/22 14:49:43  dickey
# added test cases for "-e" option.
#
# Revision 1.1  89/03/22  09:36:40  dickey
# Initial revision
# 
do_a_test.sh -q testfile1.c 1.1 1.2
do_a_test.sh -q testfile2.c 1.1 1.2 1.1.1.1
#
# test the keyword-editing stuff
do_a_test.sh -ve testfile1.c
do_a_test.sh -ve testfile3.c
