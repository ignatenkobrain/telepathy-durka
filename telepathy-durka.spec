Name:           telepathy-durka
Version:        0.0.1
Release:        1%{?dist}
Summary:        VK Connection Manager for Telepathy

License:        GPLv3+
URL:            https://github.com/ignatenkobrain/telepathy-durka
Source0:        %{url}/archive/%{version}/%{name}-%{version}

BuildRequires:  meson
BuildRequires:  glib2-devel json-glib-devel
BuildRequires:  telepathy-glib-devel
BuildRequires:  dbus-glib-devel
BuildRequires:  rest-devel
Requires:       telepathy-filesystem

%description
A full-featured VK connection manager for the Telepathy project.

%prep
%setup -q
mkdir build

%build
pushd build
  meson .. --prefix=%{_prefix}
  ninja-build -v
popd

%install
pushd build
  DESTDIR=%{buildroot} ninja-build -v install
popd

%files
%doc AUTHORS COPYING
%{_libexecdir}/%{name}
%{_datadir}/dbus-1/services/*.service
%{_datadir}/telepathy/managers/*.manager

%changelog
* Sun Nov 09 2014 Igor Gnatenko <i.gnatenko.brain@gmail.com> - 0.0.1-1
- Initial packaging
