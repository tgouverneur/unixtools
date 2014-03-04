#!/bin/bash
##
# get the pid of the listening port
# passwd in argument. (Solaris 10)
#
# $ sudo ./getpid.sh 22
#  13185: /usr/lib/ssh/sshd
#
# wildcat - 2008
##
port=$1

cd /proc
for pid in [1-9][0-9]*; do
  pfiles $pid | nawk -v port=$port '
   NR == 1 {
     pid=$1
     line=$2
   }
   found=0
   /sockname/ && $NF == port {
     getline
     if ( ! /peername/ ) {
       found=1
       exit
     } 
   }
   END {
    if ( found == 1 ) {
      printf("%d: %s\n", pid, line)
      exit 42
    } 
   }'
   if [ $? -eq 42 ]; then
     exit;
   fi
done;
