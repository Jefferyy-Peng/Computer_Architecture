#ifndef _ISA_HH_
#define _ISA_HH_

#include<pipelined.hh>

// 1. Branch Facility
#define b(X)			if (instructions::b::execute(0, #X, __LINE__)) goto X;
#define beq(X)			if (instructions::beq::execute(0, #X, __LINE__)) goto X;
#define bne(X)			if (instructions::bne::execute(0, #X, __LINE__)) goto X;

// 2. Fixed-point Facility

// 2.1. Load/Store instructions
#define lbz(RT, RA)		instructions::lbz::execute(RT, RA, __LINE__)
#define stb(RS, RA)		instructions::stb::execute(RS, RA, __LINE__)

// 2.2. Arithmetic instructions
#define addi(RT, RA, SI)	instructions::addi::execute(RT, RA, SI, __LINE__)

// 2.3. Compare instructions
#define cmpi(RA, SI)		instructions::cmpi::execute(RA, SI, __LINE__)

// 3. Floating-point Facility

// 3.1 Load/Store instructions
#define lfd(FT, RA)		instructions::lfd::execute(FT, RA, __LINE__)
#define stfd(FS, RA)		instructions::stfd::execute(FS, RA, __LINE__)

// 3.2. Arithmetic instructions
#define zd(FT)			instructions::zd::execute(FT, __LINE__)
#define fmul(FT, FA, FB)	instructions::fmul::execute(FT, FA, FB, __LINE__)
#define fadd(FT, FA, FB)	instructions::fadd::execute(FT, FA, FB, __LINE__)

#endif
