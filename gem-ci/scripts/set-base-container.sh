#!/bin/bash
#aliases for switch on/off command printout to console
shopt -s expand_aliases
alias trace_on='set -x'
alias trace_off='{ set +x; } 2>/dev/null'

trace_on

echo current container: $BASE_CONTAINER
echo current branch: $CI_COMMIT_BRANCH

# the new BASE_CONTAINER's maturity is extracted from branch name
# by deleting longest match of '/*' from end of string
MATURITY=${CI_COMMIT_BRANCH%%/*}
# the new BASE_CONTAINER's 'branch' is extracted from branch name
# by deleting longest match of '*/' from beginning of string
TARGET_BRANCH=${CI_COMMIT_BRANCH##*/}
TARGET_PROJECT=""

# map maturity to correct internal ones
if [ "$MATURITY" == "release" ]; then
    MATURITY="stable"
elif [[ "$MATURITY" != "testing" ]]; then
    MATURITY="unstable"
fi

# if '/special/' is part of the branch name and type is app, extract
# target project name from branch name. 
if [[ "$TYPE" == "app" && \
     `expr match "$CI_COMMIT_BRANCH" '.*\(/special/\).*'` == "/special/" ]]; then
    TMP=${CI_COMMIT_BRANCH##*special/}
    TARGET_PROJECT=${TMP%%/*}
    # use 'iocs' instead of 'app' as the correct path following
    # the subgroup structure on gitlab.com
    TYPE="iocs"
# else it's straight forward
elif [ "$TYPE" == "epics-base" ]; then
    TARGET_PROJECT="epics-base"
elif [ "$TYPE" == "common" ]; then
    TARGET_PROJECT="gemini-ade"
fi  

echo CONTAINER_PROJECT="nsf-noirlab/gemini/rtsw/$TYPE/$TARGET_PROJECT" >> prepare.env
echo CONTAINER_PROJECT_HTML="nsf-noirlab%2Fgemini%2Frtsw%2F$TYPE%2F$TARGET_PROJECT" >> prepare.env
echo CONTAINER_PROJECT_BRANCH="$CI_COMMIT_BRANCH" >> prepare.env
echo MATURITY="$MATURITY" >> prepare.env
echo TYPE="$TYPE" >> prepare.env
echo NEW_BASE_CONTAINER="registry.gitlab.com/nsf-noirlab/gemini/rtsw/$TYPE/$TARGET_PROJECT/$MATURITY/$TARGET_BRANCH:latest" >> prepare.env

trace_off
