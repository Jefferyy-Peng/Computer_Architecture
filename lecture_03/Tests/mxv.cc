#include<simple.hh>
#include<mxv.hh>
#include<stdio.h>

int main
(
    int		  argc,
    char	**argv
)
{
    printf("L1: %u bytes of capacity, %u sets, %u-way set associative, %u-byte line size\n", 
	   simple::caches::L1.capacity(), simple::caches::L1.nsets(), simple::caches::L1.nways(), simple::caches::L1.linesize());

	double max_flops = 0;
	int cri_m = 2;
	int cri_n = 2;
	int opt_m = 2;
	int opt_n = 2;

    for (uint32_t m = 2; m <= 70; m += 2) for (uint32_t n = m-1; n <= m+10; n += 2)
    {
	simple::zeromem();

	const uint32_t M = m;
	const uint32_t N = n;

	const uint32_t Y = 0;
	const uint32_t X = Y + M*sizeof(double);
	const uint32_t A = X + N*sizeof(double);

	for (uint32_t i=0; i<M; i++) *((double*)(simple::MEM + Y + i*sizeof(double))) = 0.0;
	for (uint32_t j=0; j<N; j++) *((double*)(simple::MEM + X + j*sizeof(double))) = (double)j;
	for (uint32_t i=0; i<M; i++) for (uint32_t j=0; j<N; j++) *((double*)(simple::MEM + A + (i*N+j)*sizeof(double))) = (double)i;

	simple::GPR[3] = Y;
	simple::GPR[4] = A;
	simple::GPR[5] = X;
	simple::GPR[6] = M;
	simple::GPR[7] = N;
	
	simple::zeroctrs();
	simple::caches::L1.clear();
	simple::caches::L2.clear();
	simple::mxv(0,0,0,0,0);
	
	double flops = (double)2*M*N/(double)simple::cycles;
	int comp_miss = M + N + M * N;
	if(simple::caches::L1.misses == comp_miss){
		opt_n = n;
		opt_m = m;
	}
	if(flops > max_flops){
		max_flops = flops;
		cri_m = m;
		cri_n = n;
	}

	printf("M = %6d, N = %6d : instructions = %6lu, cycles = %8lu, flops/cyc = %10.6f, max_flops = %10.6f, critical_m = %d, critical_n = %d, optimal_m = %d, optimal_n = %d\n L1 accesses= %6lu, L1 misses = %6lu, L2 accesses= %6lu, L2 misses = %6lu, L3 accesses= %6lu, L3 misses = %6lu :",
		M, N, simple::instructions, simple::cycles, flops, max_flops, cri_m, cri_n, opt_m, opt_n, simple::caches::L1.accesses, simple::caches::L1.misses, simple::caches::L2.accesses, simple::caches::L2.misses, simple::caches::L3.accesses, simple::caches::L3.misses);
	bool pass = true;
	for (uint32_t i=0; i<M; i++)
	{
	    double y = *((double*)(simple::MEM + Y + i*sizeof(double)));
	    //printf("\ny[%2d] = %10.0f", i, y);
	    if (y != ((N*(N-1))/2)*i) { pass = false; }
	}
	printf("\n");
	if (pass) printf("PASS\n");
	else      printf("FAIL\n");
    }
    
    return 0;
}
