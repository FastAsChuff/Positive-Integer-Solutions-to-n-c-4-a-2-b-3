//===============================================================================================================================
// npc4eqa2pb3.c: 
//===============================================================================================================================
// Author: Simon Goater Dec 2025
// -----------------------------
// Prints positive integer solutions to the Diophantine equation n + c^4 = a^2 + b^3.
// COPYRIGHT: Software is given as is without guarantee or warranty. It is offered free to use, modify, copy, or distribute
// with conspicuous attribution for any purpose.
//===============================================================================================================================
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <string.h>
#include <time.h>
#include <stdbool.h>
#include <assert.h>
#include "/home/simon/bitarray.c" // See https://github.com/FastAsChuff/Prime-Gaps-Search/blob/main/bitarray.c

// https://math.stackexchange.com/questions/5116900/for-any-integer-n-does-the-equation-n-a2-b3-c4-have-at-least-one-pos

// gcc npc4eqa2pb3.c -o npc4eqa2pb3.bin -O3 -Wall -lm

#define U128 unsigned __int128


uint32_t isqrtu64(uint64_t n) {
  if (n < 2) return n;
  uint64_t ai = sqrt(n);
  while (!((ai <= n/ai) && ((ai+1) > n/(ai+1)))) {    
    ai = (ai + n/ai)/2;
  }
  return ai;
}

_Bool isa2plusb3x(uint64_t x) {
  // Return true if there exist positive integers a,b such that x = a^2 + b^3. False otherwise.
  for (uint32_t a = 1; (U128)a*a < x; a++) {
    for (uint32_t b = 1; ; b++) {
      U128 absum = (uint64_t)a*a + ((U128)b*b)*b;
      if (absum > x) break;
      if (absum == x) return true;
    }
  }
  return false;
}

_Bool isa2plusb3(uint64_t x, uint32_t *aext, uint32_t *bext) {
  // Return true if there exist positive integers a,b such that x = a^2 + b^3. False otherwise.
  for (uint32_t b = 1; ; b++) {
    U128 b3 = ((U128)b*b)*b;
    if (b3 >= x) return false;
    uint32_t a = isqrtu64(x - b3);
    if ((uint64_t)a*a + b3 == x) {
      *aext = a;
      *bext = b;
      return true;
    }
  }
  return false;
}

int main(int argc, char* argv[]) {
  if (argc < 3) {
    printf("This program prints integer solutions to the equation n + c^4 = a^2 + b^3.\nUsage: %s lookuptablebitsize maxexeptions\n", argv[0]);
    exit(0);
  }
  uint64_t asqrdplusbcubedmax = atol(argv[1]);
  uint32_t exceptioncountmax = atoi(argv[2]);
  uint32_t exceptioncount = 0;
  uint64_t numbytesret;
  uint8_t *abbitarray = makebitarray(1+asqrdplusbcubedmax, &numbytesret);
  for (uint32_t a = 1; (uint64_t)a*a < asqrdplusbcubedmax; a++) {
    for (uint32_t b = 1; ; b++) {
      uint64_t absum = (uint64_t)a*a + ((uint64_t)b*b)*b;
      if (absum > asqrdplusbcubedmax) break;
      setbitarray(absum, abbitarray);
    }
  }
  printf("Look-up table completed.\n");
  for (uint64_t n = 1; n < 0xffffffffffffffffULL; n++) {
    _Bool solutionfound = false;
    for (uint32_t c = 1; c <= 0xffff; c++) {
      uint32_t a,b;
      uint64_t c4 = (((uint64_t)c*c)*c)*c;
      uint64_t nplusc4 = n + c4;
      assert(nplusc4 > n);
      if (nplusc4 > asqrdplusbcubedmax) {
        if (isa2plusb3(nplusc4, &a, &b)) {
          printf("%lu + (%u)^4 = (%u)^2 + (%u)^3\n", n, c, a, b);
          solutionfound = true;
          break;
        }
      } else {
        if (getbitarray(nplusc4, abbitarray)) {
          isa2plusb3(nplusc4, &a, &b);
          printf("%lu + (%u)^4 = (%u)^2 + (%u)^3\n", n, c, a, b);
          solutionfound = true;
          break;
        }
      }
    }
    if (!solutionfound) {
      printf("No solution for n=%lu with a^2 + b^3 < 2^64.\n", n);
      exceptioncount++;
      if (exceptioncount >= exceptioncountmax) break;
    }
  }
  free(abbitarray);
}
