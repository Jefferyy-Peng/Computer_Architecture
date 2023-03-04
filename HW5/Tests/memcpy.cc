#include<pipelined.hh>
#include<memcpy.hh>
#include<stdio.h>

int main
(
    int		  argc,
    char	**argv
)
{
    printf("L1D: %u bytes of capacity, %u sets, %u-way set associative, %u-byte line size\n",
	   pipelined::caches::L1D.capacity(), pipelined::caches::L1D.nsets(), pipelined::caches::L1D.nways(), pipelined::caches::L1D.linesize());
    printf("L1I: %u bytes of capacity, %u sets, %u-way set associative, %u-byte line size\n",
	   pipelined::caches::L1I.capacity(), pipelined::caches::L1I.nsets(), pipelined::caches::L1I.nways(), pipelined::caches::L1I.linesize());
    printf("L2: %u bytes of capacity, %u sets, %u-way set associative, %u-byte line size\n",
	   pipelined::caches::L2.capacity(), pipelined::caches::L2.nsets(), pipelined::caches::L2.nways(), pipelined::caches::L2.linesize());
    printf("L3: %u bytes of capacity, %u sets, %u-way set associative, %u-byte line size\n",
	   pipelined::caches::L3.capacity(), pipelined::caches::L3.nsets(), pipelined::caches::L3.nways(), pipelined::caches::L3.linesize());

    pipelined::zeromem();
    const uint32_t N = 1024;
    for (uint32_t i=0; i<N; i++) pipelined::MEM[i] = rand() % 0xff;
    for (uint32_t n = 1; n<=N; n *= 2)
    {
	pipelined::zeroctrs();

	pipelined::GPR[3].data() = 1024;
	pipelined::GPR[4].data() = 0;
	pipelined::GPR[5].data() = n;
	
	pipelined::memcpy(0,0,0);

	pipelined::caches::L2.flush();
	pipelined::caches::L3.flush();

	double rate = (double)pipelined::counters::cycles/(double)n;
	
	if (pipelined::tracing) printf("\n");
	printf("n = %6d : instructions = %6lu, cycles = %6lu, L1D accesses= %6lu, L1D hits = %6lu",
		n, pipelined::counters::operations, pipelined::counters::cycles, pipelined::caches::L1D.accesses, pipelined::caches::L1D.hits);
	printf(", cyc/B = %10.2f", rate);

	bool pass = true;
	for (uint32_t i=0; i<n; i++) if (pipelined::MEM[i] != pipelined::MEM[N+i]) pass = false;
	if (pass) printf(" | PASS\n");
	else      printf(" | FAIL\n");
    }
    
    return 0;
}
