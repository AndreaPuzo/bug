# Bug's Environment

Bug is not an independent project but is necessary for the development of the Eve ecosystem which you can find [here](https://github.com/AndreaPuzo/eve).
The goal is to simplify Eve to understand which paradigms will be most useful to make Eve development smoother.

## Structure
Currently Bug is not complete but the idea is to build a small virtual machine with:
* a simplified CPU with 16 32-bit integer registers, a discrete set of RISC-type instructions with 32-bit fixed instructions
  with quite intuitive fields to decode, some registers for managing interruptions (external, internal, faults and abortions);
* a simple SRAM that allows you to temporarily store data (starts at 0x80000000);
* an I/O memory for managing up to 16 external devices such as timers, hard disk, screen and keyboard;
* a 32-bit bus for communication between external devices (whose memory maps reside in I/O space and can be changed)
  and internal devices (whose memory maps are fixed to the sources).

## Usage
Clone the repository to your local machine with the command:
```bash
git clone https://github.com/AndreaPuzo/bug.git
cd bug
```
Currently the CMake file has not yet been created and the virtual machine is built using GCC:
```bash
gcc cpu.c ram.c iom.c bus.c main.c dev/tmr.c dev/dsk.c -o bug
```

## Contributions
For now there is only a scaffolding that will be clearer in the coming months and will grow in parallel with the Eve project.
If you have ideas about it or would like to learn more about low-level programming, communication between hardware and software,
Eve (and Bug) is what you are looking for. Feel free to fork the repository and add your changes (devices, instructions etc.),
don't forget to PR so I can hear about your ideas to combine them with mine and improve this project.

