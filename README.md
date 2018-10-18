# BESS
Battery Energy Storage System

## Dependancies
### Linux Environment
``` console
sudo apt-get update
sudo apt-get upgrade
sudo apt-get install git build-essential gcc g++ cmake make
mkdir ~/dev ~/src
```
### libmodbus
``` console
cd ~/src
git clone https://github.com/stephane/libmodbus
cd libmodbus
sudo apt-get install automake autoconf libtool
./autogen.sh
./configure
sudo make install
```
#### Test
Open two terminals run the random-test-server in one and the random-test-client in the other. The client will send a variety of response to the server and should response with test success if all signals are processed correctly.

``` console
cd src/libmodbus/tests
./random-test-server
```

``` console
cd src/libmodbus/tests
./random-test-client
```
### BOOST
The boost libraries will be sued to create the XML property trees used for modbus querries. Follow the getting started to download and move the bost libraries into the ~/src directory with other libraries.

https://www.boost.org/doc/libs/1_66_0/more/getting_started/unix-variants.html

## Install
``` console
cd ~/dev
git clone https://github.com/Tylores/BESS
```

### SunSpec Models
The last thing that must be added is the sunspec XML models.
 ``` console
cd ~/dev/BESS
git clone https://github.com/sunspec/models
 ```

### Setup
1. Open /BESS/tools/build-run.sh
2. Modify "CPU" to reflect the system you are working on

``` console
cd ~/dev/BESS/tools
./build-run.sh
```
## Use
The Command Line Interface (CLI).

```
        [Help]
> q             quit
> h             help
> i <watts>     import power
> e <watts>     export power
> p             print properties
```

## Class UML

<p align="center">
  <img src="EWH-class-uml.png" alt="Class UML">
</p>
