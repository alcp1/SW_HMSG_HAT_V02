
# SW_HMSG_HAT_V02

## Overall Description
- ATTiny communicates with the Raspberry Pi through I2C. The device addres, and the registers can be found below on the [I2C section](#I2C).
- The Raspberry Pi can configure the ATTiny as an external Watchdog. In this case, if the Raspberry Pi fails to clear a _Reset Timer_ through an I2C command, the ATTiny will cycle the power. All the timing parameters are described in the [I2C section](#I2C).
- If the ATTiny cycles the power due to this _Reset Timer_ expiring, on the next power up, it will no longer cycle the power until the Raspberry Pi enables the External Watchdog functionality again (this means the ATTiny will perform a single power cycle to recover the Raspberry Pi when _Reset Timer_ expires).
- The behaviour of the LED controlled by ATTiny (LED3 on the board) can be found below on the [LED section](#LED).

## Structure

| Path                               | Purpose                                                                                                                             |
|------------------------------------|-------------------------------------------------------------------------------------------------------------------------------------|
| _build                             | The [CMake build tree](https://cmake.org/cmake/help/latest/manual/cmake.1.html#introduction-to-cmake-buildsystems), can be deleted. |
| cmake                              | Generated [CMake](https://cmake.org/) files. May be deleted if user.cmake has not been added                                        |
| .vscode                            | See [VSCode](https://code.visualstudio.com/docs/getstarted/settings)                                                                |
| .vscode\settings.json              | Workspace specific settings                                                                                                         |
| .vscode\SW_HMSG_HAT_V02.mplab.json | The MPLAB project file, should not be deleted                                                                                       |
| out                                | Final build artifacts                                                                                                               |

## Remarks about VS Code:
An error message may show up upon opening the MPLAB project folder about the path length: 
```text
CMake Generate successful but is exceeding path limitations. (exit code 0)
```
A quick fix is to use the windows command subst to create a virtual drive mapped to the source directory with a shorter path: 
```powershell
subst X: C:\Long\Path\To\Your\Project
```
Then, instead of opening the project on C:\Long\Path\To\Your\Project, the shorter path on X: could be used when opening the folder in VS Code.

## MCC Config - Initial Steps
- Open the Command Palette (Ctrl+Shift+P).
- Select MPLAB MCC: Launch.
- Select "Create New MCC Config"
- Select "MPLAB Project" (the current project, not a new standalone config)
- Select "Proceed With Default Values"
- After startup (takes a while), on MCC, click on "Application Builder"
> REMARK: For configuring each module, click on the gear after the name of the module. If needed, click on zoom to view the modules better.

## MCC Device Resources
On the left menu, select "Device Resource", and add the following items:
- ADC0
- I2C0_Client
- NVM 
- Timer: TCA0
- Timer: TCB0
- Timer: Timer
- System: BOD
- System: BOD
- System: RSTCTRL
- System: WDT
> REMARK: ADC0 is for temperature reading. TCB0 is for periodic interrupts. TCA0 is for PWM.


## MCC Config - Modules
### System
#### Interrupt Manager
- Global Interrupt Enable: **ON** (button in _ON_)
- Round-robin Scheduling Enable: **OFF** (button in _OFF_)
- Interrupt Level Priority: **0**
- Interrupt Vector with High Priority: **0**

#### Configuration Bits
- APPEND: 0 ≤ **0**
- BOD Operation in Active Mode: **Enabled**
- BOD Level: **2.6 V**
- BOD Sample Frequency: **1kHz**
- BOD Operation in Sleep Mode: **Enabled**
- BOOTEND: 0 ≤ **0**
- OSCCFG - Frequency Select: **20 MHz**
- SYSCFG0 - CRC Source: **Disable CRC**
- SYSCFG0 - CRC Source: **Disable CRC**
- EEPROM Save: **OFF** (button in _OFF_)
- Reset Pin Configuration: **UPDI Mode**
- SYSCFG1 - Startup Time: **4ms**
- WDTCFG - Watchdog Timeout Period: **32 cycles (32ms)**
- WDTCFG - Watchdog Window Timeout Period: **8 cycles (8ms)**

> REMARK: Do not change "Reset Pin Configuration"! Always use "UPDI Mode" or the module will have to be programmed with high voltage pulse on UPDI pin.

#### CLKCTRL
- Generate Initializer Code: **Initialize all registers**
- CLKCTRL Settings - Clock Source: **20MHz internal oscillator**
- CLKCTRL Settings - Internal Oscillator Frequency: **20 MHz**
- CLKCTRL Settings - Prescaler enable: **ON** (button in _ON_)
- CLKCTRL Settings - Prescaler division: **4X**
- CLKCTRL Settings - System clock out: **OFF** (button in _OFF_)
- Advanced Settings - lock enable: **OFF** (button in _OFF_)
- Advanced Settings - Run standby 20MHz Oscillator: **OFF** (button in _OFF_)
- Advanced Settings - Run standby 32.768kHz Oscillator: **OFF** (button in _OFF_)

#### Pins
- Pin Name: **PA1** 
  - Module: **TWAI0** _(should already be set - greyed out)_
  - Function: **SDA** _(should already be set - greyed out)_
  - Direction: **in/out** _(should already be set - greyed out)_
  - Custom Name: **IO_PA1**
  - Start Hugh: **No** (_not selected_)
  - Inverted I/O: **No** (_not selected_)
  - Pull-Up: **No** (_not selected_)
  - Input/Sense Configuration [ISC]: **Interrupt disabled but input buffer enabled**

- Pin Name: **PA2** 
  - Module: **TWAI0** _(should already be set - greyed out)_
  - Function: **SCL** _(should already be set - greyed out)_
  - Direction: **in/out** _(should already be set - greyed out)_
  - Custom Name: **IO_PA2**
  - Start Hugh: **No** (_not selected_)
  - Inverted I/O: **No** (_not selected_)
  - Pull-Up: **No** (_not selected_)
  - Input/Sense Configuration [ISC]: **Interrupt disabled but input buffer enabled**

- Pin Name: **PA3**
  - Module: **Pins**
  - Function: **GPIO**
  - Direction: **output**
  - Custom Name: **IO_PA3**
  - Start Hugh: **No** (_not selected_)
  - Inverted I/O: **No** (_not selected_)
  - Pull-Up: **No** (_not selected_)
  - Input/Sense Configuration [ISC]: **Interrupt disabled but input buffer enabled**

- Pin Name: **PA6**
  - Module: **Pins**
  - Function: **GPIO**
  - Direction: **input**
  - Custom Name: **SUP_SHDN**
  - Start Hugh: **No** (_not selected_)
  - Inverted I/O: **No** (_not selected_)
  - Pull-Up: **No** (_not selected_)
  - Input/Sense Configuration [ISC]: **Interrupt disabled but input buffer enabled**

- Pin Name: **PA7**
  - Module: **TCA0**
  - Function: **WO0**
  - Direction: **output**
  - Custom Name: **LED_uC**
  - Start Hugh: **No** (_not selected_)
  - Inverted I/O: **No** (_not selected_)
  - Pull-Up: **No** (_not selected_)
  - Input/Sense Configuration [ISC]: **Interrupt disabled but input buffer enabled**

#### WDT
- Lock enable: **ON** (button in _ON_)

#### RSTCTRL
- RSTCTRL Enable: **ON** _(should already be set - greyed out)_

#### BOD
- BOD Operation in Active Mode: **Enabled**
- BOD Level: **2.6 V**
- BOD Sample Frequency: **1kHz**
- BOD Operation in Sleep Mode: **Enabled**
- VLM Interrupt Enable: **OFF** (button in _OFF_)
- VLM Interrupt Flag: **OFF** (button in _OFF_)
- VLM Configuration: **Interrupt when supply goes below VLM level**
- VLM Level: **VLM Level**

### Main
#### TCA0
- Custom Name: **TCA0**
- Generate Initializer Code: **Generate Initializer Code**
- Timer Enable: **ON** (button in _ON_)
- Run Standby Mode: **OFF** (button in _OFF_)
- Timer Enable: **ON** (button in _ON_)
- Run Standby Mode: **OFF** (button in _OFF_)
- Timer Mode: **16 Bit (Normal)**
- Clock Select: **System Clock / 4**
- Count Direction: **UP**
- Requested Period: 2.4μs ≤ **52.4288ms** ≤ 52.4288ms
- Event Action A: **POSEDGE**
- Waveform Generation Mode: **Single Slope PWM**
- Compare Channel 0 Enable: **ON** (button in _ON_)
- Duty Cycle 0 (%): 0 ≤ **0** ≤ 100
- Compare Channel 1 Enable: **OFF** (button in _OFF_)
- Compare Channel 2 Enable: **OFF** (button in _OFF_)
- Overflow Interrupt Enable: **OFF** (button in _OFF_)
- Compare Channel 0 Interrupt Enable: **OFF** (button in _OFF_)
- Compare Channel 1 Interrupt Enable: **OFF** (button in _OFF_)
- Compare Channel 2 Interrupt Enable: **OFF** (button in _OFF_)
- Generate ISR: **OFF** (button in _OFF_)

#### Timer0
- Custom Name: **Timer0**
- Timer Enable: **ON** (button in _ON_)
- Interrupt Driven: **ON** (button in _ON_)
- Requested Timer Period: 400ns ≤ **20**ms ≤ 26.214ms
- Timer PLIB Selector: **TCB0**
##### Timer0 - TCB0
- Custom Name: **TCB0**
- Initializer Code Generate: **Initialize all registers**
- Timer Enable: **ON** _(should already be set - greyed out)_
- Clock Selection: **CLKDIV2**
- Timer Mode: **INT** _(should already be set - greyed out)_
- Requested Timeout: 400ns ≤ **20**ms ≤ 26.214ms _(should already be set - greyed out)_
- Actual Timeout: **20**ms _(should already be set - greyed out)_
- Run Standby: **OFF** _(should already be set - greyed out)_
- Synchronize Update: **OFF** _(should already be set - greyed out)_
- Asynchronous Enable: **OFF** _(should already be set - greyed out)_
- Run in Debug Mode: **OFF** _(should already be set - greyed out)_
- Edge Event: **OFF** _(should already be set - greyed out)_
- Event Input Capture Enable: **OFF** _(should already be set - greyed out)_
- Noise Cancellation Filter Enable: **OFF** _(should already be set - greyed out)_
- Pin Output Enable: **OFF** _(should already be set - greyed out)_
- ISR Generate: **ON** _(should already be set - greyed out)_
- Capture/Timeout Interrupt Enable: **ON** _(should already be set - greyed out)_

#### NVM
- Generate Flash APIs: **ON** (button in _ON_)
- Place Functions in Custom Segment: **OFF** (button in _OFF_)
- Application Code Section Write Protect: **OFF** (button in _OFF_)
- Boot Section Lock: **OFF** (button in _OFF_)
- Generate EEPROM APIs: **ON** (button in _ON_)
- Generate Signature Row APIs: **OFF** (button in _OFF_)
- Generate Fuse APIs: **OFF** (button in _OFF_)
- Enable EEPROM Ready Interrupt: **OFF** (button in _OFF_)

#### ADC0
- Custom Name: **ADC0**
- Hardware Settings - Enable ADC: **ON** (button in _ON_)
- Hardware Settings - Input Configuration: **Single-Ended**
- Hardware Settings - Result Alignment: **Right**
- Hardware Settings - Resolution Selection: **10-bit mode**
- Hardware Settings - Positive Input Channel: **TEMPSENSE**
- Hardware Settings - Positive Voltage Reference: **INTREF**
- Hardware Settings - Start Event Input Enable: **OFF** (button in _OFF_)
- Hardware Settings - Free-Running Mode Enable: **ON** (button in _ON_)
- Hardware Settings - Run in Standby Mode Enable: **OFF** (button in _OFF_)
- Hardware Settings - Run in Debug Mode Enable: **OFF** (button in _OFF_)
- Hardware Settings - Sample Capacitance Selection: **ON** (button in _ON_)
- Computation Settings - Computation Mode: **Basic**
- Computation Settings - Window Comparator Mode: **No Window Comparison**
- Computation Settings - Upper Threshold: -32768 ≤ **0**
- Computation Settings - Lower Threshold: -32768 ≤ **0**
- Clock Settings - Clock Prescaler: **CLK_PER divided by 2**
- Clock Settings - Duty Cycle: **DUTY50**
- Clock Settings - Initialization Delay: **DLY32**
- Clock Settings - Automatic Sampling Delay Variation: **ASVOFF**
- Clock Settings - Sampling Delay: 0 ≤ **0**
- Clock Settings - Sample Length: 0 ≤ **30**
- Interrupt Settings - Result Ready Interrupt Enable: **OFF** (button in _OFF_)
- Interrupt Settings - Window Comparator Interrupt Enable: **OFF** (button in _OFF_)
- Interrupt Settings - Generate Interrupt APIs: **ON** (button in _ON_)

#### I2C0_Client
- Custom Name: **I2C0_Client**
- Clock Stretching: **ON** _(should already be set - greyed out)_
- Client Address: 0x0 ≤ **0x1E** ≤ 0x7F
- Client Mask: 0x0 ≤ **0x7F** ≤ 0x7F
- I2C Client PLIB Selector: **TWI0**
##### I2C0_Client - TWI0_Peripheral
- Interrupt Driven: **ON** (button in _ON_)
- General Call Address Recognition: **ON** (button in _ON_)
- Address/Stop Interrupt Enable: **ON** (button in _ON_)
- Stop Interrupt Enable: **ON** _(should already be set - greyed out)_

#### Main 
- Generate main.c file: **ON** _(should already be set - greyed out)_

## MCC Generate
On the bottom tab, click on "Notifications" to see any messagens that could prevent code generation. After checking, on the left menu, below the MCC Tab, click on "Generate" button.

## I2C
### I2C Address
The Client Address is 0x1E (see the [I2C0_Client Config](#I2C0_Client) for more details). Any address different than this will be ignored.

### I2C Registers
|Register Address|Name|Read/Write|Description|
|----------------|----|----------|-----------|
|**0x00**| Raspberry Pi Command | Written by **Host** only. | Commands sent from the Raspberry Pi: <br>• **0x01**: Reset the _Reset Timer_<br> • **0x11**: Reboot ATTiny <br> • **0x21**: Cycle Power in x seconds (_see address **0x01**_)<br> • **Other values**: Ignored<br>
|**0x01**| Power Off Delay | Written by **Host** only. | Seconds to wait for a "Cycle Power" when this command is issued (_see Register **0x00** command **0x21**_).
|**0x02**| Raspberry Pi Watchdog Enable | Written by **Host** only. | If set to **0x63**, ATTiny will update the _Reset Timer_ (see register address _**0x03**_ and _**0x04**_) every second, and if it reaches the _Reset Timer Limit_ (see register address _**0x05**_ and _**0x06**_), the ATtiny will cycle the power.|
|**0x03**<br>**0x04**| Reset Timer | Written by **Host** and **Client**. | _Reset Timer_ is a 16 bit counter updated every second. Register Address **0x03** contains the LSB, and Register Address **0x04** contains the MSB.|
|**0x05**<br>**0x06**| Reset Timer Limit | Written by **Host** only. <br> _After reset, it is written by Client_. | _Reset Timer Limit_ is a 16 bit value chacked against _Reset Timer_. Register Address **0x05** contains the LSB, and Register Address **0x06** contains the MSB.|
|**0x07**<br>**0x08**| Reset Counter | Written by **Host** and **Client**. | _Reset Counter_ is a 16 bit value saven on the ATTiny EEPROM that counts how many times the ATTiny has cycled the power, triggered by the watchdog mechanism (_Reset Timer_ > _Reset Timer Limit_). Register Address **0x07** contains the LSB, and Register Address **0x08** contains the MSB. <br><br> _**REMARK:** Other Power Cycle mechanisms (such as Raspberry Pi command) will not update this counter!_|
|**0x09**<br>**0x0A**<br>**0x0B**| LED Active Mode parameters | Written by **Host** only. <br> _After reset, it is written by Client_. |Describes how the LED behaves when in active mode (after initialization): <br>• Register **0x09**: The LED _duty_ cycle. 0 means 0%, and 255 means 100% duty cycle for the LED. <br>• Register **0x0A**: The LED _ON_ time (in seconds). <br>• Register **0x0B**: The LED _Period_ time (in seconds).|
|**0x0C**<br>**0x0D**<br>**0x0E**| LED I2C Transmit Signaling parameters | Written by **Host** only. <br> _After reset, it is written by Client_. |Describes how the LED behaves when an I2C transmission finishes - end bit detected:<br>• Register **0x0C**: The LED _duty_ cycle. 0 means 0%, and 255 means 100% duty cycle for the LED. <br>• Register **0x0D**: The LED _ON_ time (in seconds). <br>• Register **0x0E**: The LED _Period_ time (in seconds).|
|**0x0F**| ATTiny Reset Cause | Written by **Client** only. | See the Register **RSTFR** in the ATTiny402 datasheet.|
|**0x10**<br>**0x11**| ATTiny ADC Reading (Temperature) | Written by **Client** only. | Latest ADC Reading for the ATTiny temperature. See "Temperature Measurement" in the datasheet for the calculations to be performed to get the temperature in K. Register Address **0x10** contains the LSB, and Register Address **0x11** contains the MSB.|
|**0x12**<br>**0x13**| ATTiny Sigrow Offset / Gain | Written by **Client** only. | Device calibration for the ATTiny temperature measuremntes. Updated only at reset, as it is fexed for each ATTiny device. See "Temperature Measurement" in the datasheet for the calculations to be performed to get the temperature in K. Register **0x12**: Sigrow Offset. <br>• Register **0x13**: Sigrow Gain.|
|**0x14**<br>**0x15**| SW Version | Written by **Client** only. | SW version as a 16 bit unsigned integer. Register Address **0x14** contains the LSB, and Register Address **0x15** contains the MSB.|

## LED
- After reset, the LED will linearly increase the duty cycle from 0% to 100% (init mode).
- After this init, it will go to active mode, where the LEDs will be turned ON at a configurable time (_Active mode **ON** time_) and duty cycle (_Active mode **duty** cycle_), then will be turned OFF, in such a way that the total (ON time + OFF time) is a configurable time (_Active mode **Period** time_).
- When an I2C transmission finishes (stop bit detected), the LEDs will be turned ON at a configurable time (_Transmit Signaling **ON** time_) and duty cycle (_Transmit Signaling **duty** cycle_), then will be turned OFF, in such a way that the total (ON time + OFF time) is a configurable time (_Transmit Signaling **Period** Time_).
> REMARK: Transmit Signaling has priority over Active mode, meaning if the LED should be ON for Transmit Signaling, but OFF for Active mode, it will be ON.

## ADC
- The ADC module just performs the ADC conversion for the tempoerature channel. The temperature calculation can be performed by the Raspberry Pi using this reading and the temperature calibration fields in order to save program space on the ATTiny.
> REMARK: No action is done by ATTiny with the ADC Reading. It is up to the Raspberry Pi to decide what to do with this information.

## EEPROM
The following fields (I2C Registers) are saved in EEPROM:
- **Raspberry Pi Watchdog Enable**: It is mandatory to save it to EEPROM in case there is a power supply fail (resetting both the ATTiny and Raspberry Pi) and the Raspberry Pi fails to boot after it.
- **Reset Counter**: It is only updated in case the External Watchdog feature was enabled, and the _Reset Timer_ expired. Any other reset / power fail / power cycle mechanism will not increment this counter.