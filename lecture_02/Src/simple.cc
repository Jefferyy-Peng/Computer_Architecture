#include<stdlib.h>
#include<stdint.h>
#include<assert.h>
#include<simple.hh>

namespace simple
{
    const uint32_t 	N = 65536;			// 64 KiB memory
    uint8_t     	MEM[N];     			// memory is an array of N bytes
    uint32_t   	 	GPR[8];     			// 8 x 32-bit general purpose registers

    const uint32_t	latencies::MEM = 300;		// main memory latency
    const uint32_t	latencies::L1  =   2;		// L1 cache latency
    const uint32_t	params::L1::nsets = 32;		// L1 number of sets
    const uint32_t	params::L1::nways = 8;		// L1 number of ways
    const uint32_t	params::L1::linesize = 128;	// L1 line size (bytes)

    namespace caches
    {
        cache	L1(params::L1::nsets, params::L1::nways, params::L1::linesize);

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

	bool cache::hit(uint32_t addr)
	{
	    counters::L1::accesses++;
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
		counters::L1::hits++;
		sets()[setix][wayix].touched = cycles;
		return true;
	    }
	    else
	    {
		// L1 cache miss
		counters::L1::misses++;
		// find the LRU entry
		uint64_t lasttouch = cycles;
		uint32_t lru = nways();
		for (wayix = 0; wayix < nways(); wayix++)
		{
		    if (!sets()[setix][wayix].valid)
		    {
			// invalid entry, can use this one as the lru
			lru = wayix;
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
    uint64_t	counters::L1::hits = 0;		// L1 hits
    uint64_t	counters::L1::misses = 0;	// L1 misses
    uint64_t	counters::L1::accesses = 0;	// L1 accesses

    void zeromem()
    {
	for (uint32_t i=0; i<N; i++) MEM[i] = 0;
    }

    void zeroctrs()
    {
	instructions = 0;
	cycles = 0;
	counters::L1::accesses = 0;
	counters::L1::hits = 0;
	counters::L1::misses = 0;
    }

    void lbz(int RT, int RA)                	// load byte and zero-extend into a register
    {
	uint32_t EA = GPR[RA];
	GPR[RT] = MEM[EA];

	instructions++;
	if (caches::L1.hit(EA)) cycles += latencies::L1;
	else                    cycles += latencies::MEM;
    }

    void stb(int RS, int RA)                	// store byte from register
    {
	uint32_t EA = GPR[RA];
	MEM[EA] = GPR[RS] & 0xff;

	instructions++;
	if (caches::L1.hit(EA)) cycles += latencies::L1;
	else                    cycles += latencies::MEM;
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
