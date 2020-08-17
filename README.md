# gpuctl
  xorg configs go in /etc/X11/xorg.conf.d/
  and use the Xorg option -layout to choose which gpu to run with.

  gpuctl is just a tiny "lspci" to either list busids in decimal or check if available.
  and run X with -layout egpu if the busid exist like in zprofile.
