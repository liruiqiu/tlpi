#!/bin/bash

# Make sure umask is sane
umask 022

# Set up a default search path.
PATH="/sbin:/usr/sbin:/bin:/usr/bin:/home/cent/bin"
export PATH

#repo sync
echo $$ > repo_sync.pid
repo sync -f -j10 > repo_sync.log 2>&1
if [[ $? -eq 0 ]]
then
    touch succ
fi
while [ $? -ne 0 ]
do
    repo sync -f -j10 > repo_sync.log 2>&1
    if [[ $? -eq 0 ]]
    then
        touch succ
    fi
done
