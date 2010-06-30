Summary: SCCS Tools
%define AppVersion 20100629
%define LibVersion 20100624
# $Id: sccs_tools-12.0.spec,v 1.2 2010/06/30 00:32:11 tom Exp $
Name: sccs_tools
Version: 12.x
# Base version is 12.x; rpm version corresponds to "Source1" directory name.
Release: %{AppVersion}
License: MIT-X11
Group: Applications/Editors
URL: ftp://invisible-island.net/ded
Source0: td_lib-%{LibVersion}.tgz
Source1: sccs_tools-%{AppVersion}.tgz
Vendor: Thomas Dickey <dickey@invisible-island.net>

%description
These are utility programs which simplify the use of SCCS,
and extend it by preserving timestamps of archived files.
The package also includes an improved version of sccs2rcs.

%prep

# -a N (unpack Nth source after cd'ing into build-root)
# -b N (unpack Nth source before cd'ing into build-root)
# -D (do not delete directory before unpacking)
# -q (quiet)
# -T (do not do default unpacking, is used with -a or -b)
rm -rf sccs_tools-12.x
mkdir sccs_tools-12.x
%setup -q -D -T -a 1
mv sccs_tools-%{AppVersion}/* .
%setup -q -D -T -a 0

%build

cd td_lib-%{LibVersion}

./configure \
		--target %{_target_platform} \
		--prefix=%{_prefix} \
		--bindir=%{_bindir} \
		--libdir=%{_libdir} \
		--mandir=%{_mandir} \
		--datadir=%{_datadir}
make

cd ..
./configure \
		--target %{_target_platform} \
		--prefix=%{_prefix} \
		--bindir=%{_bindir} \
		--libdir=%{_libdir} \
		--mandir=%{_mandir} \
		--datadir=%{_datadir}
make

%install

[ "$RPM_BUILD_ROOT" != "/" ] && rm -rf $RPM_BUILD_ROOT

make install                    DESTDIR=$RPM_BUILD_ROOT

%clean
[ "$RPM_BUILD_ROOT" != "/" ] && rm -rf $RPM_BUILD_ROOT

%files
%defattr(-,root,root)
%{_bindir}/getdelta
%{_bindir}/sccsput
%{_bindir}/fixsccs
%{_bindir}/sccs2rcs
%{_bindir}/putdelta
%{_bindir}/sccsget
%{_mandir}/man1/getdelta.*
%{_mandir}/man1/sccsput.*
%{_mandir}/man1/sccs2rcs.*
%{_mandir}/man1/putdelta.*
%{_mandir}/man1/sccsget.*

%changelog
# each patch should add its ChangeLog entries here

* Tue Jun 29 2010 Thomas Dickey
- initial version

