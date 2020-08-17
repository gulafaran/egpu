 if [[ ! $DISPLAY && $XDG_VTNR -eq 1 ]]; then
   if gpuctl -g 5:0:0 ; then
     exec systemd-cat startx -- -layout egpu
   else
     exec systemd-cat startx -- -layout igpu
   fi
 fi
