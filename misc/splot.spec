Name: splot
Version: 0.1
Release: 1
Vendor: N/A
Summary: splot is a classic top-down arcade shoot 'em up
License: GPL
Distribution: N/A
#Icon: N/A
Group: Games/Arcade
Packager: Erik Sohns <eriksohns@123mail.org>
BuildRoot: %{_tmppath}/%{name}-root
Source: %{name}-%{version}.tar.gz
Provides: %{name}
AutoReqProv: yes
Requires: OpenGL, SDL, ftgl, ACE
Prefix: /usr/local

%description
splot is a classic top-down arcade shoot 'em up

%prep
%setup -q
#autoreconf -i

%build
mkdir build
cd build
../configure
cd ..
make

%install
rm -rf %{buildroot}
cd build
%makeinstall

%clean
rm -rf %{buildroot}

%pre
#%post -p /sbin/ldconfig
%verifyscript

%preun
#%postun -p /sbin/ldconfig

%files
%defattr(-, root, root)
%doc AUTHORS COPYING INSTALL ChangeLog NEWS README TODO
%{_bindir}/*
#%{_includedir}/*
#%{_libdir}/*
%{_datadir}/%{name}

%changelog
