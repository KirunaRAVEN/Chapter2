# RAVEN Chapter 2 Software
Chapter 2 is the fully functional test-bench.

## TODO
#### Needed:
- [x] TCP communication
- [x] UART communication
- [x] Functionality
 - [x] Verification
 - [x] Sequence
 - [x] Sensor data aquisition
 - [x] Hardware integration
 - [x] Automated flashing
#### Wanted
- [ ] Automated software testing
- [ ] Library for data.csv
- [ ] Wrappers for struct and pin access
- [ ] External eyes on the code
- [ ] A sane person to write the `README`

## Hardware design 
``` 

 |------------|   Raw TCP  |------------|  Analog  |-------------|
 | G/S Laptop |-------<----| Test bench |-----<----| H/W Sensors |
 |------------|            | (Portenta) |          |-------------|
                           |------------|
 |------------|   UART        |  |  |     GPIO   |---------------|
 | Controlbox |--------->-----|  |  |---->-------| H/W actuators |
 |------------|                  |               |---------------|
                                 |                         |
                             I2C |   |-----------------|   | Analog
                                 |-<-| Control Sensing |-<-|
                                     |-----------------|
```
## Structure
* `portenta`: all the code for the microcontroller in the testbench.
* `toolKit`: all tools that do not involve the test itself, such as updaters and analysis tools.
* `interface`: everything related to the G/S laptop.
* `controlBox`: code for the microcontroller in the controlbox.

## Design philosophy
As the lead programmer, and the one leading the charge for the refactoring, I am tasked with the impossible; code that will outlive me. As such, I am spending too much of my time writing documentation and justifications for my actions. I am under consultation with as many people as I can to make sure that the code is good. What is good? Efficient? Sure. Elegant? I'd hope so. But my primary focus is on maintainability. Is worth is to sacrifice efficiency and elegance for maintainability? I'd argue that it is. But it is such a hard-defined goal. I've been on this endeavour a while at this point, and I'll bear it for the rest of my life. I know that my name will be cursed, as I have cursed others. But I pray, with every line of code, that I've made a difference.

#### Portenta
The main loop of the program on is implemented with a large `switch`-`case`, as the software has different modes it can switch between. These modes and transitions are very few, however, and I regret to say that this choice has been grandfathered in. Same for the `TEST` and `SEQUENCE` coroutines. This might be good practice, but I'm honestly not sure.

Another thing under consideration is the amount of files. How many files are too many, and when is a file too large? One thing I know for certain that will change is the direct struct and sensor access. These will be placed in a wrapper soon, and all wrappers will probably be in the same file. 

#### Other things
The communications suite *needs* to be fixed. As of now, there are three different implementations of the reader/writer of `data.csv`. This *will* be put into a single module, so it can be changed on the fly without any issues. Same thing with the TCP communication. As of right now, any changes on the portenta needs to be manually mirrored on the G/S laptop.

## Pinmap (for GPIO on the portenta)
**Outputs**
| Pin        | Control          | Name in code      | Comment                   |
|:-----------|:-----------------|:------------------|:--------------------------|
|`CAN1_TX`   |`SIREN_SIG`       |`SIREN_SIGNAL`     | Siren control             |
|`CAMERA_D0N`|`LIGHT_SIG`       |`LIGHT_SIGNAL`     | Light control             |
|`CAMERA_D1N`|`HEATBLANKET1_SIG`|`HEATING1_RELAY`   | Heating blanket 1         |
|`CAMERA_D2N`|`HEATBLANKET2_SIG`|`HEATING2_RELAY`   | Heating blanket 2         |
|`GPIO_4`    |`IGN_SIG_1`       |`IGNITION_ARM`     | Ignition signal 1 (arm)   |
|`GPIO_5`    |`IGN_SIG_2`       |`IGNITION_RELAY`   | Ignition signal 2 (fire)  |
|`PWM0`      |`NITROGEN_SIG`    |`NITROGEN_RELAY`   | Nitrogen feeding valve    |
|`PWM1`      |`OXIDIZER1_SIG`   |`OXIDIZER1_RELAY`  | Oxidizer feeding valve    |
|`PWM2`      |`OXIDIZER2_SIG`   |`OXIDIZER2_RELAY`  | Oxidizer feeding valve    |
|`PWM9`      |`DUMP_SIG`        |`DUMP_SIGNAL`      | Dump valve                |
|`PDM_CK`    |`TEST_LED_SIG`    |`TEST_LED_SIGNAL`  | Test mode indicator       |
|`SPI1_CS`   |`HIGH_SPEED_SIG`  |`HIGH_SPEED_SIGNAL`| High-speed camera control |
**Inputs**
| Pin        | Control          | Name in code      | Comment                   |
|`GPIO_0`    |`IGN_SNS`         |`IGNITION_BUTTON`  | Ignition button sensing   |
|`PCIE_CKN`  |`RESET_SNS`       |`RESET_BUTTON`     | Reset button sensing      |
|`PCIE_TXP`  |`TESTMODE_SNS`    |`TEST_MODE_BUTTON` | Testmode button sensing   |
