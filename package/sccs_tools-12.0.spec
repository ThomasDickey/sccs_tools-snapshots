Summary: SCCS Tools
%define AppProgram sccs_tools
%define AppLibrary td_lib
%define AppVersion 12.x
%define AppRelease 20240427
%define LibRelease 20240421
# $Id: sccs_tools-12.0.spec,v 1.20 2024/04/28 00:12:31 tom Exp $
Name: %{AppProgram}
Version: %{AppVersion}
Release: %{AppRelease}
License: MIT-X11
Group: Development/Tools
URL: https://invisible-island.net/ded
Source0: https://invisible-island.net/archives/ded/%{AppProgram}-%{AppRelease}.tgz
BuildRequires: td_lib <= %{AppRelease}
Vendor: Thomas Dickey <dickey@invisible-island.net>

%description
These are utility programs which simplify the use of SCCS,
and extend it by preserving timestamps of archived files.
The package also includes an improved version of sccs2rcs.

%prep

# no need for debugging symbols...
%define debug_package %{nil}

%setup -q -n %{AppProgram}-%{AppRelease}

%build

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

make install DESTDIR=$RPM_BUILD_ROOT

%files
%defattr(-,root,root)
%{_bindir}/getdelta
%{_bindir}/sccsput
%{_bindir}/fixsccs
%{_bindir}/sccs2rcs
%{_bindir}/putdelta
%{_bindir}/sccsget
%{_mandir}/man1/fixsccs.*
%{_mandir}/man1/getdelta.*
%{_mandir}/man1/sccsput.*
%{_mandir}/man1/sccs2rcs.*
%{_mandir}/man1/putdelta.*
%{_mandir}/man1/sccsget.*

%changelog
# each patch should add its ChangeLog entries here

* Thu Jan 19 2023 Thomas Dickey
- build against td_lib package rather than side-by-side configuration

* Sat Mar 24 2018 Thomas Dickey
- disable debug-package

* Sat Jul 03 2010 Thomas Dickey
- code cleanup

* Tue Jun 29 2010 Thomas Dickey
- initial version
