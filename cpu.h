#ifndef CPU_H
# define CPU_H

/* INTerrupt number */
# define INT_RS 0x00 /* ReSet                  */
# define INT_NM 0x01 /* Non-Maskable interrupt */
# define INT_UD 0x02 /* UnDefined instruction  */
# define INT_BF 0x03 /* Bus Fault              */
# define INT_SF 0x04 /* Segmentation Fault     */
# define INT_DZ 0x05 /* Division by Zero       */
# define INT_GP 0x06 /* General Protection     */

/* Status Register Shifts */
# define SRS_PL 0 /* Privilege Level        */
# define SRS_RN 2 /* RuNning                */
# define SRS_SI 3 /* Serving Interrupt      */
# define SRS_EI 4 /* Enable Interrupts      */
# define SRS_WI 5 /* Wait for Interrupt     */
# define SRS_SM 6 /* Segmented memory Model */

/* Status Register Masks */
# define SRM_PL 0x03
# define SRM_RN 0x01
# define SRM_SI 0x01
# define SRM_EI 0x01
# define SRM_WI 0x01
# define SRM_SM 0x01

/* Status Register Falgs */
# define SRF_PL (SRM_PL << SRS_PL)
# define SRF_RN (SRM_RN << SRS_RN)
# define SRF_SI (SRM_SI << SRS_SI)
# define SRF_EI (SRM_EI << SRS_EI)
# define SRF_WI (SRM_WI << SRS_WI)
# define SRF_SM (SRM_SM << SRS_SM)

struct cpu_t {
  struct bus_t * bus         ; /* connection to the BUS       */
  u_word_t       adr         ; /* I/O start ADdRess           */
  u_word_t       edr         ; /* I/O End aDdRess             */
  u_word_t       irr         ; /* Interrupt Requests Register */
  u_word_t       imr         ; /* Interrupt Masks Register    */
  u_word_t       ivt         ; /* Interrupt Vectors Table     */
  u_word_t       isp         ; /* Interrupt Stack Pointer     */
  u_word_t       sdt         ; /* Segment Descriptors Table   */
  u_word_t       ck          ; /* ClocK counter               */
  u_word_t       tk          ; /* instruction TicKs           */
  u_word_t       ip          ; /* Instruction Pointer         */
  u_word_t       pc          ; /* Program Counter             */
  u_word_t       sr          ; /* Status Register             */
  u_word_t       xr [ 0x10 ] ; /* X Registers                 */
} ;

void     cputk (struct cpu_t * cpu, u_word_t num) ;
void     cpuri (struct cpu_t * cpu, u_word_t num) ;
int      cpuhi (struct cpu_t * cpu, u_word_t num, int op) ;
u_word_t cputr (struct cpu_t * cpu, u_word_t adr, int rwx) ;
u_word_t cpuld (struct cpu_t * cpu, u_word_t adr) ;
void     cpust (struct cpu_t * cpu, u_word_t adr, u_word_t dat) ;
u_word_t cpufi (struct cpu_t * cpu) ;
int      cpurs (struct cpu_t * cpu) ;
void     cpuck (struct cpu_t * cpu) ;
u_word_t cpurd (struct cpu_t * cpu, u_word_t adr) ;
void     cpuwr (struct cpu_t * cpu, u_word_t adr, u_word_t dat) ;

#endif
