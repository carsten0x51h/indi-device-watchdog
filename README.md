![cmake workflow](https://github.com/carsten0x51h/indi-device-watchdog/actions/workflows/cmake.yml/badge.svg)

INDI Device Watchdog
====================

## Summary
This software project aims to improve the overall availability and connectivity of INDI devices after a connection loss without the need to restart the INDI server. This is especially helpful for astronomy devices with often used USB plugs which have become a bit unreliable. The indi-device-watchdog monitors a list of specified INDI devices and their corresponding Linux devices. Under certain conditions it directly instructs the INDI server to restart an INDI driver without restarting the entire INDI server. This can improve the overall reliability of the setup. Furthermore, the watchdog can be instructed to automatically connect INDI devices whenever they get disconnected or when the setup was just started.


## Supported platforms
The software was tested on Ubuntu 20.04 LTS, Ubuntu 22.04, Ubuntu 23.10 and Raspbian GNU/Linux 12 (bookworm).


## Supported INDI versions
The software currently supports older INDI versions (e.g. 1.8.4) with the old API and also new INDI versions (e.g. 2.0.4) with the new API. 


## Motivation
In a perfect world you wouldn't need this piece of software. Unfortunately, the world is not always perfect - especially when it comes to IT. When using the INDI software in the field with my astro imaging hardware I ran into multiple unreliabilities which in the end made some imaging sessions fail and caused some frustation. The main reason: Unreliable USB connections mainly due to drained plugs.

Over the years, some of the USB ports on my astro equipment have become a little wonky and slightly unreliable. Still, replacing an entire astronomy camera because of that is way to expensive (at least for me). Loosing the connection (even just for a second) leads to a chain of events:

 1. It often took minutes to realize that a device had physically lost connection.
 2. The INDI driver did not realize the unavailability of the Linux device and thought it was still connected.
 3. Trying to disconnect the INDI driver made the driver not respond anymore or sometimes - if disconnecting worked - reconnecting the INDI driver mostly failed.
 4. Some INDI drivers hung or crashed due to temporary unavailability of the Linux device (some drivers do not detect when a Linux devices disappears or do not handle this situation correctly)
 5. Physically reconnecting the device worked on the Linux level but on the INDI level it often did not (even after making sure that the Linux device is always the same after a physical reconnect).
 6. Often the only chance to solve this problem was to restart the INDI server (I later learned that this is not always required - see section [Controlling the INDI server](#controlling-the-indi-server) below).

Restarting the INDI server in the middle of an imaging session typically ruins the entire image and even worse - the telescope alignment may have to be repeated. Two of those fails typically ruin an entire astro evening. To avoid such prolems in the future I did some research and came up with this little program - the "indi-device-watchdog".


## The idea
The idea is simple: A hardware device - say an USB camera - has a representation on the Linux OS level (typically something like /dev/video0) and a representation within the INDI server as an INDI driver (e.g. indi_v4l2_ccd). The watchdog can monitor the Linux device and the corresponding INDI device and try to manage connection of the INDI device in case the Linux device disappears or re-appears. Since the INDI server allows to stop and start dedicated device drivers without restarting the entire INDI server (see section [Controlling the INDI server](#controlling-the-indi-server) below), the indi-device-watchdog can also restart a dedicated device driver in case the Linux device exists but the crresponding INDI device is not available or connecting the INDI device fails. The following illustration gives an overview:

![INDI Device Watchdog System Overview](doc/images/indi-device-watchdog-overview.svg)


## Further information
Please visit my blog https://www.lost-infinity.com for further details and potentially related projects.


## build

### Install required dependencies
The following libraries are required to build the INDI device watchdog on Ubuntu 20.04 LTS / Ubuntu 22.04 or Raspberry Pi OS.
For other Linux distributions the package names may slightly vary.

	sudo apt-get update
	sudo apt-get install zlib1g-dev libindi-dev libboost-dev \
	                     libboost-log-dev libboost-system-dev libboost-regex-dev \
	                     libboost-program-options-dev \
	                     libboost-test-dev clang-tidy

### Checkout repository

	git clone https://github.com/carsten0x51h/indi-device-watchdog.git

### Configure the build
To build from command line, simply do the following steps:

	cd indi-device-watchdog
	mkdir build
	cd build
	cmake ..

This will generate the build environment for your operating system. It will fail
if at least one dependency to an external library could not be resolved.


### Build the code
Run the following command to build the project: 

	cmake --build . -- all

or to build parallel on e.g. 12 cores, run

	cmake --build . -j12 -- all

### Create device configuration
To tell the INDI device watchdog which INDI devices and corresponding Linux devices it should monitor, a JSON configuration is used. An example of such file is shown below. In addition for each device the corresponding INDI driver name shall be specified to allow restart of this driver if necessary (unfortunately this information is currently not available from INDI under all given circumstances). Furthermore, for each entry the option if automatically connecting the INDI device is available. When enabled the INDI device watchdog tries to connect the respective INDI device. 

```
{
    "indiDevices": [
        {
            "indiDeviceName": "Joystick",
            "linuxDeviceName": "\/dev\/input\/js0",
            "indiDeviceDriverName": "indi_joystick",
            "enableAutoConnect": "true"
        },
        {
            "indiDeviceName": "V4L2 CCD",
            "linuxDeviceName": "\/dev\/video1",
            "indiDeviceDriverName": "indi_v4l2_ccd",
            "enableAutoConnect": "true"
        },
        {
            "indiDeviceName": "MoonLite",
            "linuxDeviceName": "\/dev\/serial\/by-id\/usb-FTDI_FT232R_USB_UART_A400rRDV-if00-port0",
            "indiDeviceDriverName": "indi_moonlite_focus",
            "enableAutoConnect": "true"
        },
        {
            "indiDeviceName": "Atik EFW2",
            "linuxDeviceName": "\/dev\/hidraw81",
            "indiDeviceDriverName": "indi_atik_wheel",
            "enableAutoConnect": "true"
        },
        {
            "indiDeviceName": "EQMod Mount",
            "linuxDeviceName": "\/dev\/serial\/by-id\/usb-FTDI_FT232R_USB_UART_A600ztuh-if00-port0",
            "indiDeviceDriverName": "indi_eqmod_telescope",
            "enableAutoConnect": "false"
        }
    ]
}
```

### Controlling the INDI server

The INDI server provides a simple file-based interface to stop and start INDI drivers while the INDI server is running. This allows restarting single INDI device drivers without the need to restart the entire server. To achieve that two simple steps are required.

1. Create a pipe - e.g. in the /tmp folder:

```
sudo mkfifo /tmp/indiserverFIFO
sudo chmod 664 /tmp/indiserverFIFO
```

2. Start the INDI server with the -f option passing the name of the pipe which you hust created. 

```
sudo indiserver -f /tmp/indiserverFIFO <Your INDI drivers>
```

To send commands to the INDI server you can now simply write to the pipe. For example to stop and start the "indi_v4l2_ccd" INDI driver without restarting the entire INDI server, the following commands can be used:

```
sudo echo "stop indi_v4l2_ccd" > /tmp/indiserverFIFO
sudo echo "start indi_v4l2_ccd" > /tmp/indiserverFIFO
```

The INDI device watchdog makes use of this mechanism to restart an INDI driver in case the corresponding Linux device exists but the INDI device does not.



### Run the INDI device watchdog
To run the INDI device watchdog the first time simply execute the following command to get an overview of the options:

```
./indi_device_watchdog --help
INDI device watchdog options:
  -h [ --help ]                         Display this parameter overview
  -H [ --hostname ] arg (=localhost)    Set hostname of INDI server
  -p [ --port ] arg (=7624)             Port of INDI server.
  -T [ --timeout ] arg (=3)             Timeout in seconds.
  -B [ --indi-bin ] arg (=/usr/bin)     Search path for INDI binaries.
  -P [ --indi-server-pipe ] arg (=/tmp/indiserverFIFO)
                                        Pipe which should be used to write 
                                        commands to the INDI server.
  -D [ --device-config ] arg            Config file with devices to monitor.
  -v [ --verbose ] arg                  Print more verbose messages at each 
                                        additional verbosity level.	

```


Simple example: Assuming the INDI server runs on localhost on the default port and the INDI binaries are located in /usr/bin, the command boils down to:

	sudo ./indi_device_watchdog -D my-indi-device-config.json


### Allow the INDI device watchdog to monitor devices which have no representation in /dev (e.g. Atik 383L+)

Some USB devices have no representation in the /dev folder of the Linux system. However, the INDI device watchdog currently checks for such a file to determine if a device is available on the Linux level or not. In order to make a device visible on that level to the watchdog, udev rules can be used. Each USB device - when plugged in - sends a bunch of information to the PC. In most cases the vendor ID and the product ID are already sufficient to identify a certain device. The udev daemon can be conigured to create and remove a temporary file when a given device is plugged in or removed. For this purpose "udev" rules are used. There are tons of details available on the web about this topic. Just in short: The command "lsusb" helps to identify the vendor ID and the product ID of a given device.

	lsusb

For my Atik383L+ camera I added the following udev rules to /etc/udev/rules.d/84-atik383.rules.
The rules create and remove a file in the /tmp directory when my Atik383L+ camera is plugged in/out.

```
ACTION=="add", ATTRS{idVendor}=="20d6", ATTRS{idProduct}=="da30", RUN+="/usr/bin/touch /tmp/atik383"
ACTION=="remove", ENV{PRODUCT}=="20d6/da30/d49f", RUN+="/bin/rm -rf /tmp/atik383"
```
This is probably a hack and there are better ways to do it but for now it works. Feel free to drop me a note when you have a better idea. I may incorporate it into the INDI device watchdog.
