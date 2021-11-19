config_opts['target_arch'] = 'x86_64'
config_opts['legal_host_arches'] = ('x86_64',)
config_opts['rpmbuild_networking'] = True
config_opts['use_host_resolv'] = True


config_opts['dnf.conf'] += """
[gem-rtsw-epics-base-testing]
name=Gemini RTSW group software epics-base testing packages
baseurl=http://hbfswgrepo-lv1.hi.gemini.edu/repo/gem-rtsw-testing/epics-base/<BRANCH>/centos-8/x86_64/
gpgcheck=0
timeout=0
"""
