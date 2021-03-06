h62886
s 00003/00002/00022
d D 1.3 89/03/22 14:40:09 dickey 3 2
c mods to test 'sccs2rcs' edit-option
e
s 00003/00000/00021
d D 1.2 88/05/06 14:54:03 dickey 2 1
c port to gould
e
s 00021/00000/00000
d D 1.1 87/11/25 11:35:00 dickey 1 0
c date and time created 87/11/30 17:07:05 by dickey
e
u
U
t
T
I 1
#ifndef	lint
D 3
static	char	sccs_id[] = "%W% %E% %U%";
E 3
I 3
static	char	sccs_id[] = "%W% %E% %U%";	/* ident-string */
E 3
#endif	lint

/*
 * Title:	closeall.c (close-all files)
 * Author:	T.E.Dickey
 * Created:	25 Nov 1987
D 3
 * Modified:
E 3
I 3
 * Modified:	22 Mar 1988, to provide a simple test-case for sccs2rcs '-e'
E 3
 *
 * Function:	Close all open files, beginning with a specified one.
 *
I 3
 *		Don't substitute "@(#)"
E 3
 */

#include	<stdio.h>
I 2
#ifndef	_NFILE
#define	_NFILE	20		/* ...well I tried */
#endif	_NFILE
E 2

closeall(fd)
{
	while (fd < _NFILE)
		(void) close(fd++);
}
E 1
