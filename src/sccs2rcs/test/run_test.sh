#!/bin/sh
# $Id: run_test.sh,v 5.0 1991/07/24 09:15:49 ste_cm Rel $
# Perform tests to ensure that the sccs/RCS conversion works properly.
# 
do_a_test.sh -q testfile1.c 1.1 1.2
do_a_test.sh -q testfile2.c 1.1 1.2 1.1.1.1
#
# test the keyword-editing stuff
do_a_test.sh -ve testfile1.c
do_a_test.sh -vec' *		' testfile3.c 1
