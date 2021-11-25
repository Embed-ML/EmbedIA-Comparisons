#ifndef FIXED_H
#define FIXED_H

#include <stdint.h>



#define FIX_SIZE 32

#ifndef FIX_SIZE
#define FIX_SIZE 32
#endif



#if FIX_SIZE == 32

#warning representacion de 32 bits en punto fijo

#define FIX_FRC_SZ 18
typedef int32_t fixed;
typedef int64_t dfixed;

#elif FIX_SIZE == 16


#warning representacion de 16 bits en punto fijo

#define FIX_FRC_SZ 9
typedef int16_t fixed;
typedef int32_t dfixed;


#elif FIX_SIZE == 8

#warning representacion de 8 bits en punto fijo

#define FIX_FRC_SZ 3
typedef int8_t fixed;
typedef int16_t dfixed;


#endif


#define FIX_INT_SZ (FIX_SIZE - FIX_FRC_SZ)

#define FIX_FRC_MSK  (((fixed)1 << FIX_FRC_SZ) - 1)

// some constant

#define FIX_ZERO 0
#define FIX_ONE ((fixed)((fixed)1 << FIX_FRC_SZ))
#define FIX_TWO (FIX_ONE + FIX_ONE)
#define FIX_HALF (FIX_ONE >> 1)
#define FIX_PI FLOAT_TO_FIXED(3.14159265358979323846)
#define FIX_E  FLOAT_TO_FIXED(2.7182818284590452354)
#define FIX_MAX (fixed)((dfixed(1) << (FIX_SIZE-1)) -1)
// convertion macros
#define FLOAT_TO_FIXED(F) ((fixed)((F) * FIX_ONE + ((F) >= 0 ? 0.5 : -0.5)))
#define INT_TO_FIXED(I) ((fixed)(I) << FIX_FRC_SZ)
#define FL2FX(F) FLOAT_TO_FIXED(F)


#define FIXED_TO_DOUBLE(F) ((double) ((F)*((double)(1)/(double)(1L << FIX_FRC_SZ))))
#define FIXED_TO_FLOAT(F) ((float) ((F)*((float)(1)/(float)(1L << FIX_FRC_SZ))))
#define FIXED_TO_INT(F) ((fixed)(F) >> FIX_FRC_SZ)
#define FIXED_FRAC(F) ( (fixed)(F) & FIX_FRC_MSK )
#define FIXED_INT(F) ( (fixed)(F) & ~FIX_FRC_MSK )



// arithmetic macros

#define FIXED_ADD(A,B) ((A) + (B))

#define FIXED_SUB(A,B) ((A) - (B))

/*#define FIXED_MUL(A,B)            \
  ((fixed)(((fixed)(A) * (fixed)(B)) >> FIX_FRC_SZ))
*/
#define FIXED_MUL(A,B)            \
  ((fixed)(((dfixed)(A) * (dfixed)(B)) >> FIX_FRC_SZ))

#define DFIXED_MUL(A,B)            \
  ((dfixed)(((dfixed)(A) * (dfixed)(B)) >> FIX_FRC_SZ))

#define FIXED_DIV(A,B)           \
  ((fixed)(((dfixed)(A) << FIX_FRC_SZ) / (dfixed)(B)))



// extra macros
#define FIXED_ABS(A) ((A) < 0 ? -(A) : (A))

#define FIXED_CEIL(A) ( FIXED_INT(A) +  (FIXED_FRAC(A) ? FIX_ONE : 0) )
#define FIXED_FLOOR(A) ( FIXED_INT(A) )



    // convertion functions


    fixed float_to_fixed(float f);


    fixed int_to_fixed(int32_t i);


    double fixed_to_double(fixed f);


    float fixed_to_float(fixed f);


    int32_t fixed_to_int(fixed f);


    // arithmetic functions

    fixed fixed_add(fixed a, fixed b);


    fixed fixed_sub(fixed a, fixed b);



    fixed fixed_mul(fixed a, fixed b);


    fixed fixed_div(fixed a, fixed b);



    // special functions
    /* Returns the square root of the given number, or -1 in case of error */

    fixed fixed_sqrt(fixed a);

    /* Returns the value exp(x), i.e. e^x of the given fixed number. */
    //static inline
    fixed fixed_exp(fixed fp);


    /* Returns x multiplied by 2 raised to the power of exp */
    fixed fixed_ldexp(fixed x, int exp);




    /* Returns the natural logarithm of the given fixed number. */
    //static inline
    fixed fixed_log(fixed x);


    /* Returns the logarithm of the given base of the given fixed number */

    fixed fixed_logn(fixed x, fixed base);


    /* Return the power value (n^exp) of the given fixed numbers */

    fixed fixed_pow(fixed n, fixed exp);



    // extra functions

    fixed fixed_abs(fixed a);


    fixed fixed_ceil(fixed a);


    fixed fixed_floor(fixed a);



#endif
