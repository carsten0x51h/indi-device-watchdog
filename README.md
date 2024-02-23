![cmake workflow](https://github.com/carsten0x51h/indi-device-watchdog/actions/workflows/cmake.yml/badge.svg)

INDI Device Watchdog
====================

## Summary
TODO...


## Project vision
TODO...


## System overview
TODO...

![INDI Device Watchdog System Overview](doc/images/system_overview.jpg)

## Further information
Please visit my blog https://www.lost-infinity.com for further details. The first article I published about this project can be found here: https://www.lost-infinity.com/indi-device-watchdog


## build

### Install required dependencies
The following libraries are required to build FoFi on Ubuntu 20.04 LTS.
For other Linux distributions the package names may slightly vary.

	sudo apt-get update
	sudo apt-get install zlib1g-dev libindi-dev libboost-dev \
	                     libboost-log-dev libboost-system-dev \
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

The rest of this section is optional. To get finer grained control over the cmake process
the following options are available (the first one is always the _default_): 

	cmake .. -DOPTION_BUILD_DOC=OFF|ON
		 -DCMAKE_BUILD_TYPE=RELEASE|DEBUG|COVERAGE|PERF
		 -DCMAKE_VERBOSE_MAKEFILE:BOOL=OFF|ON
		 -DOPTION_ENABLE_CLANG_TIDY=ON|OFF
		 -DCMAKE_CXX_CLANG_TIDY="clang-tidy;-checks=-*,readability-*"
		 -DCMAKE_C_COMPILER=clang|gcc
		 -DCMAKE_CXX_COMPILER=clang++|g++


### Build the code
Run the following command to build the project: 

	cmake --build . -- all

or to build parallel on e.g. 12 cores, run

	cmake --build . -j12 -- all

### Run the program
To run the INDI device watchdog simply execute

    ./indi_device_watchdog

...