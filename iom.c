#include "env.h"

void iomri (struct iom_t * iom, struct iodev_t * dev)
{
  busri(iom->bus, 0x8 | dev->id) ;
}

int iommn (struct iom_t * iom, struct iodev_t * dev)
{
  if (NULL == dev)
    return 1 ;

  int idx ;

  for (idx = 0 ; idx < 0x10 ; ++idx) {
    if (NULL == iom->dev[idx])
      break ;
  }

  if (0x10 == idx)
    return 2 ; /* busy */

  struct iodev_t ** ptr = iom->dev + idx ;

  for (idx = 0 ; idx < 0x10 ; ++idx) {
    if (NULL == iom->dev[idx])
      continue ;

    if (overlapping(
        dev->smr, dev->emr,
        iom->dev[idx]->smr, iom->dev[idx]->emr
      )
    ) return 3 ; /* overlapped */
  }

  *ptr     = dev ;
  dev->iom = iom ;
  dev->id &= 0x7 ;

  return 0 ;
}

int iomum (struct iom_t * iom, struct iodev_t * dev)
{
  if (NULL == dev)
    return 1 ;

  for (int idx = 0 ; idx < 0x10 ; ++idx) {
    if (dev == iom->dev[idx]) {
      dev->iom      = NULL ;
      iom->dev[idx] = NULL ;
      return 0 ;
    }
  }

  return 1 ;
}

int iomrm (struct iom_t * iom, struct iodev_t * dev)
{
  if (0 != iomum(iom, dev))
    return 1 ; /* not present */

  return iommn(iom, dev) ;
}

int iomrs (struct iom_t * iom)
{
  iom->adr = 0x00000100 ;
  iom->edr = 0x7FFFFFFF ;

  for (int idx = 0 ; idx < 0x10 ; ++idx) {
    if (NULL == iom->dev[idx])
      continue ;

    if (NULL == iom->dev[idx]->rs)
      continue ;

    if (0 != iom->dev[idx]->rs(iom->dev + idx))
      return 1 ;
  }

  return 0 ;
}

void iomck (struct iom_t * iom)
{
  for (int idx = 0 ; idx < 0x10 ; ++idx) {
    if (NULL == iom->dev[idx])
      continue ;

    if (NULL == iom->dev[idx]->ck)
      continue ;

    iom->dev[idx]->ck(iom->dev + idx) ;
  }
}

u_word_t iomrd (struct iom_t * iom, u_word_t adr)
{
  u_word_t dat = 0 ;

  for (int idx = 0 ; idx < 0x10 ; ++idx) {
    if (NULL == iom->dev[idx])
      continue ;

    if (NULL == iom->dev[idx]->rd)
      continue ;

    if (iom->dev[idx]->smr <= adr && adr <= iom->dev[idx]->emr) {
      dat = iom->dev[idx]->rd(iom->dev + idx, adr - iom->dev[idx]->smr) ;
    }
  }

  return dat ;
}

void iomwr (struct iom_t * iom, u_word_t adr, u_word_t dat)
{
  u_word_t dat = 0 ;

  for (int idx = 0 ; idx < 0x10 ; ++idx) {
    if (NULL == iom->dev[idx])
      continue ;

    if (NULL == iom->dev[idx]->wr)
      continue ;

    if (iom->dev[idx]->smr <= adr && adr <= iom->dev[idx]->emr) {
      iom->dev[idx]->wr(iom->dev + idx, adr - iom->dev[idx]->smr, dat) ;
    }
  }

  return dat ;
}