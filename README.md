# Moteino

Source to run on Moteino chips. They have radio so we try to make a ad-hoc network.

## Installation

The projects needs platformio to work. See the [installation page](docs.platformio.org/en/latest/installation.html)

platformio installs the required libs, compiles the used libs in a static way, links them to the present sources and allows to push the code of a project to device.

## Execution

you need to go in a project folder and type
 platfomio run --target upload
to push the .hex to the device.

## Documentation

see the pdf in the [doc folder](doc/moteino.pdf)
