#!/bin/bash
#aliases for switch on/off command printout to console
shopt -s expand_aliases
alias trace_on='set -x'
alias trace_off='{ set +x; } 2>/dev/null'


# default for 'app'
PROJECT_NAME=$CI_PROJECT_NAME

trace_on

# if 'common' or 'epics-base'
if [ "$TYPE" != "app" ]; then
    echo setting PROJECT_NAME to $TYPE &&
    PROJECT_NAME=$TYPE
fi

#strip off MATURITY from CI_COMMIT_BRANCH
SOURCE_BRANCH_LOCAL=${CI_COMMIT_BRANCH#*/}
echo "SOURCE_BRANCH_LOCAL is set to $SOURCE_BRANCH_LOCAL"
echo "MOCK_TEMPLATE_COMMON is set to $MOCK_TEMPLATE_COMMON"
echo "MOCK_TEMPLATE_EPICS is set to $MOCK_TEMPLATE_EPICS"

# if one of 'common' or 'epics-base',
# create a mock template configuration file from template.
# Touch it before so that we have some 'material' for the artifacts...
touch gem-rtsw-$TYPE.tpl
  
if [ "$TYPE" != "app" ]; then
    echo "patching gem-rtsw-$TYPE-$MATURITY.tpl"
    sed -e "s#<BRANCH>#$SOURCE_BRANCH_LOCAL#g" \
     -e "s#<MOCK_TEMPLATE_COMMON>#$MOCK_TEMPLATE_COMMON#g" \
     -e "s#<MOCK_TEMPLATE_EPICS>#$MOCK_TEMPLATE_EPICS#g" \
     gem-ci/templates/mock/$TYPE/templates/gem-rtsw-$TYPE-$MATURITY.tpl > gem-rtsw-$TYPE.tpl
fi

# create mock configuration file from template
sed -e "s#<PROJECTNAME>#$PROJECT_NAME#g" \
  -e "s#<BRANCH>#$SOURCE_BRANCH_LOCAL#g" \
  -e "s#<MOCK_TEMPLATE_COMMON>#$MOCK_TEMPLATE_COMMON#g" \
  -e "s#<MOCK_TEMPLATE_EPICS>#$MOCK_TEMPLATE_EPICS#g" \
  gem-ci/templates/mock/$TYPE/gem-rtsw-$TYPE-$MATURITY.cfg > gem-rtsw-$TYPE.cfg 

# create RPM repo configuration file from template 
sed -e "s#<PROJECTNAME>#$PROJECT_NAME#g" \
  -e "s#<BRANCH>#$SOURCE_BRANCH_LOCAL#g" \
  gem-ci/templates/rpm/$TYPE/gem-rtsw-$TYPE-$MATURITY.repo > gem-rtsw-$TYPE.repo

# create releasers.conf from template
# and replace /etc/mock with . for using the gemini-$TYPE-$MATURITY.cfg artifact from above
sed -e "s#<PROJECTNAME>#$PROJECT_NAME#g" \
  -e "s#<BRANCH>#$SOURCE_BRANCH_LOCAL#g" \
  gem-ci/templates/tito/$TYPE/releasers.conf.$MATURITY > releasers.conf

# tweak Containerfile to COPY these repo configuration file instead of default testing ones
sed -e "s#<BASE_CONTAINER>#$BASE_CONTAINER#g" \
  -e "s#<PROJECTNAME>#$PROJECT_NAME#g" \
  -e "s#<MATURITY>#$MATURITY#g" \
  gem-ci/templates/docker/$TYPE/Containerfile > Containerfile

trace_off
