#ifndef RAM_H
# define RAM_H

struct ram_t {
  struct bus_t * bus ;
  u_word_t       adr ;
  u_word_t       edr ;
  u_word_t *     buf ;
} ;

int      ramrs (struct ram_t * ram) ;
void     ramck (struct ram_t * ram) ;
u_word_t ramrd (struct ram_t * ram, u_word_t adr) ;
void     ramwr (struct ram_t * ram, u_word_t adr, u_word_t dat) ;

#endif