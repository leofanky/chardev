== How to use ==

You need to install kernel headers. Check your distro manual on how to get them.

Run `make modules` to build the module.

To insert the module into kernel run `insmod ./chardev.ko`. You need root privileges for that.
To remove module use `rmmod chardev`.

Kernel messages can be accessed with `dmesg`.
