Summary: SCCS Tools
%define AppProgram sccs_tools
%define AppLibrary td_lib
%define AppVersion 12.x
%define AppRelease 20180324
%define LibRelease 20180324
# $Id: sccs_tools-12.0.spec,v 1.8 2018/03/24 18:01:50 tom Exp $
Name: %{AppProgram}
Version: %{AppVersion}
Release: %{AppRelease}
License: MIT-X11
Group: Development/Tools
URL: ftp://ftp.invisible-island.net/ded
Source0: %{AppLibrary}-%{LibRelease}.tgz
Source1: %{AppProgram}-%{AppRelease}.tgz
Vendor: Thomas Dickey <dickey@invisible-island.net>

%description
These are utility programs which simplify the use of SCCS,
and extend it by preserving timestamps of archived files.
The package also includes an improved version of sccs2rcs.

%prep

# no need for debugging symbols...
%define debug_package %{nil}

# -a N (unpack Nth source after cd'ing into build-root)
# -b N (unpack Nth source before cd'ing into build-root)
# -D (do not delete directory before unpacking)
# -q (quiet)
# -T (do not do default unpacking, is used with -a or -b)
rm -rf %{AppProgram}-%{AppVersion}
mkdir %{AppProgram}-%{AppVersion}
%setup -q -D -T -a 1
mv %{AppProgram}-%{AppRelease}/* .
%setup -q -D -T -a 0

%build

cd %{AppLibrary}-%{LibRelease}

./configure \
		--target %{_target_platform} \
		--prefix=%{_prefix} \
		--bindir=%{_bindir} \
		--libdir=%{_libdir} \
		--mandir=%{_mandir} \
		--datadir=%{_datadir} \
		--disable-echo
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

* Sat Mar 24 2018 Thomas Dickey
- disable debug-package

* Sat Jul 03 2010 Thomas Dickey
- code cleanup

* Tue Jun 29 2010 Thomas Dickey
- initial version
