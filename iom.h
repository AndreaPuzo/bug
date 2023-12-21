#ifndef IOM_H
# define IOM_H

typedef int      ( * iodevrs_t ) (struct iodev_t *) ;
typedef void     ( * iodevck_t ) (struct iodev_t *) ;
typedef u_word_t ( * iodevrd_t ) (struct iodev_t *, u_word_t) ;
typedef void     ( * iodevwr_t ) (struct iodev_t *, u_word_t, u_word_t) ;

struct iodev_t {
  struct iom_t * iom ; /* connection to the bus */
  u_word_t       smr ; /* Start Memory Register */
  u_word_t       emr ; /* End Memory Register   */
  u_word_t       id  ; /* interrupt ID          */
  iodevrs_t      rs  ; /* ReSet handler         */
  iodevck_t      ck  ; /* ClocK handler         */
  iodevrd_t      rd  ; /* ReaD handler          */
  iodevwr_t      wr  ; /* WRite handler         */
} ;

# include "dev/dsk.h"
# include "dev/tmr.h"

struct iom_t {
  struct bus_t *   bus          ; /* connection to the BUS */
  u_word_t         adr          ; /* I/O start ADdRess     */
  u_word_t         edr          ; /* I/O End aDdRess       */
  struct iodev_t * dev [ 0x10 ] ; /* connected DEVices     */
} ;

void     iomri (struct iom_t * iom, struct iodev_t * dev) ;
int      iommn (struct iom_t * iom, struct iodev_t * dev) ;
int      iomum (struct iom_t * iom, struct iodev_t * dev) ;
int      iomrm (struct iom_t * iom, struct iodev_t * dev) ;
int      iomrs (struct iom_t * iom) ;
void     iomck (struct iom_t * iom) ;
u_word_t iomrd (struct iom_t * iom, u_word_t adr) ;
void     iomwr (struct iom_t * iom, u_word_t adr, u_word_t dat) ;

#endif