# NeuroDesign
## Code overview:
  - **Balance EMG folder** - contains all code for Unit 2 - working with 2 channels and producing coactivation index values.
    - single_EMG_norm - code to record from one module with normalization (MVC)
    - dual_EMG_norm - code to record from two modules with normalized signals.
    - dual_EMG_CI - returns coActivation index, plus normalized values for each muschle<br>
  - **Servo + EMG folder** - contains all code for Unit 1 - working with servos and EMGs (Units 1 and 2)
    - (advanced) **AutoArm_SweepTest** - for testing the functionality of each servo in the arm
    - (intermediate/class on Feb 3) **EMG_Analogread** - for reading EMG signal with analogRead() command, and driving a servo arm conditionally
    - (early/class on Jan 28) **ThresholdSweep** - developed in class on Jan 28. Uses a random value to selectively drive a servo motor.

## Resources for EOG recording (Weds 2/18)
 - Code will be in the Balance EOG folder
 - Visual stimulus to evoke L/R eye movement: [p5.js example](https://editor.p5js.org/sazamore/full/Y1YfNpO1e)

## Resources for EMG Sensor Devices

### Spiker:bit
[Spiker:bit code to control a servo](https://makecode.microbit.org/S47669-98849-21590-38433)

### MyoWare 2.0
**Note** The Myoware model is relatively new and there is little support an tested example code available. The following guidelines are based on my testing. I will reach out to the company to get more information hopefully by Feb 3.
[Available for purchase here](https://www.sparkfun.com/myoware-2-muscle-sensor.html)

#### Working with this device:
  Arduino devices (Uno or Nano recommended)
  Use the `analogRead(PIN)` command and replace "PIN" with the pin variable or A0, A1, etc.
#### Wiring Guide:

![Myoware Sensor](https://www.sparkfun.com/media/catalog/product/cache/a793f13fd3d678cea13d28206895ba0c/D/E/DEV-27924-Myoware-Muscle-Sensor-Back.jpg)

| MyoWare Pin    | Uno Pin        | Notes/conditions             |
| :------------- | :------------: | ---------------------------: |
|       GND      |     GND        | Usually white colored pin    |
|       Vin      |   5V or 3.3V   | Sensor will work w/both      |
|       Env      |       --       | Overdamped values. Don't use |
|       Rect     |       A0       | Signal smoothing needed (2/3)|
|       Raw      |       A1       | Optional use.                |


### Bio Amp EXG Pill
A small, versitile device that can measure EMG, ECG, EOG and EEGs. It has an [extensive GitHub repository](https://github.com/upsidedownlabs/BioAmp-EXG-Pill), with clear examples and videos in the **Software** folder. 
[Available for purchase here](https://www.tindie.com/products/upsidedownlabs/bioamp-exg-pill-sensor-for-ecg-emg-eog-or-eeg/) - Note a long lead time (~10 days) and potential customs fee, as it ships from India.

#### Working with this device:
  Arduino devices (Uno or Nano recommended)
  Use the `analogRead(PIN)` command and replace "PIN" with the pin variable or A0, A1, etc.

#### Wiring Guide (Nano shown):
![BioEXG Sensor](https://www.crowdsupply.com/img/1535/518ed265-c363-4bf9-a268-4705e3a01535/bioamp-exg-pill-electromyography_png_md-xl.jpg)

