#ifndef  DSFMTWRAP_H_SEEN 
#define  DSFMTWRAP_H_SEEN 

// Fichier include d'enrobage pour flags et definition du generateur 
// SIMD-oriented Fast Mersenne Twister (SFMT).
// http://www.math.sci.hiroshima-u.ac.jp/~m-mat/MT/emt.html

// Definir les flags en fonction des architectures ...
// Choix de la periode du generateur 
//#define  DSFMT_MEXP  521
#define  DSFMT_MEXP  19937

// Si on veut generer les instructions SSE2, : 
// Streaming SIMD Extensions 2, is one of the IA-32 SIMD (Single Instruction, Multiple Data) 
#if defined(MACH_INTEL) || defined(MACH_AMD) 
#define  HAVE_SSE2 1
#endif

#endif
