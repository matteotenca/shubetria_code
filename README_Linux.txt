********** THIS DOCUMENT IS INCOMPLETE AND A WORK IN PROGRESS ******

                                    Phoebetria

                                User Guide for Linux


2.      Installation
        2.1     Users and groups

                2.1.1   Accessing the Recon device

                The BitFenix(R) Recon is not automatically recognised by Linux.
                There are two ways that Phoebetria can connect to the device:

                a)  running Phoebetria as the root user (not recommended); or
                b)  adding udev rules so that users belonging to a
                    "fancontroller" group (or group of your choice) have read
                    and write access to the device

                Option (b) is the recommendation and is straightforward to set
                up.

                2.1.2   Setting up udev

                Pheobetria requires read and write access to the BitFenix(R)
                recon USB device. The recommended approach is to create a
                user group that has read/write permissions for the device and
                then add users to that group. This can be achieved using udev
                rules.

                For brevity the following instructions are based upon creating
                a user group called "fancontrollers" whose members have access
                to the device. You can of course use your own group name or
                configure a different rule -- the only important thing is that
                the user running Phoebetria has read and write permissions for
                the device.

                a) Open a terminal and switch user to root:

                    su -

                b) Add the new user group

                    groupadd fancontrollers

                c) Create or edit /etc/udev/rules.d/10-local.rules and add the
                   following rule:

                   SUBSYSTEM=="usb", ATTRS{idVendor}=="0c45",
                   ATTRS{idProduct}=="7100", NAME="bfxrecon",
                   GROUP="fancontrollers"

                   The above rule must be on *one* line (not as it is presented
                   above) in the .rules file.

                   Save the file.

                d)  Add the user account that you want to use to run Phoebetria
                    to the "fancontrollers" group:

                    usermod -a -G fancontrollers username

                e)  Load the new udev rules

                    ************** INSERT ********************

