#include<simple.hh>
#include<memcpy.hh>
#include<stdio.h>

namespace simple
{
    void *memcpy                // GPR[3]
    (
        void            *dest,  // GPR[3]
        const void      *src,   // GPR[4]
        size_t           n      // GPR[5]
    )
    {
        addi(r7, r3, 0);        // preserve GPR[3] so that we can just return it
loop:   cmpi(r5, 0);            // n == 0?
        //printf("cmp_status0=%d\n",flags.EQ);
        beq(end);               // while(n != 0)
        cmpi(r5, 1);						// n == 1?
        //printf("cmp_status1=%d\n",flags.EQ);
        beq(last);							// if(n == 1)
        lhz(r6, r4);            // load byte from *src
        sth(r6, r7);            // store byte to *dest
        //printf("srcmem[%d]=%d \n", GPR[r4], GPR[r6]);
        //printf("destmem[%d]=%d \n", GPR[r7], MEM[GPR[r7]]);
        addi(r4, r4, 2);        // src++
        addi(r7, r7, 2);        // dest++
        addi(r5, r5, -2);       // n--
        b(loop);                // end while
last:		lbz(r6, r4);						
      	stb(r6, r7);
      	addi(r4, r4, 1);
      	addi(r7, r7, 1);
      	addi(r5, r5, -1);
end:    return dest;
    }
};