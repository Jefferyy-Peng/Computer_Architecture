#include<pipelined.hh>
#include<ISA.hh>
#include<mxv.hh>

namespace pipelined
{
    void mxv
    (
        double		*y,	// GPR[3]
	double		*a,	// GPR[4]
	double		*x,	// GPR[5]
	uint32_t	 m,	// GPR[6]
	uint32_t	 n	// GPR[7]
    )
    {

loopi:
	cmpi(r6, 0);
	beq(end);
	addi(r8, r5, 0);
	addi(r9, r7, 0);
	zd(f0);
loopj:  cmpi(r9,0);
	beq(nexti);
	lfd(f1, r8);
	lfd(f2, r4);
	fmul(f3, f2, f1);
	fadd(f0, f0, f3);
	addi(r8, r8, 8);
	addi(r4, r4, 8);
	addi(r9, r9, -1);
	b(loopj);
nexti:  stfd(f0, r3);
	addi(r3, r3, 8);
	addi(r6, r6, -1);
	b(loopi);
end:    
	return;
    }
};
