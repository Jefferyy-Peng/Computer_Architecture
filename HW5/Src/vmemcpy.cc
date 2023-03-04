#include<pipelined.hh>
#include<ISA.hh>
#include<vmemcpy.hh>

namespace pipelined
{
    void *vmemcpy               // GPR[3]
    (
        void            *dest,  // GPR[3]
        const void      *src,   // GPR[4]
        size_t           n      // GPR[5]
    )
    {
        addi(r7, r3, 0);        // preserve GPR[3] so that we can just return it
loop:	cmp(r5,r8);		// n == 16?
    // std::cout << "test1" << std::endl;
	blt(remain);		// while(n >= 16)
	vlb(v0, r4);		// load vector of bytes from *src
	vstb(v0, r7);		// store vector of bytes to *dest
	add(r4, r4, r8);	// src += 16
	add(r7, r7, r8);	// dest += 16
	sub(r5, r5, r8);	// n -= 16
	b(loop);		// end while
remain: cmpi(r5, 0);            // n == 0?
        beq(end);               // while(n != 0)
        lbz(r6, r4);            // load byte from *src
        stb(r6, r7);            // store byte to *dest
        addi(r4, r4, 1);        // src++
        addi(r7, r7, 1);        // dest++
        addi(r5, r5, -1);       // n--
        b(remain);              // end while
end:    return dest;
    }
};
