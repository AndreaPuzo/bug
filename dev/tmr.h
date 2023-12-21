#ifndef TMR_H
# define TMR_H

# define TSRS_RS 0 /* reset                   */
# define TSRS_CK 1 /* perform ClocK timing    */
# define TSRS_WA 2 /* wait for acknowledgment */
# define TSRS_RP 3 /* repeating timer         */

# define TSRM_RS 0x01
# define TSRM_CK 0x01
# define TSRM_WA 0x01
# define TSRM_RP 0x01

# define TSRF_CK (TSRM_RS << TSRS_RS)
# define TSRF_CK (TSRM_CK << TSRS_CK)
# define TSRF_WA (TSRM_WA << TSRS_WA)
# define TSRF_RP (TSRM_RP << TSRS_RP)

struct tmr_t {
  struct iodev_t iodev ; /* header for I/O DEVice       */
  u_word_t       csr   ; /* Control and Status Register */
  u_word_t       scr   ; /* Start Clock Register        */
  u_word_t       ecr   ; /* End Clock Register          */
} ;

void deftmr (struct tmr_t * tmr, u_word_t id, u_word_t adr, u_word_t len) ;

int      tmrrs (struct tmr_t * tmr) ;
void     tmrck (struct tmr_t * tmr) ;
u_word_t tmrrd (struct tmr_t * tmr, u_word_t adr) ;
void     tmrwr (struct tmr_t * tmr, u_word_t adr, u_word_t dat) ;

#endif