#ifndef _SIMPLE_HH_
#define _SIMPLE_HH_

#include<stdlib.h>
#include<stdint.h>

namespace simple
{
    extern const uint32_t 	N;	
    extern uint8_t     		MEM[];  	// memory is an array of N bytes
    extern uint32_t   	 	GPR[8];     	// 8 x 32-bit general purpose registers

    namespace latencies
    {
	extern const uint32_t MEM;
    };

    typedef enum
    {
	r0 = 0,
	r1 = 1,
	r2 = 2,
	r3 = 3,
	r4 = 4,
	r5 = 5,
	r6 = 6,
	r7 = 7
    } gprnum;					// valid GPR numbers

    typedef struct
    {
	bool    LT;     			// less than
	bool    GT;     			// greater than
	bool    EQ;     			// equal to
    } flags_t;					// flags

    extern flags_t	flags;

    extern uint32_t     CIA; 			// current instruction address
    extern uint32_t     NIA;   			// next instruction address

    extern uint64_t	instructions;		// instruction counter
    extern uint64_t	cycles;			// cycle counter

    void zeromem();
    void zeroctrs();

    void lbz(int RT, int RA);                	// load byte and zero-extend into a register
    void lhz(int RT, int RA);                	// load byte and zero-extend into a register
    void stb(int RS, int RA);                	// store byte from register
    void sth(int RS, int RA);                	// store byte from register
    void cmpi(int RA, int16_t SI);           	// compare the contents of a register with a signed integer
    void addi(int RT, int RA, int16_t SI);   	// add the contents of a register to a signed integer
    bool beq(int16_t BD);                    	// branch if comparison resuts was "equal"
    bool b(int16_t BD);                      	// unconditional branch
};

#define b(X)	if (b(0)) goto X
#define beq(X)	if (beq(0)) goto X

#endif
