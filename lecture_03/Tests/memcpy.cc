#include<simple.hh>
#include<memcpy.hh>
#include<stdio.h>

int main
(
    int		  argc,
    char	**argv
)
{
    printf("L1: %u bytes of capacity, %u sets, %u-way set associative, %u-byte line size\n", 
	   simple::caches::L1.capacity(), simple::caches::L1.nsets(), simple::caches::L1.nways(), simple::caches::L1.linesize());
    simple::zeromem();
    const uint32_t N = 1024;
    for (uint32_t i=0; i<N; i++) simple::MEM[i] = rand() % 0xff;
    for (uint32_t n = 1; n<=N; n *= 2)
    {
	simple::GPR[3] = 1024;
	simple::GPR[4] = 0;
	simple::GPR[5] = n;
	
	simple::zeroctrs();
	simple::caches::L1.clear();
	simple::memcpy(0,0,0);

	double rate = (double)simple::cycles/(double)n;
	
	printf("n = %6d : instructions = %6lu, cycles = %6lu, L1 accesses= %6lu, L1 hits = %6lu",
		n, simple::instructions, simple::cycles, simple::counters::L1::accesses, simple::counters::L1::hits);
	printf(", cyc/B = %10.2f", rate);

	bool pass = true;
	for (uint32_t i=0; i<n; i++) if (simple::MEM[i] != simple::MEM[N+i]) pass = false;
	if (pass) printf(" | PASS\n");
	else      printf(" | FAIL\n");
    }
    
    return 0;
}
