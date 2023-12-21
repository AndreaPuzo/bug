#ifndef BUS_H
# define BUS_H

struct bus_t {
  struct cpu_t cpu ;
  struct ram_t ram ;
  struct iom_t iom ;
} ;

void     bustk (struct bus_t * bus, u_word_t num) ;
void     busri (struct bus_t * bus, u_word_t num) ;
int      busmn (struct bus_t * bus, struct iodev_t * dev) ;
int      busum (struct bus_t * bus, struct iodev_t * dev) ;
int      busrm (struct bus_t * bus, struct iodev_t * dev) ;
int      busrs (struct bus_t * bus) ;
void     busck (struct bus_t * bus) ;
u_word_t busrd (struct bus_t * bus, u_word_t adr) ;
void     buswr (struct bus_t * bus, u_word_t adr, u_word_t dat) ;

#endif