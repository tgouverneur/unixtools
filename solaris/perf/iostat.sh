#!/bin/bash
##
# iostat.sh - check system disks which usage is over ${pc}% 
# and match their zpool's name to see the culprit.
#
# Quick&Dirty -- use at your own risk.
#
# Thomas Gouverneur - 2014
##


if [ $# -ne 1 ]; then
  echo "$0 <pc>";
  exit 1;
fi
pc=${1}

function ctrl_c() {
        echo "** Trap SIGINT, cleanup and exit";
	rm -rf ${TMP};
        exit 0;
}
trap ctrl_c INT

TMP=$(mktemp -d /tmp/iostat.XXXX);

echo "[-] Getting pool's disks index..";
for p in $(zpool list -Ho name); do
  su nobody -c "/sbin/zpool status ${p}" > ${TMP}/${p}.status;
done;

echo "[-] Starting iostat";
stdbuf -i0 -o0 -e0 iostat -xnzt 1 | gawk -v pc=${pc} '$1 != "tin" && $1 != "extended" && $(NF-1) > pc' | while read line; do
  dev=$(echo $line|awk '{print $NF}');
  pool=$(ggrep -l ${dev} ${TMP}/*.status);
  pool=$(basename $pool|sed 's/\.status//g');
  echo "$line ${pool}";
done;
