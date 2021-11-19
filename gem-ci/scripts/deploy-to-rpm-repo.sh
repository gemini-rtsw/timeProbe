#!/bin/bash
#aliases for switch on/off command printout to console
shopt -s expand_aliases
alias trace_on='set -x'
alias trace_off='{ set +x; } 2>/dev/null'

#strip off MATURITY from CI_COMMIT_BRANCH
SOURCE_BRANCH_LOCAL=${CI_COMMIT_BRANCH#*/}
echo "SOURCE_BRANCH_LOCAL is set to $SOURCE_BRANCH_LOCAL"

trace_on
# make sure the ssh key of hbfswgrepo-lv1 is in .ssh/known_hosts
ssh -o StrictHostKeyChecking=no koji@hbfswgrepo-lv1.hi.gemini.edu "echo importing public key of hbfswgrepo-lv1"

# patch again for the deployment to RPM repo (unfortunately, the template files must be
# located under /etc/mock/templates and can not be loaded from other paths, hence
# their content is simply appended to gem-rtsw-$TYPE-$MATURITY.cfg
if [ "$TYPE" != "app" ]; then
    for mock_template in $MOCK_TEMPLATE_COMMON $MOCK_TEMPLATE_EPICS
    do
	exitcode=0
	grep -q $mock_template gem-rtsw-$TYPE.cfg || exitcode=1
	# if matched
	if [ $exitcode -eq 0 ]; then
	    # delete line from config file
	    echo "found $mock_template in gem-rtsw-$TYPE, patch it for building Container"
	    sed -i \
	      -e "\#$mock_template#d" \
	      gem-rtsw-$TYPE.cfg

	    # strip off path (/etc/mock/templates/)
	    template=${mock_template##*/}

	    # append the just created file, if possible, if not use the one
	    # from the base container (e.g. the 'common' one for epics-base layer)
	    if [ -f $template ]; then
		echo "appending content of $template"
		cat $template >> gem-rtsw-$TYPE.cfg
	    elif [ -f $mock_template ]; then
		echo "appending content of $mock_template"
		cat $mock_template >> gem-rtsw-$TYPE.cfg
	    else
		echo "!WARNING! Found neither $template nor $mock_template"
	    fi
	fi
    done
    cat gem-rtsw-$TYPE.cfg
fi
# patch again for the deployment to RPM repo
unlink .tito/releasers.conf && sed -e "s#/etc/mock#\.#g" releasers.conf > .tito/releasers.conf
# deploy the RPM to it
RSYNC_USERNAME=koji tito release gem-rtsw-$TYPE -o tmp && rm -rf tmp || exit 123
trace_off
