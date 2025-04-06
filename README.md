# Fork of [MacroPad mini](https://github.com/wagiminator/CH552-Macropad-mini/) for the Knob version

![Macropad_knob.jpg](https://raw.githubusercontent.com/Puding07/CH552-Macropad-mini-cncjs/main/documentation/Macropad_knob.JPG)

## Configured for CNCjs

- Key bottom: Feed hold
- Key middle: Homing
- Key top:    Unlock
- Knob switch: Axis toggle (X/Y/Z)
- Knob clockwise: Jog forward
- Knob counter clockwise: Jog backward

## For first boot:
you need to jump 3.3V to D+ with a 10K Ohm resistor

![Awful_boot_soldering.jpg](https://raw.githubusercontent.com/Puding07/CH552-Macropad-mini-cncjs/main/documentation/Awful_boot_soldering.JPG)

## Rest:
Can enter boot mode by pressing `Key bottom` while connecting USB - white LED should turn on

## Compiling firmware:
`make bin`

## Flashing firmware:

### Windows:
Install drivers based on [ch55xduino](https://github.com/DeqingSun/ch55xduino?tab=readme-ov-file#driver-for-windows) documentation.
Then use `Arduino CLI`/`WCHISPTool` or the provided tool in `software/tools/chprog.py` (in the script there is HOW TO GUIDE)

### Linux:
Depending on your distro but [make sure you have permission](https://www.reddit.com/r/linux/comments/2j43jl/how_to_make_arduino_ide_and_arch_linux_communicate/) for /dev/{devices}
Then use `Arduino CLI` or the provided tool in `software/tools/chprog.py` (in the script there is HOW TO GUIDE)

## The rest of the original documentation is [here](https://github.com/wagiminator/CH552-Macropad-mini/?tab=readme-ov-file#ch552e-macropad-mini)