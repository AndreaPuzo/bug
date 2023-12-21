#include "env.h"

void deftmr (struct tmr_t * tmr, u_word_t id, u_word_t adr, u_word_t len)
{
  tmr->iodev.rs  = (iodevrs_t)tmrrs ;
  tmr->iodev.ck  = (iodevck_t)tmrck ;
  tmr->iodev.rd  = (iodevrd_t)tmrrd ;
  tmr->iodev.wr  = (iodevwr_t)tmrwr ;
  tmr->iodev.id  = id ;
  tmr->iodev.iom = NULL ;
  tmr->iodev.smr = adr ;
  tmr->iodev.emr = adr + len - 1 ;
  tmr->csr       = 0 ;
  tmr->scr       = 0 ;
  tmr->ecr       = 0 ;
}

#define CPU_CK tmr->iodev.iom->bus->cpu.ck

int tmrrs (struct tmr_t * tmr)
{
  tmr->csr = 0 ;
  tmr->scr = 0 ;
  tmr->ecr = 0 ;

  return 0 ;
}

void tmrck (struct tmr_t * tmr)
{
  if (0 == (tmr->csr & TSRF_CK))
    return ;

  u_word_t ck = CPU_CK ;

  if (tmr->ecr < ck) {
    tmr->csr &= ~TSRF_CK ;
    tmr->csr |=  TSRF_WA ;
    iomri(tmr->iodev.iom, (struct iodev_t *)tmr) ;
  }
}

// 0x00 -> adr (rw)
// 0x01 -> edr (r)
// 0x02 -> csr (rw)
// 0x12 -> csr.ck (rw)
// 0x22 -> csr.wa (rw)
// 0x32 -> csr.rp (rw)
// 0x03 -> scr (rw)
// 0x04 -> ecr (rw)
// 0x05 -> eps (r)

u_word_t tmrrd (struct tmr_t * tmr, u_word_t adr)
{
  u_word_t dat = 0 ;

  switch (adr) {
  case 0x00 :
    dat = tmr->iodev.smr ;
    break ;

  case 0x01 :
    dat = tmr->iodev.emr ;
    break ;

  case 0x02 :
    dat = tmr->csr ;
    break ;

  case 0x12 :
    dat = (tmr->csr >> TSRS_RS) & TSRM_RS ;
    break ;

  case 0x22 :
    dat = (tmr->csr >> TSRS_CK) & TSRM_CK ;
    break ;

  case 0x32 :
    dat = (tmr->csr >> TSRS_WA) & TSRM_WA ;
    break ;

  case 0x42 :
    dat = (tmr->csr >> TSRS_RP) & TSRM_RP ;
    break ;

  case 0x03 :
    dat = tmr->scr ;
    break ;

  case 0x04 :
    dat = tmr->ecr ;
    break ;

  case 0x05 :
    dat = tmr->ecr - tmr->scr + 1 ;
    break ;

  case 0x06 :
    dat = CPU_CK - tmr->scr ;
    break ;
  }

  return dat ;
}

// TODO: write fields
void tmrwr (struct tmr_t * tmr, u_word_t adr, u_word_t dat)
{
  switch (adr) {
  case 0x00 : {
    u_word_t start = tmr->iodev.smr ;
    u_word_t len   = tmr->iodev.emr - tmr->iodev.smr + 1 ;
    tmr->iodev.smr = dat ;
    tmr->iodev.emr = dat + len - 1 ;

    if (0 != iomrm(tmr->iodev.iom, (struct iodev_t *)tmr)) {
      tmr->iodev.smr = start ;
      tmr->iodev.emr = start + len - 1 ;

      (void) iommn(tmr->iodev.iom, (struct iodev_t *)tmr) ;
    }

    (void) tmrrs(tmr) ;
  } break ;

  case 0x01 :
    dat = tmr->iodev.emr ;
    break ;

  case 0x02 :
    dat = tmr->csr ;
    break ;

  case 0x12 :
    tmr->csr &= ~TSRM_RS ;
    tmr->csr |= (dat & TSRM_RS) << TSRS_RS ;
    break ;

  case 0x22 :
    tmr->csr &= ~TSRM_CK ;
    tmr->csr |= (dat & TSRM_CK) << TSRS_CK ;
    break ;

  case 0x32 : {
    u_word_t wa = 0 != (tmr->csr & TSRF_WA) ;

    tmr->csr &= ~TSRM_WA ;
    tmr->csr |= (dat & TSRM_WA) << TSRS_WA ;

    if (wa && 0 == dat) {
      if (0 != (tmr->csr & TSRF_RP)) {
        u_word_t dt = tmr->ecr - tmr->scr + 1 ;

        tmr->csr = TSRF_CK ;
        tmr->scr = CPU_CK ;
        tmr->ecr = tmr->scr + dt - 1 ;
      } else {
        tmr->csr = 0 ;
        tmr->scr = 0 ;
        tmr->ecr = 0 ;
      }
    }
  } break ;

  case 0x42 :
    tmr->csr &= ~TSRM_RP ;
    tmr->csr |= (dat & TSRM_RP) << TSRS_RP ;
    break ;

  case 0x03 :
    if (0 == dat) {
      tmr->csr = 0 ;
      tmr->scr = 0 ;
      tmr->ecr = 0 ;
    } else {
      tmr->scr = CPU_CK ;
      tmr->ecr = tmr->scr + dat - 1 ;
    }
    break ;

  case 0x04 :
    dat = tmr->ecr ;
    break ;

  case 0x05 :
    dat = tmr->ecr - tmr->scr + 1 ;
    break ;

  case 0x06 : {
    u_word_t ck = tmr->iodev.iom->bus->cpu.ck ;
    dat = ck - tmr->scr ;
  } break ;
  }
}
