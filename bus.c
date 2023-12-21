#include "env.h"

void bustk (struct bus_t * bus, u_word_t num)
{
  cputk(&bus->cpu, num) ;
}

void busri (struct bus_t * bus, u_word_t num)
{
  cpuri(&bus->cpu, num) ;
}

int busmn (struct bus_t * bus, struct iodev_t * dev)
{
  return iommn(&bus->iom, dev) ;
}

int busum (struct bus_t * bus, struct iodev_t * dev)
{
  return iomum(&bus->iom, dev) ;
}

int busrm (struct bus_t * bus, struct iodev_t * dev)
{
  return iomrm(&bus->iom, dev) ;
}

int busrs (struct bus_t * bus)
{
  if (0 != cpurs(&bus->cpu))
    return 1 ;

  if (0 != ramrs(&bus->ram))
    return 1 ;

  if (0 != iomrs(&bus->iom))
    return 1 ;

  return 0 ;
}

void busck (struct bus_t * bus)
{
  cpuck(&bus->cpu) ;
  memck(&bus->cpu) ;
  iomck(&bus->iom) ;
}

u_word_t busrd (struct bus_t * bus, u_word_t adr)
{
  u_word_t dat = 0 ;

  if (bus->cpu.adr <= adr && adr <= bus->cpu.edr) {
    dat = cpurd(&bus->cpu, adr - bus->cpu.adr) ;
  } else if (bus->ram.adr <= adr && adr <= bus->ram.edr) {
    dat = ramrd(&bus->ram, adr - bus->ram.adr) ;
    bustk(bus, 4) ;
  } else if (bus->iom.adr <= adr && adr <= bus->iom.edr) {
    dat = iomrd(&bus->iom, adr - bus->iom.adr) ;
    bustk(bus, 4) ;
  } else {
    busri(bus, INT_BF) ;
  }

  return dat ;
}

void buswr (struct bus_t * bus, u_word_t adr, u_word_t dat)
{
  if (bus->cpu.adr <= adr && adr <= bus->cpu.edr) {
    cpuwr(&bus->cpu, adr - bus->cpu.adr, dat) ;
  } else if (bus->ram.adr <= adr && adr <= bus->ram.edr) {
    ramwr(&bus->ram, adr - bus->ram.adr, dat) ;
    bustk(bus, 4) ;
  } else if (bus->iom.adr <= adr && adr <= bus->iom.edr) {
    iomwr(&bus->iom, adr - bus->iom.adr, dat) ;
    bustk(bus, 4) ;
  } else {
    busri(bus, INT_BF) ;
  }
}
