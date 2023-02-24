#include<simple.hh>
#include<mxv.hh>
#include<stdio.h>
#include<iostream>
#include<vector>
using namespace std;


double* div_mxv(int m_size, int n_size, int ori_m, int ori_n, int NWC[2], int SEC[2], int Y, int X, int A){
	double* y = new double[m_size];
	if(m_size <= 17 && n_size <= 17){
		simple::GPR[3] = Y;
		simple::GPR[4] = A + NWC[1] * ori_n * sizeof(double) + NWC[0] * sizeof(double);
		simple::GPR[5] = X + NWC[0] * sizeof(double);
		simple::GPR[6] = m_size;
		simple::GPR[7] = n_size;
		simple::GPR[10] = (ori_n - n_size)*sizeof(double);
		simple::GPR[11] = ori_m * sizeof(double);
		//cout << Y << "," << A << "," << simple::GPR[4] << "," << X << "," << simple::GPR[5] << "," << simple::GPR[10] << endl;

		simple::mxv(0,0,0,0,0);
		for(int i=0; i<m_size; i++){
			y[i]=*((double*)(simple::MEM + Y + i*sizeof(double)));
			//cout << "y[" << i << "]=" << y[i] << endl;
		}
		return y;
	}
	else{
		if(m_size>=n_size){
			double* y1;
			double* y2;
			int next_NWC_2[2] = {NWC[0],NWC[1]+(SEC[1]-NWC[1])/2};
			int next_SEC_1[2] = {SEC[0],NWC[1]+(SEC[1]-NWC[1])/2};
			y1 = div_mxv(m_size/2,n_size,ori_m,ori_n,NWC,next_SEC_1,Y,X,A);
            //cout << "NWC1= " << NWC[0] << "," << NWC[1] << endl;
			y2 = div_mxv(m_size-m_size/2,n_size,ori_m,ori_n,next_NWC_2,SEC,Y,X,A);
			//cout << "NWC2= " << next_NWC_2[0] << "," << next_NWC_2[1] << endl;            
			// for(int i = 0 ; i<m_size/2; i++)
			// 	cout << "y1[" << i << "]=" << y1[i] << endl;
			// for(int i = 0 ; i<m_size-m_size/2; i++)
			// 	cout << "y2[" << i << "]=" << y2[i] << endl;
			for(int i = 0; i<m_size;i++){
				if(i<m_size/2)
					y[i] = y1[i];
				else
					y[i] = y2[i-m_size/2];
			}
			return y;
		}
		else{
			double* y1;
			double* y2;
			int next_NWC_2[2] = {NWC[0]+(SEC[0]-NWC[0])/2,NWC[1]};
			int next_SEC_1[2] = {NWC[0]+(SEC[0]-NWC[0])/2,SEC[1]};
			y1=div_mxv(m_size,n_size/2,ori_m,ori_n,NWC,next_SEC_1,Y,X,A);
            //cout << "NWC1= " << NWC[0] << "," << NWC[1] << endl;
			y2=div_mxv(m_size,n_size-n_size/2,ori_m,ori_n,next_NWC_2,SEC,Y,X,A);
            //cout << "NWC2= " << next_NWC_2[0] << "," << next_NWC_2[1] << endl; 
            // for(int i = 0 ; i<m_size; i++)
			// 	cout << "y1[" << i << "]=" << y1[i] << endl;
			// for(int i = 0 ; i<m_size; i++)
			// 	cout << "y2[" << i << "]=" << y2[i] << endl;    
			for(int i = 0; i<m_size;i++){
				y[i] = y1[i]+y2[i];
			}
			return y;
		}
	}
}

int main
(
    int		  argc,
    char	**argv
)
{
    printf("L1: %u bytes of capacity, %u sets, %u-way set associative, %u-byte line size\n", 
	   simple::caches::L1.capacity(), simple::caches::L1.nsets(), simple::caches::L1.nways(), simple::caches::L1.linesize());

	simple::zeromem();
	uint32_t ori_m = 48;
	uint32_t ori_n = 37;
	uint32_t m = ori_m;
	uint32_t n = ori_n;

	const uint32_t M = m;
	const uint32_t N = n;

	//cout << m_block_size << "," << m_blocks << "," << n_block_size << "," << n_blocks << endl;
	const uint32_t Y = 0;
	const uint32_t X = Y + M*sizeof(double);
	const uint32_t A = X + N*sizeof(double);

	for (uint32_t i=0; i<M; i++) *((double*)(simple::MEM + Y + i*sizeof(double))) = 0.0;
	for (uint32_t j=0; j<N; j++) *((double*)(simple::MEM + X + j*sizeof(double))) = (double)j;
	for (uint32_t i=0; i<M; i++) for (uint32_t j=0; j<N; j++) *((double*)(simple::MEM + A + (i*N+j)*sizeof(double))) = (double)i;

	simple::zeroctrs();
	simple::caches::L1.clear();
	double* result;
	int a[2]={0,0};
	int b[2]={N,M};

	result = div_mxv(M,N,M,N,a,b,Y,X,A);
	for (uint32_t i=0; i<M; i++) *((double*)(simple::MEM + Y + i*sizeof(double))) = result[i];

	//uint32_t compulsory_miss = M + N + M * N;
	//bool is_opt = false;
	//if(compulsory_miss == simple::counters::L1::misses)
	//	is_opt = true;
	
	//printf("M = %6d, N = %6d : instructions = %6lu, cycles = %8lu, L1 accesses= %6lu, L1 misses = %6lu, L1 is optimal = %d :",
	//	M, N, simple::instructions, simple::cycles, simple::counters::L1::accesses, simple::counters::L1::misses, is_opt);
	bool pass = true;
	for (uint32_t i=0; i<M; i++)
	{
	    double y = *((double*)(simple::MEM + Y + i*sizeof(double)));
	    // printf("\ny[%2d] = %10.0f", i, y);
	    if (y != ((N*(N-1))/2)*i) { pass = false; }
	}
	// printf("\n");
	if (pass) printf("PASS\n");
	else      printf("FAIL\n");
    
    
    return 0;
}
