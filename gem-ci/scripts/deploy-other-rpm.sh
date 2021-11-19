#!/bin/bash
#aliases for switch on/off command printout to console
shopt -s expand_aliases
alias trace_on='set -x'
alias trace_off='{ set +x; } 2>/dev/null'

trace_on

echo current container: $BASE_CONTAINER

ssh -o StrictHostKeyChecking=no koji@hbfswgrepo-lv1.hi.gemini.edu "echo importing public key of hbfswgrepo-lv1"
unlink .tito/releasers.conf 
#rm -f .tito/releasers.conf
cp -f /gem_base/usr/share/tito/$TYPE/releasers.conf .tito/releasers.conf && RSYNC_USERNAME=koji tito release gem-rtsw-$TYPE

trace_off
