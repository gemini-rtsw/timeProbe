#!/bin/sh
#aliases for switch on/off command printout to console
shopt -s expand_aliases
alias trace_on='set -x'
alias trace_off='{ set +x; } 2>/dev/null'

echo "rules-recognized TYPE: $TYPE"

# create branch-specific RPM repo directory and initialize it from 'source' repo, 
# if testing repo is existing and unstable repo is empty
echo creating $RPM_REPO directory on hbfswgrepo-lv1

# default for 'app'
PROJECT_NAME=$CI_PROJECT_NAME

trace_on

# if 'common' or 'epics-base'
if [ "$TYPE" != "app" ]; then
    echo setting PROJECT_NAME to $TYPE &&
    PROJECT_NAME=$TYPE
fi

#strip off MATURITY from CI_COMMIT_BRANCH
BRANCH=${CI_COMMIT_BRANCH#*/}
RPM_REPO="/var/www/html/repo/gem-rtsw-$MATURITY/$PROJECT_NAME/$BRANCH/centos-8/x86_64"


ssh -o StrictHostKeyChecking=no koji@hbfswgrepo-lv1.hi.gemini.edu \
    "[ ! -d $RPM_REPO ] && mkdir -p $RPM_REPO && cd $RPM_REPO && createrepo_c ." \
    || echo RPM repository directory $RPM_REPO already existing, ommiting to initialize it
  
#if [ $MATURITY == "unstable" ]; then
#    echo initializing $RPM_REPO with data from $RPM_REPO_SOURCE
#    ssh -o StrictHostKeyChecking=no koji@hbfswgrepo-lv1.hi.gemini.edu \
#      "[ -d $RPM_REPO_SOURCE ] && \
#      [ $(ssh -o StrictHostKeyChecking=no koji@hbfswgrepo-lv1.hi.gemini.edu ls $RPM_REPO | wc -w) -eq 1 ] && \
#      rm -rf $RPM_REPO/repodata && cp -rf $RPM_REPO_SOURCE/* $RPM_REPO/" \
#      || echo  $RPM_REPO_SOURCE repo not existing or $RPM_REPO already filled, ommitting to copy content over
#fi

ssh -o StrictHostKeyChecking=no koji@hbfswgrepo-lv1.hi.gemini.edu \
    "cd $RPM_REPO && createrepo_c ." \
    || echo Failure to initialize $RPM_REPO
trace_off
# make CI_COMMIT_BRANCH available for subsequent jobs
#        - echo "$CI_COMMIT_BRANCH" > branch.env

