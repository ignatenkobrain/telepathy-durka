Name:           telepathy-durka
Version:        0.0.0.1
Release:        1%{?dist}
Summary:        VK Connection Manager for Telepathy

License:        GPLv3+
URL:            https://github.com/ignatenkobrain/telepathy-durka
Source0:        %{url}/archive/%{version}/%{name}-%{version}

BuildRequires:  libtool autoconf
BuildRequires:  glib2-devel json-glib-devel
BuildRequires:  telepathy-glib-devel
BuildRequires:  dbus-glib-devel dbus-devel
BuildRequires:  rest-devel libxml2-devel
Requires:       telepathy-filesystem

%description
A full-featured VK connection manager for the Telepathy project.

%prep
%setup -q

%build
./autogen.sh
%configure
make %{?_smp_mflags}

%install
%make_install

%files
%doc AUTHORS COPYING NEWS
%{_libexecdir}/%{name}
%{_datadir}/dbus-1/services/*.service
%{_datadir}/telepathy/managers/*.manager

%changelog
* Sun Nov 09 2014 Igor Gnatenko <i.gnatenko.brain@gmail.com> - 0.0.0.1-1
- Initial packaging
