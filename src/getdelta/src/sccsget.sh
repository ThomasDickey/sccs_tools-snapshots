#!/bin/sh
# $Id: sccsget.sh,v 2.1 1989/10/10 15:52:05 dickey Exp $
# Check files out of SCCS (T.E.Dickey)
#
# Use SCCS-get to checkout one or more files from the sccs-directory which is
# located in the current working directory.  Then, use 'sccsdate' to set the
# modification date of the checked-out files according to the last delta date
# (rather than the current date, as SCCS assumes).
#
# Options are designed to feed-thru to 'get(1)'.
#
# Environment:
#	GET_PATH- location of "get" utility
#	SCCS_DIR- name of sccs directory
#
# hacks to make this run on apollo:
if [ -f /com/vt100 ]
then	PATH=$PATH:/sys5/bin
fi
TRACE=
SCCS=${SCCS_DIR-sccs}
#
WD=`pwd`
OPT=
set -$TRACE `getopt nfskr:c: $*`
for i in $*
do
	case $i in
	-[nfsk]) OPT="$OPT $i";   shift;;
	-c)	OPT="$OPT -c$2"; shift; shift;;
	-r)	OPT="$OPT -r$2"; shift; shift;;
	--)	shift; break;;
	-*)	echo "usage: $0 [-nfsk] [-r sid] [-c cutoff] files";exit;;
	esac
done
#
if [ -z "$1" ]
then
	$0 $OPT $SCCS
elif [ "$1" = $SCCS ]
then
	$0 $OPT $SCCS/s.*
else
	for i in $*
	do
		if [ -d $i ]
		then
			if [ "$i" != $SCCS -a "$i" != RCS ]
			then
				echo '** checkout directory "'$i'"'
				cd $i
				$0 $OPT *
				cd $WD
			fi
		else
			getdelta $OPT $i
		fi
	done
fi
