# Auto-Headlights
Controller designed to control and automate vehicle headlights based on environmental conditions in parallel with vehicle controls.

## Table of Contents
- [Auto-Headlights](#Auto-Headlights)
  - [Table of Contents](#table-of-contents)
  - [About The Project](#about-the-project)
  - [Getting Started](#getting-started)
    - [Prerequisites](#prerequisites)
    - [Installation](#installation)
  - [Usage](#usage)
  - [Changelog](#changelog)

## About The Project
This project was designed for a Subaru Liberty/Legacy 4GEN in an effort to add modern automatic headlights and light home functionalities.

It involves PCB design and automotive wiring. Hence, this project should only be replicated by individuals who are confident with automotive electrical systems.

The design was based around the following requirements:

1. Control headlights dependent on light conditions.
2. Ensure original vehicle controls remained functional.
3. Support reading lock/unlock states.
4. Ability to update software i.e. ensure programing pins are accessible through header/connector.

## Getting Started
To get a build this project (or parts thereof), these steps may be followed.

### Prerequisites
1. PlatformIO is required to build binaries from source code. Microchip Studios or Arduino IDE can also be used but will require extra configuration.
2. An ISP programmer is required to upload to the controller. An Arduino as ISP can be used.
3. If you want to make changes to the PCB design or would like to generate new fabrication files, an installation of KiCAD is required.
4. A 3D printer is required to fabricate the PCB enclosure from the provided CAD files.

### Installation
1. Manufacture and assemble controller PCB.
2. Build and upload code binary.
3. Fabricate PCB enclosure.
4. Splice vehicle wiring as per wiring diragram/requirements.
5. Pin connectors and attach to controller.
6. Mount controller in a secure location.

## Usage
Examples will be given in future iterations of this file.

_For more examples, please refer to the [Documentation](https://)_

## Changelog
All notable changes to this project will be documented in this section.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.0.0/),
and this project attempts to adhere to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

### [v1.0.0]
#### Initial design
- ATmega328PB based design.
- Included optocouplers to read 12V signals from vehicle.
- MOLEX Micro-fit connectors.