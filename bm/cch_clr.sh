#!/bin/sh
# Flush and clear the disk caches.

if [[ "$OSTYPE" = "linux-gnu" ]]; then
  sync
  sudo sh -c "echo 3 > /proc/sys/vm/drop_caches"
elif [[ "$OSTYPE" = "macos" ]]; then
  purge
fi    
    
