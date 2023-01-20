#!/bin/sh
# $Id: run_test.sh,v 6.1 2023/01/20 23:52:19 tom Exp $
# Perform tests to ensure that the sccs/RCS conversion works properly.
# 
echo "** `date`"
#
./do_test.sh -q testfile1.c 1.1 1.2
./do_test.sh -q testfile2.c 1.1 1.2 1.1.1.1
#
# test the keyword-editing stuff
./do_test.sh -qe testfile1.c
./do_test.sh -qec' *		' testfile3.c 1
