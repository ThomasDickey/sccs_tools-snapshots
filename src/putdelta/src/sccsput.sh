: '$Header: /users/source/archives/sccs_tools.vcs/src/putdelta/src/RCS/sccsput.sh,v 2.0 1988/07/29 09:14:10 ste_cm Rel $'
# Check-in one or more modules to SCCS (T.E.Dickey)
# 
# Notes:
#	We assume that (because we are doing ongoing development), that we do
#	not wish to simply archive files, but to retain a version for editing.
#	Also, we would like to (if possible) preserve the modification date on
#	files which we are checking in.  Therefore, if the SCCS-file already
#	exists, we save the original file, and re-check it out.
#
#	This procedure should *only* be used for development-testing.  For
#	releases, copy the accumulated 'sccs' directory to the release and
#	do the command
#
#		get sccs
#
#	to load into (the presumably clean) directory all of the released
#	source.  This will expand all of the '%' tokens (showing for example,
#	the last delta-data for each module).
#
# Options:
#	-b (passed to 'diff' in display of differences)
#	-c (use 'cat' for paging diffs)
#	-f (force checkin, ignoring 'file')
#	-l (specify logfile for differences)
#	-n (suppress check-in process, do differences only)
#	-s (suppress most messages, other than yes/no summary)
#
#	The environment-variable NOTE, if given, is used to specify the "-y"
#	comment for the admin/delta commands.  This is intended to be used in
#	shell procedures (e.g., 'sccsjoin').
#
#	If a directory name is given, this script will recur into lower levels.
#	All options will be inherited in recursion.
#
# Environment:
#	NOTE	- used by 'sccsjoin' to merge sccs-deltas
#	PAGER	- may override to use in difference listing
#	SCCS_DIR- name of sccs directory
#
# hacks to make this run on Apollo:
if [ -f /com/vt100 ]
then	PATH=$PATH:/sys5/bin
fi
TRACE=
SCCS=${SCCS_DIR-sccs}
#
if [ -n "$NOTE" ]
then
	echo $NOTE
fi
#
BLANKS=
FORCE=
SILENT=
NOP=
LOG=
#
WD=`pwd`
#
# Process options
set -$TRACE `getopt l:bcfns $*`
if [ $? != 0 ]
then
	echo 'usage: checkin [-l LOGFILE] [-bcfns] files'
	exit 1
fi
#
# (patch: assignment to 'i' is necessary for SCO-xenix bug)
OPTS=
for i in $*
do	i=$1
	shift
	case $i in
	-l)	D=`dirname $1`
		F=`basename $1`
		cd $D
		LOG=`pwd`/$F
		cd $WD
		i="$i$LOG"
		shift;;
	-b)	BLANKS="-b";;
	-c)	PAGER="cat";;
	-f)	FORCE="T";;
	-n)	NOP="T";;
	-s)	SILENT="-s";;
	--)	break;;
	esac
	OPTS="$OPTS $i"
done
#
# Process list of files
for i in $*
do
	ACT=
	cd $WD
	D=`dirname $i`
	F=`basename $i`
	if [ -n "$LOG" ]
	then	echo '*** '$WD/$i >>$LOG
	fi
	cd $D
	D=`pwd`
	i=$F
	if [ -f $i ]
	then
		if [ ! -d $SCCS -a -z "$NOP" ]
		then	mkdir $SCCS
		fi
	elif [ -d $i ]
	then
		if [ $i != $SCCS ]
		then	echo '*** recurring to '$i
			cd $i
			$0 $OPTS *
		fi
		continue
	else
		echo '*** Ignored "'$i'" (not a file)'
		continue
	fi
	j=$SCCS/s.$i
	if [ -f $j ]
	then
		echo '*** Checking differences for "'$i'"'
		cd /tmp
		get -s -k $D/$j
		cd $D
		if (cmp -s $i /tmp/$i)
		then
			echo '*** no differences found ***'
		else
			diff $BLANKS /tmp/$i $i >/tmp/diff$$
			if [ -z "$SILENT" ]
			then
				${PAGER-'more'} /tmp/diff$$
			fi
			if [ -n "$LOG" ]
			then
				echo appending to logfile
				cat /tmp/diff$$ >>$LOG
			fi
			rm -f /tmp/diff$$
			ACT="D"
		fi
		rm -f /tmp/$i
	elif [ -f $i ]
	then
		if [ -n "$FORCE" ]
		then	ACT="I"
		elif (file $i | fgrep -v packed | grep text$)
		then	ACT="I"
		else	echo '*** "'$i'" does not seem to be a text file'
		fi
	fi
#
	if [ -z "$NOP" -a -n "$ACT" ]
	then
		if [ -n "$NOTE" ]
		then	putdelta "$NOTE" $i
		else	putdelta $i
		fi
	elif [ -n "$ACT" ]
	then
		if [ "$ACT" = "I" ]
		then	echo '--- This would be initial for "'$i'"'
		else	echo '--- Delta would be applied to "'$i'"'
		fi
	fi
done
