# NeuroDesign
## Code overview:
  - Servo + EMG folder - contains all code for working with servos and EMGs (Units 1 and 2)
    - (advanced) **AutoArm_SweepTest** - for testing the functionality of each servo in the arm
    - (intermediate/class on Feb 3) **EMG_Analogread** - for reading EMG signal with analogRead() command, and driving a servo arm conditionally
    - (early/class on Jan 28) **ThresholdSweep** - developed in class on Jan 28. Uses a random value to selectively drive a servo motor.

## Resources for EMG Sensor Devices

### Spiker:bit
[Spiker:bit code to control a servo](https://makecode.microbit.org/S47669-98849-21590-38433)

### MyoWare 2.0
**Note** The Myoware model is relatively new and there is little support an tested example code available. The following guidelines are based on my testing. I will reach out to the company to get more information hopefully by Feb 3. 
To work with the input from this device, use the ~analogRead(PIN)~ command and replace "PIN" with the pin variable or A0, A1, etc.


Hookup Table:
| MyoWare Pin    | Uno Pin        | Notes/conditions             |
| :------------- | :------------: | ---------------------------: |
|       GND      |     GND        | Usually white colored pin    |
|       Vin      |   5V or 3.3V   | Sensor will work w/both      |
|       Env      |       --       | Overdamped values. Don't use |
|       Rect     |       A0       | Signal smoothing needed (2/3)|
|       Raw      |       A1       | Optional use.                |

