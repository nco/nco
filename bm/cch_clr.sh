#!/bin/sh
# Flush and clear the disk caches.

if [ -z "$OSTYPE" ]; then
    echo "$0: unknown OS type env-var OSTYPE not set"
    exit 1;
fi

case "$OSTYPE" in
    
  darwin*)
    purge
    ;;
  
  linux*)
    echo "$0: about to clear caches for linux"  
    sync
    sudo sh -c "echo 3 > /proc/sys/vm/drop_caches"
    ;;
	 
  bsd*)
    ;;
  
  # windoze
  msys*)
    ;;

  #default do nothing
  *)
    ;;   

esac

exit 0
