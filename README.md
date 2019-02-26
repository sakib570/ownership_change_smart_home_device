# chownIoT Prototype for Smart Home Device 
This project implements a prototype for smart home device features proposed by chownIoT to protect the privacy of both new and previous owner during the ownership change of IoT devices

## Dependencies
### Hardware Dependencies
* Bluetooth
* Wi-Fi

### Software Dependencies
* OS: Tested on Ubuntu 18.04/16.04 & Raspbian Jessie
* Library: Bluez 

Install the bluetooth library using the following command: 
```bash
sudo apt install libbluetooth-dev
```
	
## How to build and run the project
### Building and Running the Project
First clone the project and navigate to the *ownership_change_smart_home_device* directory in Terminal using following command:
```bash
cd path/ownership_change_smart_home_device
```
Once you are inside the *ownership_change_smart_home_device* folder first build the project with the following command:
```bash
make clean all
```
To run project you need to have the *sudo* rights in the working computer. With sudo rights to run the project enter the following command:
```bash
sudo ./ownership_change
```

### Building and Running the Test
To build and run the test at first navigate to *test* folder located in the *src* directory of the project using the following command:
```bash
cd path/ownership_change_smart_home_device/src/test
```
Once you are in the *test* folder build the project with following command:
```bash
make clean all
```
To run the project enter the following command:
```bash
./unit_test
```


## Commit convention 
The following prefixes are used as a convention for the commit messages of the project:
*feat: Adding new feature or functionality in the project
*fix: Any changes related to fixing any bug or issues
*refactor: Any change related to refactoring the existing code to improve readability
*chore: Updating the Makefile or any other work not related to change in actual code base
*test: Adding, fixing or any other changes related to test
*doc: Any changes related to the documentation of the project