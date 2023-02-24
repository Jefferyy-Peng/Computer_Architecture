#include<stdlib.h>
#include<stdint.h>
#include<simple.hh>

namespace simple
{
    const uint32_t 	N = 65536;		// 64 KiB memory
    uint8_t     	MEM[N];     		// memory is an array of N bytes
    uint32_t   	 	GPR[8];     		// 8 x 32-bit general purpose registers

    const uint32_t	latencies::MEM = 300;	// main memory latency

    flags_t	flags;				// flags

    uint32_t    CIA; 				// current instruction address
    uint32_t    NIA;    			// next instruction address

    uint64_t	instructions = 0;		// instruction counter
    uint64_t	cycles = 0;			// cycle counter

    void zeromem()
    {
      for (uint32_t i=0; i<N; i++) MEM[i] = 0;
    }

    void zeroctrs()
    {
			instructions = 0;
			cycles = 0;
    }

    void lbz(int RT, int RA)                	// load byte and zero-extend into a register
    {
			uint32_t EA = GPR[RA];
			GPR[RT] = MEM[EA];

			instructions++;
			cycles += latencies::MEM;
    }
  
  void lhz(int RT, int RA)                	// load byte and zero-extend into a register
    {
			uint32_t EA = GPR[RA];
			GPR[RT] = MEM[EA] + (MEM[EA+1]<<8);

			instructions++;
			cycles += latencies::MEM;
    }

    void stb(int RS, int RA)                	// store byte from register
    {
			uint32_t EA = GPR[RA];
			MEM[EA] = GPR[RS] & 0xff;

			instructions++;
			cycles += latencies::MEM;
    }
  
    void sth(int RS, int RA)                	// load byte and zero-extend into a register
    {
			uint32_t EA = GPR[RA];
			MEM[EA] = GPR[RS] & 0xff;
  		    MEM[EA+1] = (GPR[RS] & 0xff00) >> 8;

			instructions++;
			cycles += latencies::MEM;
    }

    void cmpi(int RA, int16_t SI)           	// compare the contents of a register with a signed integer
    {
	flags.LT = false; flags.GT = false; flags.EQ = false;
	if      (GPR[RA] < SI) flags.LT = true;
	else if (GPR[RA] > SI) flags.GT = true;
	else   		       flags.EQ = true;

	instructions++;
	cycles++;
    }

    void addi(int RT, int RA, int16_t SI)   	// add the contents of a register to a signed integer
    {
	GPR[RT] = GPR[RA] + SI;

	instructions++;
	cycles++;
    }

#undef beq
    bool beq(int16_t BD)                    	// branch if comparison resuts was "equal"
    {
	if (flags.EQ) { NIA = CIA + BD; return true; }
	return false;
    }

#undef b
    bool b(int16_t BD)                      	// unconditional branch
    {
	NIA = CIA + BD;
	return true;
    }
};