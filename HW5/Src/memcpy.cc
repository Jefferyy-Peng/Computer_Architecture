#include<pipelined.hh>
#include<ISA.hh>
#include<memcpy.hh>

namespace pipelined
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
        beq(end);               // while(n != 0)
        lbz(r6, r4);            // load byte from *src
        stb(r6, r7);            // store byte to *dest
        addi(r4, r4, 1);        // src++
        addi(r7, r7, 1);        // dest++
        addi(r5, r5, -1);       // n--
        b(loop);                // end while
end:    return dest;
    }
};
