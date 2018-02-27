#
#    fty-rest - REST API for auth Convergence
#
#    Copyright (c) the Authors
#

# To build with draft APIs, use "--with drafts" in rpmbuild for local builds or add
#   Macros:
#   %_with_drafts 1
# at the BOTTOM of the OBS prjconf
%bcond_with drafts
%if %{with drafts}
%define DRAFTS yes
%else
%define DRAFTS no
%endif
Name:           fty-rest
Version:        1.0.0
Release:        1
Summary:        rest api for auth convergence
License:        GPL-2.0+
URL:            https://42ity.org
Source0:        %{name}-%{version}.tar.gz
Group:          System/Libraries
# Note: ghostscript is required by graphviz which is required by
#       asciidoc. On Fedora 24 the ghostscript dependencies cannot
#       be resolved automatically. Thus add working dependency here!
BuildRequires:  ghostscript
BuildRequires:  asciidoc
BuildRequires:  automake
BuildRequires:  autoconf
BuildRequires:  libtool
BuildRequires:  pkgconfig
BuildRequires:  xmlto
BuildRequires:  gcc-c++
BuildRequires:  zeromq-devel
BuildRequires:  czmq-devel
BuildRequires:  malamute-devel
BuildRequires:  libcidr-devel
BuildRequires:  cxxtools-devel
BuildRequires:  tntnet-devel
BuildRequires:  tntdb-devel
BuildRequires:  file-devel
BuildRequires:  fty-proto-devel
BuildRoot:      %{_tmppath}/%{name}-%{version}-build

%description
fty-rest rest api for auth convergence.

%package -n libfty_rest1
Group:          System/Libraries
Summary:        rest api for auth convergence shared library

%description -n libfty_rest1
This package contains shared library for fty-rest: rest api for auth convergence

%post -n libfty_rest1 -p /sbin/ldconfig
%postun -n libfty_rest1 -p /sbin/ldconfig

%files -n libfty_rest1
%defattr(-,root,root)
%doc COPYING
%{_libdir}/libfty_rest.so.*

%package devel
Summary:        rest api for auth convergence
Group:          System/Libraries
Requires:       libfty_rest1 = %{version}
Requires:       zeromq-devel
Requires:       czmq-devel
Requires:       malamute-devel
Requires:       libcidr-devel
Requires:       cxxtools-devel
Requires:       tntnet-devel
Requires:       tntdb-devel
Requires:       file-devel
Requires:       fty-proto-devel

%description devel
rest api for auth convergence development tools
This package contains development files for fty-rest: rest api for auth convergence

%files devel
%defattr(-,root,root)
%{_includedir}/*
%{_libdir}/libfty_rest.so
%{_libdir}/pkgconfig/libfty_rest.pc
%{_mandir}/man3/*
%{_mandir}/man7/*

%prep

%setup -q

%build
sh autogen.sh
%{configure} --enable-drafts=%{DRAFTS} --with-tntnet=yes --with-libmagic=yes
make %{_smp_mflags}

%install
make install DESTDIR=%{buildroot} %{?_smp_mflags}

# remove static libraries
find %{buildroot} -name '*.a' | xargs rm -f
find %{buildroot} -name '*.la' | xargs rm -f


%changelog
