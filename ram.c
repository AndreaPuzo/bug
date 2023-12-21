#include "env.h"

int ramrs (struct ram_t * ram)
{
  u_word_t len = ram->edr - ram->adr + 1 ;

  for (u_word_t ofs = 0 ; ofs < len ; ++ofs) {
    ram->buf[ofs] = 0 ;
  }

  ram->adr = 0x80000000 ;
  ram->edr = ram->adr + len - 1 ;

  return 0 ;
}

void ramck (struct ram_t * ram)
{
  /* do nothing */
}

u_word_t ramrd (struct ram_t * ram, u_word_t adr)
{
  return ram->buf[adr] ;
}

void ramwr (struct ram_t * ram, u_word_t adr, u_word_t dat)
{
  ram->buf[adr] = dat ;
}