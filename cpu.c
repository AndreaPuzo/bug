#include "env.h"

void cputk (struct cpu_t * cpu, u_word_t num)
{
  cpu->tk += num ;
}

void cpuri (struct cpu_t * cpu, u_word_t num)
{
  num &= 0xF ;

  cpu->irr |= 1 << num  ;
  cpu->irr &= ~cpu->imr ;
}

int cpuhi (struct cpu_t * cpu, u_word_t num, int op)
{
  if (0 == op) {
    /* invoke the interrupt subroutine */

    u_word_t adr = cpu->ivt + num ;
    u_word_t isr = busrd(cpu->bus, adr) ;
    u_word_t atr = isr & 0x3 ;
    isr &= ~0x3 ;

    if (0 == (atr & 0x1))
      return 1 ;

    u_word_t ra ;

    if (0 == (atr & 0x2)) {
      ra = cpu->ip ; /* fault */
    } else {
      ra = cpu->pc ; /* trap  */
    }

    u_word_t sp = cpu->isp ;

    buswr(cpu->bus, --sp, ra)           ; /* save return address   */
    buswr(cpu->bus, --sp, cpu->sr)      ; /* save status           */
    buswr(cpu->bus, --sp, cpu->xr[0xF]) ; /* save frame pointer    */
    buswr(cpu->bus, --sp, cpu->xr[0xE]) ; /* save stack pointer    */
    buswr(cpu->bus, --sp, num)          ; /* save interrupt number */

    cpu->sr     &= ~SRF_EI  ; /* disable interrupts    */
    cpu->sr     &= ~SRF_PL  ; /* enter in machine mode */
    cpu->sr     |= SRF_SI   ; /* serving interrupt     */
    cpu->pc      = isr      ; /* next instruction      */
    cpu->xr[0xF] = cpu->isp ; /* next frame            */
    cpu->xr[0xE] = sp       ; /* next stack            */
  } else {
    /* return from interrupt subroutine */

    u_word_t sp = cpu->isp ;

    cpu->pc      = busrd(cpu->bus, --sp) ; /* restore return address */
    cpu->sr      = busrd(cpu->bus, --sp) ; /* restore status         */
    cpu->xr[0xF] = busrd(cpu->bus, --sp) ; /* restore frame pointer  */
    cpu->xr[0xE] = busrd(cpu->bus, --sp) ; /* restore stack pointer  */
  }

  return 0 ;
}

u_word_t cputr (struct cpu_t * cpu, u_word_t adr, int rwx)
{
  if (0 == cpurd(cpu, 0x50))
    return adr ;

  u_word_t pl  = cpurd(cpu, 0x10) ;
  u_word_t ofs = (adr >>  0) & 0xFFF ; /* segment offset        */
  u_word_t grs = (adr >> 12) & 0x3FF ; /* group segment index   */
  u_word_t prs = (adr >> 22) & 0x3FF ; /* process segment index */
  u_word_t sds ; /* segment descriptor */

  adr = cpu->sdt + prs ;
  sds = busrd(cpu->bus, adr) ;

  if (0 == (sds & 0x1)) {
    cpuri(cpu, INT_SF) ;
    return 0 ;
  }

  if (((sds >> 1) & 0x3) < pl) {
    cpuri(cpu, INT_GP) ;
    return 0 ;
  }

  adr = (sds & ~0xFFF) + grs ;
  sds = busrd(cpu->bus, adr) ;

  if (0 == (sds & 0x1)) {
    cpuri(cpu, INT_SF) ;
    return 0 ;
  }

  if (((sds >> 1) & 0x3) < pl) {
    cpuri(cpu, INT_GP) ;
    return 0 ;
  }

  if (rwx != (((sds >> 3) & 0x7) & rwx)) {
    cpuri(cpu, INT_GP) ;
    return 0 ;
  }

  return (sds & ~0xFFF) | ofs ;
}

u_word_t cpuld (struct cpu_t * cpu, u_word_t adr)
{
  adr = cputr(cpu, adr, 0b100) ;
  return busrd(cpu->bus, adr) ;
}

void cpust (struct cpu_t * cpu, u_word_t adr, u_word_t dat)
{
  adr = cputr(cpu, adr, 0b010) ;
  buswr(cpu->bus, adr, dat) ;
}

u_word_t cpufi (struct cpu_t * cpu)
{
  u_word_t adr = cpu->ip = cpu->pc++ ;
  adr = cputr(cpu, adr, 0b101) ;
  return busrd(cpu->bus, adr) ;
}

int cpurs (struct cpu_t * cpu)
{
  cpu->adr = 0x00000000 ;
  cpu->edr = cpu->adr + 0xFF ;
  cpu->irr = 0 ;
  cpu->imr = 0xFFFF & ~INT_NM ;
  cpu->ivt = 0 ;
  cpu->isp = 0 ;
  cpu->ck  = 0 ;
  cpu->tk  = 0 ;
  cpu->ip  = 0 ;
  cpu->pc  = 0x80000000 ;
  cpu->sr  = 0 ;

  for (int idx = 0 ; idx < 0x10 ; ++idx) {
    cpu->xr[idx] = 0 ;
  }

  return 0 ;
}

u_word_t cpurd (struct cpu_t * cpu, u_word_t adr)
{
  u_word_t dat = 0 ;

  switch (adr) {
  case 0x00 :
    break ;

  case 0x10 :
    dat = (cpu->sr >> SRS_PL) & SRM_PL ;
    break ;

  case 0x20 :
    dat = (cpu->sr >> SRS_SI) & SRM_SI ;
    break ;

  case 0x30 :
    dat = (cpu->sr >> SRS_EI) & SRM_EI ;
    break ;

  case 0x40 :
    dat = (cpu->sr >> SRS_WI) & SRM_WI ;
    break ;

  case 0x50 :
    dat = (cpu->sr >> SRS_SM) & SRM_SM ;
    break ;

  case 0x60 :
    dat = cpu->sr ;
    break ;

  case 0x01 :
    dat = cpu->irr ;
    break ;

  case 0x02 : case 0x12 : case 0x22 : case 0x32 :
  case 0x42 : case 0x52 : case 0x62 : case 0x72 :
  case 0x82 : case 0x92 : case 0xA2 : case 0xB2 :
  case 0xC2 : case 0xD2 : case 0xE2 : case 0xF2 :
    dat = (cpu->irr >> (adr >> 8)) & 0x1 ;
    break ;

  case 0x03 :
    dat = cpu->imr ;
    break ;

  case 0x04 : case 0x14 : case 0x24 : case 0x34 :
  case 0x44 : case 0x54 : case 0x64 : case 0x74 :
  case 0x84 : case 0x94 : case 0xA4 : case 0xB4 :
  case 0xC4 : case 0xD4 : case 0xE4 : case 0xF4 :
    dat = (cpu->imr >> (adr >> 8)) & 0x1 ;
    break ;

  case 0x05 :
    dat = cpu->ivt ;
    break ;

  case 0x06 :
    dat = cpu->isp ;
    break ;

  case 0x07 :
    dat = cpu->sdt ;
    break ;
  }

  return dat ;
}

void cpuwr (struct cpu_t * cpu, u_word_t adr, u_word_t dat)
{
  u_word_t pl = cpurd(cpu, 0x10) ;

  if (0x3 == pl) {
    cpuri(cpu, INT_GP) ;
    return ;
  }

  switch (adr) {
  case 0x00 :
    break ;

  case 0x10 :
    cpu->sr &= ~SRF_PL ;
    cpu->sr |= (dat & SRM_PL) << SRS_PL ;
    break ;

  case 0x20 :
    cpu->sr &= ~SRF_RN ;
    cpu->sr |= (dat & SRM_RN) << SRS_RN ;
    break ;

  case 0x30 :
    cpu->sr &= ~SRF_EI ;
    cpu->sr |= (dat & SRM_EI) << SRS_EI ;
    break ;

  case 0x40 :
    cpu->sr &= ~SRF_WI ;
    cpu->sr |= (dat & SRM_WI) << SRS_WI ;
    break ;

  case 0x50 :
    cpu->sr &= ~SRF_SM ;
    cpu->sr |= (dat & SRM_SM) << SRS_SM ;
    break ;

  case 0x60 :
    cpu->sr = dat ;
    break ;

  case 0x01 :
    break ;

  case 0x02 : case 0x12 : case 0x22 : case 0x32 :
  case 0x42 : case 0x52 : case 0x62 : case 0x72 :
  case 0x82 : case 0x92 : case 0xA2 : case 0xB2 :
  case 0xC2 : case 0xD2 : case 0xE2 : case 0xF2 :
    break ;

  case 0x03 :
    cpu->imr = (dat & 0xFFFF) & ~INT_NM ;
    break ;

  case 0x04 : case 0x14 : case 0x24 : case 0x34 :
  case 0x44 : case 0x54 : case 0x64 : case 0x74 :
  case 0x84 : case 0x94 : case 0xA4 : case 0xB4 :
  case 0xC4 : case 0xD4 : case 0xE4 : case 0xF4 : {
    u_word_t num = adr >> 8 ;
    cpu->imr &= ~(0x1 << num) ;
    if (INT_NM == num) {
      cpu->imr |= (dat & 0x1) << num ;
    }
  } break ;

  case 0x05 :
    cpu->ivt = dat ;
    break ;

  case 0x06 :
    cpu->isp = dat ;
    break ;

  case 0x07 :
    cpu->sdt = dat ;
    break ;
  }
}

void cpuck (struct cpu_t * cpu)
{
  /* complete the previous instruction */

  if (0 != cpu->tk)
    return ;

  cpu->xr[0] = 0 ; /* hardwired to ground */

  /* check for interrupts */

  int found_int = 0 ;

  if (0 != cpurd(cpu, 0x30)) {
    do {
      if (0 == cpu->irr)
	      break ;

      u_word_t num ;

      for (num = 0 ; num < 0x10 ; ++num) {
        if (0 != ((cpu->irr >> num) & 0x1))
          break ;
      }

      cpu->irr &= ~(0x1 << num) ;
      found_int = 0 == cpuhi(cpu, num, 0) ;
    } while (0 == found_int) ;
  }

  if (0 != cpurd(cpu, 0x40) && 0 == found_int)
    return ;

  /* fetch the next instruction */

  cpu->tk = 0 ;
  u_word_t ins = cpufi(cpu) ;

  /* decode the instruction */

  u_word_t o = (ins >>  0) & 0xF ;
  u_word_t a = (ins >>  4) & 0xF ;
  u_word_t b = (ins >>  8) & 0xF ;
  u_word_t f = (ins >> 12) & 0xF ;
  u_word_t c = (ins >> 16) & 0xF ;
  u_word_t u = (ins >> 20) & 0xFFF ;
  s_word_t s = (s_word_t)ins >> 20 ;

  /* execute the instruction */

  switch (o) {
  case 0x0 : {
    u_word_t * rd  = cpu->xr + a ;
    u_word_t   rs0 = cpu->xr[b]  ;
    u_word_t   rs1 = cpu->xr[c]  ;

    switch (f) {
    case 0x0 : /* add  */
      *rd = (u_word_t)rs0 + (u_word_t)(rs1 + s) ;
      break ;

    case 0x1 : /* sub  */
      *rd = (u_word_t)rs0 - (u_word_t)(rs1 + s) ;
      break ;
 
    case 0x2 : /* umul */
      *rd = (u_word_t)rs0 * (u_word_t)(rs1 + s) ;
      break ;
 
    case 0x3 : /* udiv */
    case 0x4 : /* umod */ {
      u_word_t div = (u_word_t)(rs1 + s) ;

      if (0 != div) {
        if (0x3 == f) {
          *rd = (u_word_t)rs0 / div ;
        } else {
          *rd = (u_word_t)rs0 % div ;
        }
      } else {
	      cpuri(cpu, INT_DZ) ;
      }
    } break ;

    case 0x5 : /* smul */
      *rd = (s_word_t)rs0 * (s_word_t)(rs1 + s) ;
      break ;
 
    case 0x6 : /* sdiv */
    case 0x7 : /* smod */ {
      s_word_t div = (s_word_t)(rs1 + s) ;

      if (0 != div) {
        if (0x6 == f) {
          *rd = (s_word_t)rs0 / div ;
        } else {
          *rd = (s_word_t)rs0 % div ;
        }
      } else {
	      cpuri(cpu, INT_DZ) ;
      }
    } break ;

    case 0x8 : /* and  */
      *rd = (u_word_t)rs0 & (u_word_t)(rs1 + s) ;
      break ;

    case 0x9 : /* or   */
      *rd = (u_word_t)rs0 | (u_word_t)(rs1 + s) ;
      break ;

    case 0xA : /* eor  */
      *rd = (u_word_t)rs0 ^ (u_word_t)(rs1 + s) ;
      break ;

    case 0xB : /* nand */
      *rd = ~((u_word_t)rs0 & (u_word_t)(rs1 + s)) ;
      break ;

    case 0xC : /* nor  */
      *rd = ~((u_word_t)rs0 | (u_word_t)(rs1 + s)) ;
      break ;

    case 0xD : /* shl  */
      *rd = (u_word_t)rs0 << (u_word_t)(rs1 + s) ;
      break ;

    case 0xE : /* ushr */
      *rd = (u_word_t)rs0 >> (u_word_t)(rs1 + s) ;
      break ;

    case 0xF : /* sshr */
      *rd = (s_word_t)rs0 >> (u_word_t)(rs1 + s) ;
      break ;
    }
  } break ;
 
  case 0x1 : {
    u_word_t rd  = cpu->xr[c] ;
    u_word_t rs0 = cpu->xr[a] ;
    u_word_t rs1 = cpu->xr[b] ;

    switch (f) {
    case 0x0 : /* bne  */
      if ((u_word_t)rs0 != (u_word_t)rs1) {
	      cpu->pc += rd + s ;
      }
      break ;

    case 0x1 : /* beq  */
      if ((u_word_t)rs0 == (u_word_t)rs1) {
	      cpu->pc += rd + s ;
      }
      break ;

    case 0x2 : /* ublt */
      if ((u_word_t)rs0 < (u_word_t)rs1) {
	      cpu->pc += rd + s ;
      }
      break ;

    case 0x3 : /* uble */
      if ((u_word_t)rs0 <= (u_word_t)rs1) {
	      cpu->pc += rd + s ;
      }
      break ;

    case 0x4 : /* sblt */
      if ((s_word_t)rs0 < (s_word_t)rs1) {
	      cpu->pc += rd + s ;
      }
      break ;

    case 0x5 : /* sble */
      if ((s_word_t)rs0 <= (s_word_t)rs1) {
	      cpu->pc += rd + s ;
      }
      break ;

    case 0x6 : /* jal  */
      cpu->xr[a] = cpu->pc ;
      cpu->pc = rd + ((s << 4) | b) ;
      break ;

    case 0x7 : /* rjal */
      cpu->xr[a] = cpu->pc ;
      cpu->pc += rd + ((s << 4) | b) ;
      break ;

    case 0x8 : /* movl */ {
      u_word_t val = (u << 4) | c ;
      cpu->xr[a] = (cpu->xr[b] & ~0xFFFF) | val ;
    } break ;

    case 0x9 : /* movh */ {
      u_word_t val = ((u << 4) | c) << 16 ;
      cpu->xr[a] = (cpu->xr[b] &  0xFFFF) | val ;
    } break ;

    case 0xA : /* ldr  */
      cpu->xr[a] = cpuld(cpu, cpu->xr[b] + cpu->xr[c] + s) ;
      break ;

    case 0xB : /* str  */
      cpust(cpu, cpu->xr[b] + cpu->xr[c] + s, cpu->xr[a]) ;
      break ;

    case 0xC : /* ldi  */
      cpu->xr[a] = (s << 8) | (c << 4) | b ;
      break ;

    case 0xD : /* int  */
      cpuri(cpu, a) ;
      break ;

    case 0xE : /* rti */
      cpuhi(cpu, 0, 1) ;
      break ;

    case 0xF : /* rst */
      busrs(cpu->bus) ;
      break ;
    }
  } break ;

  default :
    cpuri(cpu, INT_UD) ;
    break ;
  }

  ++cpu->ck ;
}
