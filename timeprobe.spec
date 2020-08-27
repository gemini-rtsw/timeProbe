%define _prefix /gem_base/epics/support
%define name timeProbe
%define version 3.15.8
%define release 1.1.15
%define repository gemdev
%define debug_package %{nil}
%define arch %(uname -m)
%define checkout %(git log --pretty=format:'%h' -n 1) 

#These global defines are added to prevent stripping
# symbols on vxWorks cross-compiled code
# Getting 'strip' to work is probably only needed for
# building a related debug sub-package
#
# But this prevents all the strip warnings
# mrippa 20120202
%global _enable_debug_package 0
%global debug_package %{nil}
%global __os_install_post /usr/lib/rpm/brp-compress %{nil}

Summary: %{name} Package, a module for EPICS base
Name: %{name}
Version: %{version}
Release: %release.%(date +"%Y%m%d%H%M")%{checkout}%{?dist}
License: EPICS Open License
Group: Applications/Engineering
Source0: %{name}-%{version}.tar.gz
ExclusiveArch: %{arch}
Prefix: %{_prefix}
## You may specify dependencies here
BuildRequires: epics-base-devel re2c tdct bancomm-devel geminiRec-devel timelib-devel
Requires: epics-base bancomm geminiRec timelib
## Switch dependency checking off
# AutoReqProv: no

%description
This is the module %{name}.

## If you want to have a devel-package to be generated uncomment the following:
%package devel
Summary: %{name}-devel Package
Group: Development/Gemini
Requires: %{name} tdct bancomm-devel geminiRec-devel timelib-devel
%description devel
This is the module %{name}.

%prep
%setup -q 

%build
#update environment from former rpm installations due to BuildRequires
source /gem_base/etc/profile
#start virtual framebuffer to have graphics for java
#Xvfb :1  -ac -nolisten tcp -nolisten unix &

make distclean uninstall
#DISPLAY=:1 make
make
#killall Xvfb

%install
export DONT_STRIP=1
rm -rf $RPM_BUILD_ROOT
mkdir -p $RPM_BUILD_ROOT/%{_prefix}/%{name}
cp -r dbd $RPM_BUILD_ROOT/%{_prefix}/%{name}
cp -r db $RPM_BUILD_ROOT/%{_prefix}/%{name}
# cp -r bin $RPM_BUILD_ROOT/%{_prefix}/%{name}
cp -r lib $RPM_BUILD_ROOT/%{_prefix}/%{name}
#cp -r include $RPM_BUILD_ROOT/%{_prefix}/%{name}
cp -r configure $RPM_BUILD_ROOT/%{_prefix}/%{name}
# find $RPM_BUILD_ROOT/%{_prefix}/%{name}/configure -name ".git" -exec rm -rf {} \;


%postun
if [ "$1" = "0" ]; then
	rm -rf %{_prefix}/%{name}
fi


%clean
rm -rf $RPM_BUILD_ROOT

%files
%defattr(-,root,root)
#   /%{_prefix}/%{name}/bin
   /%{_prefix}/%{name}/lib

%files devel
%defattr(-,root,root)
   /%{_prefix}/%{name}/db
   /%{_prefix}/%{name}/dbd
   #/%{_prefix}/%{name}/include
   /%{_prefix}/%{name}/configure

%changelog
* Thu Aug 27 2020 Felix Kraemer <fkraemer@gemini.edu> 3.15.8-1.1.15.2020082710217ad810f
- Added support for configure/RELEASE.local file to overwrite default
  definitions (fkraemer@gemini.edu)

* Wed Aug 26 2020 Felix Kraemer <fkraemer@gemini.edu> 3.15.8-1.1.15.2020082611173882fe6
- commented out WORK_SUPP as it conflicts with the one in timelib; replaced
  WORK_SUPP with PROD_SUPP in lines below (fkraemer@gemini.edu)
- Fixed timelibSupp.dbd reference (iarriagada@gemini.edu)
- removed references to iarriagada on RELEASE (iarriagada@gemini.edu)
- Added support .dbd to be used by IOCs (iarriagada@gemini.edu)
- Release tag enriched with hour and minute (%%H%%M) to be able to build
  several RPMs a day without messing up the repo (fkraemer@gemini.edu)
- don't copy not existing include dir (fkraemer@gemini.edu)

* Wed Jul 29 2020 fkraemer <fkraemer@gemini.edu> 3.15.8-1.1.15.2020072991538fe
- adapted dependecies for BuildRequires and devel package (fkraemer@gemini.edu)

* Mon Jul 27 2020 fkraemer <fkraemer@gemini.edu> 3.15.8-1.1.15.20200727e5d4050
- added timelib dependency to specfile (fkraemer@gemini.edu)
- added timelib dep in configure/RELEASE (fkraemer@gemini.edu)

* Mon Jul 27 2020 fkraemer <fkraemer@gemini.edu> 3.15.8-1.6.13.20200727aa1632f
- new package built with tito

