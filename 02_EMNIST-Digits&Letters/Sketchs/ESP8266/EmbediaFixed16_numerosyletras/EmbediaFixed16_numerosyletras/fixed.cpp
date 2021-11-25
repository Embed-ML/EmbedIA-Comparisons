#include "fixed.h"

//////////////////////////////////// funciones de conversion de tipos ////////////////////////////////////
    
    // Devuelve un numero en punto fijo a partir de uno en punto flotante
    fixed float_to_fixed(float f){
        return FLOAT_TO_FIXED(f);
    }   
    // Devuelve un numero en punto fijo a partir de un entero
    fixed int_to_fixed(int32_t i){
        return INT_TO_FIXED(i);    
    }
  
    // Devuelve la representacion en punto flotante de doble precision de un numero en punto fijo
    double fixed_to_double(fixed f){
        return FIXED_TO_DOUBLE(f);
    }
    
    // Devuelve la representacion en punto flotante de un numero en punto fijo
    float fixed_to_float(fixed f){
        return FIXED_TO_FLOAT(f);
    }
    
    // Devuelve el entero a partir de un numero en punto fijo
    int32_t fixed_to_int(fixed f){
        return FIXED_TO_INT(f);
    }
   

     
    
//////////////////////////////////// funciones aritmeticas ////////////////////////////////////
   
    // Devuelve la suma en punto fijo entre a y b
    fixed fixed_add(fixed a, fixed b){
        return FIXED_ADD(a, b);
    }
    
    // Devuelve la resta en punto fijo entre a y b
    fixed fixed_sub(fixed a, fixed b){
        return FIXED_SUB(a, b);
    }
    
    // Devuelve la multiplicacion en punto fijo entre a y b
    fixed fixed_mul(fixed a, fixed b){
        return FIXED_MUL(a, b);
    }
    
    // Devuelve la division en punto fijo entre a y b
    fixed fixed_div(fixed a, fixed b){
        return FIXED_DIV(a,b);
    }


//////////////////////////////////// funciones especiales ////////////////////////////////////
    
    
    // Devuelve la raiz cuadrada del numero a o -1 en caso de error
    fixed fixed_sqrt(fixed a)
    {
      int invert = 0;
      int iter = FIX_FRC_SZ;
      int l, i;
    
      if (a < 0)
        return (-1);
      if (a == 0 || a == FIX_ONE)
        return (a);
      if (a < FIX_ONE && a > 6) {
        invert = 1;
        a = fixed_div(FIX_ONE, a);
      }
      if (a > FIX_ONE) {
        int s = a;
    
        iter = 0;
        while (s > 0) {
          s >>= 2;
          iter++;
        }
      }
    
      /* Newton's iterations */
      l = (a >> 1) + 1;
      for (i = 0; i < iter; i++)
        l = (l + FIXED_DIV(a, l)) >> 1;
      if (invert)
        return (FIXED_DIV(FIX_ONE, l));
      return (l);
    }
    
    // Devuelve el valor de exp(a) 
    fixed fixed_exp(fixed fp)
    {
      fixed xabs, k, z, R, xp;
      const fixed LN2 = FLOAT_TO_FIXED(0.69314718055994530942);
      const fixed LN2_INV = FLOAT_TO_FIXED(1.4426950408889634074);
      const fixed EXP_P[5] = {
        FLOAT_TO_FIXED(1.66666666666666019037e-01),
        FLOAT_TO_FIXED(-2.77777777770155933842e-03),
        FLOAT_TO_FIXED(6.61375632143793436117e-05),
        FLOAT_TO_FIXED(-1.65339022054652515390e-06),
        FLOAT_TO_FIXED(4.13813679705723846039e-08),
      };
    
      if (fp == 0)
        return (FIX_ONE);
      xabs = fixed_abs(fp);
      k = fixed_mul(xabs, LN2_INV);
      k += FIX_HALF;
      k &= ~FIX_FRC_MSK;
      if (fp < 0)
        k = -k;
      fp -= fixed_mul(k, LN2);
      z = fixed_mul(fp, fp);
      /* Taylor */
      R = FIX_TWO +
          fixed_mul(z, EXP_P[0] + fixed_mul(z, EXP_P[1] +
          fixed_mul(z, EXP_P[2] + fixed_mul(z, EXP_P[3] +
          fixed_mul(z, EXP_P[4])))));
      xp = FIX_ONE + fixed_div(fixed_mul(fp, FIX_TWO), R - fp);
      if (k < 0)
        k = FIX_ONE >> (-k >> FIX_FRC_SZ);
      else
        k = FIX_ONE << (k >> FIX_FRC_SZ);
      return (fixed_mul(k, xp));
    }


    // Devuelve x * 2^exp
    fixed fixed_ldexp(fixed x, int exp){
        return fixed_mul(x, fixed_pow(FIX_TWO, exp));
    }
 
    // Devuelve el logaritmo de x
    fixed fixed_log(fixed x)
    {
      fixed log2, xi;
      fixed f, s, z, w, R;
      const fixed LN2 = FLOAT_TO_FIXED(0.69314718055994530942);
      const fixed LG[7] = {
        FLOAT_TO_FIXED(6.666666666666735130e-01),
        FLOAT_TO_FIXED(3.999999999940941908e-01),
        FLOAT_TO_FIXED(2.857142874366239149e-01),
        FLOAT_TO_FIXED(2.222219843214978396e-01),
        FLOAT_TO_FIXED(1.818357216161805012e-01),
        FLOAT_TO_FIXED(1.531383769920937332e-01),
        FLOAT_TO_FIXED(1.479819860511658591e-01)
      };
    
      if (x < 0)
        return (0);
      if (x == 0)
        return 0xffffffff;
    
      log2 = 0;
      xi = x;
      while (xi > FIX_TWO) {
        xi >>= 1;
        log2++;
      }
      f = xi - FIX_ONE;
      s = fixed_div(f, FIX_TWO + f);
      z = fixed_mul(s, s);
      w = fixed_mul(z, z);
      R = fixed_mul(w, LG[1] + fixed_mul(w, LG[3]
          + fixed_mul(w, LG[5]))) + fixed_mul(z, LG[0]
          + fixed_mul(w, LG[2] + fixed_mul(w, LG[4]
          + fixed_mul(w, LG[6]))));
      return (fixed_mul(LN2, (log2 << FIX_FRC_SZ)) + f
          - fixed_mul(s, f - R));
    }
      
    
    // Devuelve el logaritmo de base b del valor x
    fixed fixed_logn(fixed x, fixed base)
    {
      return (fixed_div(fixed_log(x), fixed_log(base)));
    }
    
    
    // Devuelve n^exp
    fixed fixed_pow(fixed n, fixed exp)
    {
      if (exp == 0)
        return (FIX_ONE);
      if (n < 0)
        return 0;
      return (fixed_exp(fixed_mul(fixed_log(n), exp)));
    }
    

//////////////////////////////////// funciones adicionales ////////////////////////////////////
   
    // Devuelve el valor absoluto de a
    fixed fixed_abs(fixed a){
        return FIXED_ABS(a);
    }

    // Devuelve el entero menor x tal que  x >= a
    fixed fixed_ceil(fixed a){
        return FIXED_CEIL(a);
    }

    // Devuelve el entero mayor x tal que  x <= a
    fixed fixed_floor(fixed a){
        return FIXED_FLOOR(a); 
    }



