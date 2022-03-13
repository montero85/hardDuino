hardDuino is an educational project to develop software for Arduino boards for more intermediate users. Sketches and .ino files are great for newbies approaching embedded programming for the first time but can be a limitation for more experienced users who prefer to have more control over the software. I am sure there are already tons of other projects like this. This is my own version that I want to use to experiment a bit with IoT.

Arduino basic framework is maintained (src/avr/core_lib) and built as a core library (like Arduino IDE does). Original main function has been made "weak" (TODO: Check it works). The principal implication of this is that it is still possible to build firmware in the "traditional" way by providing a setup() and loop() function (it can be useful to have a level of backward compatibility with existing projects). It is though also possible for a new project to partially or totally divert from the core framework and re-implement functionalities in different ways. 

BUILD:
The build system is based on the dear old make (more for nostalgia than other) and uses the avr toolchain. Latter can be found in the Arduino IDE itself under <arduino_path>/hardware/tools/avr.
The idea is to have several targets. Each target has a <TARGET>.inc file listing the source files that it is going to need. Core library files are excluded from the list since they will be built independently and anyway.
A target can be build via:
make <TARGET>
TODO:
- add upload process when you define it.

SUPPORTED HARDWARE:
- The reference board is "Arduino Uno Wifi". All the configuration is based around that and (at the moment) mirrors the one performed by Arduino IDE. It should be possible to extend for different boards in a similar way.

LIST OF AVAILABLE TARGETS:
- blink: "hello world" target to test the build/upload process. It is directly derived from "01.Basic/Blink" Example in the IDE.
