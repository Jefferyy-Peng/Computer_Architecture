//modify the simple.cc in Src folder, define latency,nsets,nways,linesize for L2. Modify the cache::hit() function to make it more compatible by instead of using counters, I defined hits, misses and accesses variables within cache class.
#include<stdlib.h>
#include<stdint.h>
#include<assert.h>
#include<simple.hh>

namespace simple
{
    const uint32_t 	N = 65536;			// 64 KiB memory
    uint8_t     	MEM[N];     			// memory is an array of N bytes
    uint32_t   	 	GPR[16];     			// 16 x 32-bit general purpose registers
    double              FPR[8];         		// 8 x 64-bit floating-point registers

    const uint32_t	latencies::MEM = 300;		// main memory latency
    const uint32_t	latencies::L1  =   2;		// L1 cache latency
	const uint32_t	latencies::L2  =   4;		// L1 cache latency
	const uint32_t	latencies::L3  =   4;		// L1 cache latency
    const uint32_t	params::L1::nsets = 16;		// L1 number of sets
    const uint32_t	params::L1::nways = 4;		// L1 number of ways
    const uint32_t	params::L1::linesize = 8;	// L1 line size (bytes)
	const uint32_t	params::L2::nsets = 64;		// L1 number of sets
    const uint32_t	params::L2::nways = 4;		// L1 number of ways
    const uint32_t	params::L2::linesize = 8;	// L1 line size (bytes)
	const uint32_t	params::L3::nsets = 64;		// L1 number of sets
    const uint32_t	params::L3::nways = 4;		// L1 number of ways
    const uint32_t	params::L3::linesize = 8;	// L1 line size (bytes)

    namespace caches
    {
        cache	L1(params::L1::nsets, params::L1::nways, params::L1::linesize);
		cache	L2(params::L2::nsets, params::L2::nways, params::L2::linesize);
		cache	L3(params::L2::nsets, params::L2::nways, params::L2::linesize);

	cache::cache(uint32_t nsets, uint32_t nways, uint32_t linesize) : _sets(nsets)
	{
	    _nsets = nsets;
	    _nways = nways;
	    _linesize = linesize;

	    entry empty; empty.valid = false; empty.touched = 0;
	    set   init(nways); for (uint32_t i=0; i<nways; i++) init[i] = empty;
	    for (uint32_t i=0; i<nsets; i++) _sets[i] = init;
	}

	void cache::clear()
	{
	    for (uint32_t setix=0; setix<nsets(); setix++)
		for (uint32_t wayix=0; wayix<nways(); wayix++)
		{
		    sets()[setix][wayix].valid = false;
		    sets()[setix][wayix].touched = 0;
		    sets()[setix][wayix].addr = 0;
		}
		hits = 0;
		misses = 0;
		accesses = 0;
	}

	uint32_t cache::linesize() const
	{
	    return _linesize;
	}

	uint32_t cache::nsets() const
	{
	    return _nsets;
	}

	uint32_t cache::nways() const
	{
	    return _nways;
	}

	uint32_t cache::capacity() const
	{
	    return _nsets * _nways * _linesize;
	}

	array &cache::sets()
	{
	    return _sets;
	}

	void cache::invalid(uint32_t addr){
		uint32_t lineaddr = addr / linesize();
	    uint32_t setix = lineaddr % nsets();
	    uint32_t wayix;
		for (wayix = 0; wayix < nways(); wayix++)
	    {
			if (sets()[setix][wayix].valid && (sets()[setix][wayix].addr == lineaddr)) break;
	    }
		sets()[setix][wayix].valid = false;
	}

	void cache::receive_ev(uint32_t addr){
		uint32_t lineaddr = addr / linesize();
	    uint32_t setix = lineaddr % nsets();
	    uint32_t wayix;
		uint64_t lasttouch = cycles;
		uint32_t lru = nways();
		bool fall = true;
		for (wayix = 0; wayix < nways(); wayix++)
		{
		    if (!sets()[setix][wayix].valid)
		    {
			// invalid entry, can use this one as the lru
			lru = wayix;
			full = false;
			break;
		    }
		    if (sets()[setix][wayix].touched <= lasttouch)
		    {
			// older than current candidate - update
			lru = wayix;
			lasttouch = sets()[setix][wayix].touched;
		    }
		}
		assert(lru < nways());
		sets()[setix][lru].valid = true;
		sets()[setix][lru].addr = caches::L2.ev_addr;
		sets()[setix][lru].touched = caches::L2.ev_cycles;
	}

	bool cache::hit(uint32_t addr)
	{
	    accesses++;
	    uint32_t lineaddr = addr / linesize();
	    uint32_t setix = lineaddr % nsets();
	    uint32_t wayix;
	    for (wayix = 0; wayix < nways(); wayix++)
	    {
		if (sets()[setix][wayix].valid && (sets()[setix][wayix].addr == lineaddr)) break;
	    }
	    if      (wayix < nways())
	    {
		// L1 cache hit
		hits++;
		sets()[setix][wayix].touched = cycles;
		return true;
	    }
	    else
	    {
		// L1 cache miss
		misses++;
		// find the LRU entry
		full = true;
		uint64_t lasttouch = cycles;
		uint32_t lru = 0;
		for (wayix = 0; wayix < nways(); wayix++)
		{
		    if (!sets()[setix][wayix].valid)
		    {
			// invalid entry, can use this one as the lru
			lru = wayix;
			full = false;
			break;
		    }
		    if (sets()[setix][wayix].touched <= lasttouch)
		    {
			// older than current candidate - update
			lru = wayix;
			lasttouch = sets()[setix][wayix].touched;
		    }
		}
		assert(lru < nways());
		ev_addr = sets()[setix][lru].addr;
		ev_cycles = sets()[setix][lru].touched;
		sets()[setix][lru].valid = true;
		sets()[setix][lru].addr = lineaddr;
		sets()[setix][lru].touched = cycles;
		return false;
	    }
	}
    };

    flags_t	flags;				// flags

    uint32_t    CIA; 				// current instruction address
    uint32_t    NIA;    			// next instruction address

    uint64_t	instructions = 0;		// instruction counter
    uint64_t	cycles = 0;			// cycle counter
    // uint64_t	counters::L1::hits = 0;		// L1 hits
    // uint64_t	counters::L1::misses = 0;	// L1 misses
    // uint64_t	counters::L1::accesses = 0;	// L1 accesses
	// uint64_t	counters::L2::hits = 0;		// L1 hits
    // uint64_t	counters::L2::misses = 0;	// L1 misses
    // uint64_t	counters::L2::accesses = 0;	// L1 accesses

    void zeromem()
    {
	for (uint32_t i=0; i<N; i++) MEM[i] = 0;
    }

    void zeroctrs()
    {
	instructions = 0;
	cycles = 0;
	// counters::L1::accesses = 0;
	// counters::L1::hits = 0;
	// counters::L1::misses = 0;
    }

    void lbz(int RT, int RA)                	// load byte and zero-extend into a register
    {
	uint32_t EA = GPR[RA];
	GPR[RT] = MEM[EA];

	instructions++;
	if (caches::L1.hit(EA)) cycles += latencies::L1;
	else if(caches::L2.hit(EA)) cycles += latencies::L2;
	else if(caches::L3.hit(EA)){
		caches::L3.invalid(EA);
		cycles += latencies::L3;
	}
	else                    cycles += latencies::MEM;
    }

    void lfd(int FT, int RA)			// load double-precision number into floating-point register
    {
	uint32_t EA = GPR[RA];
	FPR[FT] = *((double*)(MEM + EA));

        instructions++;
	if (caches::L1.hit(EA)) cycles += latencies::L1;
	else if(caches::L2.hit(EA)) cycles += latencies::L2;
	else if(caches::L3.hit(EA)){
		caches::L3.invalid(EA);
		cycles += latencies::L3;
	}
	else                    cycles += latencies::MEM;
    }

    void stb(int RS, int RA)                	// store byte from register
    {
	uint32_t EA = GPR[RA];
	MEM[EA] = GPR[RS] & 0xff;

	instructions++;
	bool L1_hit = caches::L1.hit(EA);
	bool L2_hit = caches::L2.hit(EA);//write_through, should access L2 when L1_hit
	bool L3_hit = caches::L3.hit(EA);
	if (L1_hit) cycles += latencies::L2;
	else if(L2_hit) cycles += latencies::L2;
	else{           
    	if(caches::L2.full){
			caches::L3.receive_ev(EA);
			cycles += latencies::MEM;
		}
		else cycles += latencies::L2;
      }
    }

    void stfd(int FS, int RA)			// store double-precision number from floating-point register
    {
	uint32_t EA = GPR[RA];
	*((double*)(MEM + EA)) = FPR[FS];

	instructions++;
	bool L1_hit = caches::L1.hit(EA);
	bool L2_hit = caches::L2.hit(EA);
	bool L3_hit = caches::L3.hit(EA);
	if (L1_hit) cycles += latencies::L2;
	else if(L2_hit) cycles += latencies::L2;
	else{           
    	if(caches::L2.full){
			caches::L3.receive_ev(EA);
			cycles += latencies::MEM;
		}
		else cycles += latencies::L2;
      }
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

	void add(int RT, int RA, int RS)   	// add the contents of a register to a signed integer
    {
	GPR[RT] = GPR[RA] + GPR[RS];

	instructions++;
	cycles++;
    }

    void fmul(int FT, int FA, int FB)		// multiply two double-precision numbers
    {
	FPR[FT] = FPR[FA] * FPR[FB];

	instructions++;
	cycles++;
    }

    void fadd(int FT, int FA, int FB)		// add two double-precision numbers
    {
	FPR[FT] = FPR[FA] + FPR[FB];

	instructions++;
	cycles++;
    }

    void zd(int FT)				// zero the contents of a floating-point register
    {
	FPR[FT] = 0.0;

	instructions++;
	cycles++;
    }

#undef beq
    bool beq(int16_t BD)                    	// branch if comparison resuts was "equal"
    {
	instructions++;
	cycles++;

	if (flags.EQ) { NIA = CIA + BD; return true; }
	return false;
    }

#undef b
    bool b(int16_t BD)                      	// unconditional branch
    {
	instructions++;
	cycles++;

	NIA = CIA + BD;
	return true;
    }
};