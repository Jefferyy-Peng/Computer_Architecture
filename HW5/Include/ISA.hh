#ifndef _ISA_HH_
#define _ISA_HH_

#include<pipelined.hh>

// 1. Branch Facility
#define b(X)			if (instructions::b::execute(0, #X, __LINE__)) goto X;
#define beq(X)			if (instructions::beq::execute(0, #X, __LINE__)) goto X;
#define bne(X)			if (instructions::bne::execute(0, #X, __LINE__)) goto X;
#define blt(X)			if (instructions::blt::execute(0, #X, __LINE__)) goto X;

// 2. Fixed-point Facility

// 2.1. Load/Store instructions
#define lbz(RT, RA)		instructions::lbz::execute(RT, RA, __LINE__)
#define stb(RS, RA)		instructions::stb::execute(RS, RA, __LINE__)

// 2.2. Arithmetic instructions
#define addi(RT, RA, SI)	instructions::addi::execute(RT, RA, SI, __LINE__)
#define add(RT, RA, RB)	instructions::add::execute(RT, RA, RB, __LINE__)
#define sub(RT, RA, RB)	instructions::sub::execute(RT, RA, RB, __LINE__)

// 2.3. Compare instructions
#define cmpi(RA, SI)		instructions::cmpi::execute(RA, SI, __LINE__)
#define cmp(RA, RB)		instructions::cmp::execute(RA, RB, __LINE__)

// 3. Floating-point Facility

// 3.1 Load/Store instructions
#define lfd(FT, RA)		instructions::lfd::execute(FT, RA, __LINE__)
#define stfd(FS, RA)		instructions::stfd::execute(FS, RA, __LINE__)

// 3.2. Arithmetic instructions
#define zd(FT)			instructions::zd::execute(FT, __LINE__)
#define fmul(FT, FA, FB)	instructions::fmul::execute(FT, FA, FB, __LINE__)
#define fadd(FT, FA, FB)	instructions::fadd::execute(FT, FA, FB, __LINE__)

// 4. Vector Facility

// 4.1 Load/Store instructions
#define vlb(VT, RA)		instructions::vlb  ::execute(VT, RA, __LINE__)
#define vstb(VS, RA)		instructions::vstb ::execute(VS, RA, __LINE__)
#define vlfd(VT, RA)		instructions::vlfd ::execute(VT, RA, __LINE__)
#define vstfd(VS, RA)		instructions::vstfd::execute(VS, RA, __LINE__)

// 4.2. Arithmetic instructions
#define vfmuldp(VT, VA, VB)	instructions::vfmul::execute(VT, VA, VB, __LINE__)
#define vfadddp(VT, VA, VB)	instructions::vfadd::execute(VT, VA, VB, __LINE__)

#endif
