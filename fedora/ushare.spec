Summary: UPnP (TM) A/V Media Server
Name: ushare
Version: 0.9.5
Release: 4%{?dist}
License: GPL
Group: Applications/Multimedia
URL: http://ushare.geexbox.org/

Source: http://ushare.geexbox.org/releases/%{name}-%{version}.tar.bz2
Source1:ushare.init
Patch0:	ushare-0.9.5-buffer.diff
BuildRoot: %{_tmppath}/%{name}-%{version}-%{release}-root-%(%{__id_u} -n)

BuildRequires: libupnp-devel
Requires(pre): fedora-usermgmt
Requires(post): /sbin/chkconfig
Requires(preun): /sbin/service, /sbin/chkconfig
Requires(postun): /sbin/service

%description
uShare is a UPnP (TM) A/V Media Server. It implements the server 
component that provides UPnP media devices with information on 
available multimedia files. uShare uses the built-in http server 
of libupnp to stream the files to clients.

%prep
%setup -q
%patch0 -p1

%build
%configure
make %{?_smp_mflags}

%install
rm -rf %{buildroot}
%makeinstall
%{__rm} -rf   %{buildroot}%{_sysconfdir}/init.d
%{__install} -m 0755 -D %{SOURCE1} %{buildroot}%{_initrddir}/ushare
%{__mkdir_p} %{buildroot}%{_var}/lib/ushare
%find_lang %{name}

%clean
rm -rf %{buildroot}

%pre
%{_sbindir}/fedora-useradd 21 -s /sbin/nologin -M -r -d %{_var}/lib/ushare \
    -c "ushare service account" ushare &>/dev/null || :

%post
/sbin/chkconfig --add ushare

%preun
if [ $1 -eq 0 ]; then
    /sbin/service ushare stop &>/dev/null || :
    /sbin/chkconfig --del ushare
fi

%postun
if [ $1 -ge 1 ]; then
    /sbin/service ushare condrestart &>/dev/null || :
fi

%files -f %{name}.lang
%defattr(-,root,root,-)
%doc AUTHORS ChangeLog COPYING NEWS README
%{_bindir}/ushare
%config(noreplace) %{_sysconfdir}/ushare.conf
%{_initrddir}/ushare
%{_mandir}/man1/*
%attr(770,ushare,ushare) %dir %{_var}/lib/ushare/

%changelog
* Tue Dec 27 2005 Eric Tanguy 0.9.5-4
- Use %find_lang macro instead of %{_datadir}/locale/*

* Tue Dec 27 2005 Eric Tanguy 0.9.5-3
- Drop "Requires: libupnp"
- replace %{_sysconfdir}/ushare.conf by %config(noreplace) %{_sysconfdir}/ushare.conf

* Tue Dec 27 2005 Eric Tanguy 0.9.5-2
- add patch for buffer

* Tue Dec 27 2005 Eric Tanguy 0.9.5-1
- First build

