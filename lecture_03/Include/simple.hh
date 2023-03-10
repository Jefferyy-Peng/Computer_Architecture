#ifndef _SIMPLE_HH_
#define _SIMPLE_HH_

#include<stdlib.h>
#include<stdint.h>
#include<vector>

namespace simple
{
    extern const uint32_t 	N;	
    extern uint8_t     		MEM[];  	// memory is an array of N bytes
    extern uint32_t   	 	GPR[16];     	// 16 x 32-bit general purpose registers
    extern double		FPR[8];		// 8 x 64-bit floating-point registers

    namespace latencies
    {
	extern const uint32_t MEM;
	extern const uint32_t L1;
	extern const uint32_t L2;
	extern const uint32_t L3;
    };

    namespace params
    {
	namespace L1
	{
	    extern const uint32_t nsets;
	    extern const uint32_t nways;
	    extern const uint32_t linesize;
	};
	namespace L2
	{
	    extern const uint32_t nsets;
	    extern const uint32_t nways;
	    extern const uint32_t linesize;
	};
	namespace L3
	{
	    extern const uint32_t nsets;
	    extern const uint32_t nways;
	    extern const uint32_t linesize;
	};
    };

    namespace counters
    {
	namespace L1
	{
	    extern uint64_t hits;
	    extern uint64_t misses;
	    extern uint64_t accesses;
	};
	namespace L2
	{
	    extern uint64_t hits;
	    extern uint64_t misses;
	    extern uint64_t accesses;
	};
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
	r7 = 7,
	r8 = 8,
	r9 = 9,
	r10 = 10,
	r11 = 11
    } gprnum;					// valid GPR numbers

    typedef enum
    {
	f0 = 0,
	f1 = 1,
	f2 = 2,
	f3 = 3,
	f4 = 4,
	f5 = 5,
	f6 = 6,
	f7 = 7
    } fprnum;					// valid FPR numbers

    typedef struct
    {
	bool    LT;     			// less than
	bool    GT;     			// greater than
	bool    EQ;     			// equal to
    } flags_t;					// flags

    extern flags_t	flags;

    namespace caches
    {
	class entry				// cache entry
	{
	    public:
		bool		valid;		// is entry valid?
		uint32_t	addr;		// address of this entry
		uint64_t	touched;	// last time this entry was used
	};

	typedef std::vector<entry>	set;
	typedef std::vector<set>	array;

	class cache
	{
	    private:
		uint32_t	_nsets;
		uint32_t	_nways;
		uint32_t	_linesize;
		array		_sets;

	    public:
		cache(uint32_t nsets, uint32_t nways, uint32_t linesize);

		uint32_t	nsets() const;		// number of sets
		uint32_t	nways() const;		// number of ways
		uint32_t	linesize() const;	// in bytes
		uint32_t	capacity() const;	// in bytes
		array&		sets();			// cache array
		uint64_t hits;
		uint64_t misses;
		uint64_t accesses;
		uint32_t ev_addr;
		uint32_t ev_cycles;
		bool full;
		bool	hit(uint32_t);		// tests for address hit in cache
		void		clear();		// clear the cache
		void 	invalid(uint32_t addr);
		void 	receive_ev(uint32_t addr);
	};

	extern cache L1;
	extern cache L2;
	extern cache L3;
    };

    extern uint32_t     CIA; 			// current instruction address
    extern uint32_t     NIA;   			// next instruction address

    extern uint64_t	instructions;		// instruction counter
    extern uint64_t	cycles;			// cycle counter

    void zeromem();
    void zeroctrs();

    void lbz(int RT, int RA);                	// load byte and zero-extend into a register
    void stb(int RS, int RA);                	// store byte from register
    void cmpi(int RA, int16_t SI);           	// compare the contents of a register with a signed integer
    void addi(int RT, int RA, int16_t SI);   	// add the contents of a register to a signed integer
	void add(int RT, int RA, int RS);
    bool beq(int16_t BD);                    	// branch if comparison resuts was "equal"
    bool b(int16_t BD);                      	// unconditional branch

    void lfd(int FT, int RA);			// load double-precision number into a floating-point register
    void stfd(int FS, int RA);			// store double-precision number from a floating-point register
    void zd(int FT);				// zero floating-point register
    void fadd(int FT, int FA, int FB);		// add two floating-point registers
    void fmul(int FT, int FA, int FB);		// multiply two floating-point registers
};

#define b(X)	if (b(0)) goto X
#define beq(X)	if (beq(0)) goto X

#endif
