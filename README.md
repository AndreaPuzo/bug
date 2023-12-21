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
