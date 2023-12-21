#ifndef ENV_H
# define ENV_H

# include <stdint.h>
# include <stddef.h>

typedef uint8_t  u_byte_t ;
typedef uint16_t u_half_t ;
typedef uint32_t u_word_t ;

typedef int8_t   s_byte_t ;
typedef int16_t  s_half_t ;
typedef int32_t  s_word_t ;

# define overlapping(a, b, c, d) (0 == (b < c || a > d))

# include "ram.h"
# include "cpu.h"
# include "bus.h"
# include "iom.h"

#endif
