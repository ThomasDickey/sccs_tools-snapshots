#!/bin/sh
# $Id: run_test.sh,v 3.0 1989/10/10 15:27:51 ste_cm Rel $
# Perform tests to ensure that the sccs/RCS conversion works properly.
# $Log: run_test.sh,v $
# Revision 3.0  1989/10/10 15:27:51  ste_cm
# BASELINE Tue Jun 18 08:04:39 1991 -- apollo sr10.3
#
# Revision 2.1  89/10/10  15:27:51  dickey
# fix for apollo sr10.1
# 
# Revision 1.3  89/03/23  08:20:55  dickey
# refined test-case for '-e' option using '-c' option
# 
# Revision 1.2  89/03/22  14:49:43  dickey
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
do_a_test.sh -vec' *		' testfile3.c 1
