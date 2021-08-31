//-----------------------------------------------------------
// Classe FMTRandGen
// Generateur aleatoire utilisant le code dSFMT 
// SOPHYA class library - (C)  UPS+LAL IN2P3/CNRS  , CEA-Irfu 
// R. Ansari          (C) UPS+LAL IN2P3/CNRS  2009
// C. Magneville      (C) DAPNIA/SPP  CEA     2009
//-----------------------------------------------------------

#include <math.h> 
#include <stdlib.h>

#include "randfmt.h"
#include "dsfmtflags.h"
#include "dSFMT.h"

using namespace std;

namespace SOPHYA {

//----------------------------------------------------------------
// Implementation d'un generateur aleatoire utilisant le code 
// SIMD-oriented Fast Mersenne Twister (SFMT).
// http://www.math.sci.hiroshima-u.ac.jp/~m-mat/MT/emt.html


FMTRandGen::FMTRandGen(uint_4 seed)
{
  // On est oblige de faire new car la structure ds_fmt_t pas defini ds randfmt.h 
  dsfmt_ptr_ = new dsfmt_t;
  dsfmt_init_gen_rand(dsfmt_ptr_,seed);
}

FMTRandGen::~FMTRandGen()
{
// Ne pas oublier de faire le delete 
  delete dsfmt_ptr_;
}

void FMTRandGen::ShowRandom()
{
  cout<<"RandomGenerator is FMTRandGen"<<endl;
}

void FMTRandGen::SetSeed(uint_4 seed)
{
  dsfmt_init_gen_rand(dsfmt_ptr_,seed);
}

void FMTRandGen::SetSeed(vector<uint_4> seed)
{
  if(seed.size()<=0) return;
  int key_length = (int)seed.size();
  uint_4* init_key = new uint_4[key_length];
  for(int i=0;i<key_length;i++) init_key[i] = seed[i];

  dsfmt_init_by_array(dsfmt_ptr_,init_key,key_length);

  delete [] init_key;
}

r_8 FMTRandGen::Next()
{
  return dsfmt_genrand_close_open(dsfmt_ptr_);
}

void FMTRandGen::AutoInit(int lp)
{
  vector<uint_2> seed;
  GenerateSeedVector(1,seed,lp);
  vector<uint_4> s;
  uint_4 s4;
  s4 = seed[0] | (seed[1]<<16);
  s.push_back(s4);
  s4 = seed[2] | (seed[3]<<16);
  s.push_back(s4);
  SetSeed(s);
}

//----------------------------------------------------------
// Classe pour la gestion de persistance
// ObjFileIO<FMTRandGen>
//----------------------------------------------------------

/* --Methode-- */
DECL_TEMP_SPEC  /* equivalent a template <> , pour SGI-CC en particulier */
void ObjFileIO<FMTRandGen>::WriteSelf(POutPersist& s) const
{
  if (dobj == NULL)
    throw NullPtrError("ObjFileIO<FMTRandGen>::WriteSelf() dobj=NULL");
  s.Put((uint_4)DSFMT_MEXP);
  s.Put(dobj->dsfmt_ptr_->idx);
  for(int i=0;i<DSFMT_N+1;i++) {
    for(int j=0;j<2;j++) {
      uint_8 v = dobj->dsfmt_ptr_->status[i].u[j];
      s.PutU8(v);
    }
  }
  return;
}

/* --Methode-- */
DECL_TEMP_SPEC  /* equivalent a template <> , pour SGI-CC en particulier */
void ObjFileIO<FMTRandGen>::ReadSelf(PInPersist& s)
{
  uint_4 mexp;
  s.Get(mexp);
  if(mexp != DSFMT_MEXP)
    throw SzMismatchError("ObjFileIO<FMTRandGen>::WriteSelf() wrong DSFMT_MEXP");
  if(dobj == NULL) dobj = new FMTRandGen();
  s.Get(dobj->dsfmt_ptr_->idx);
  for(int i=0;i<DSFMT_N+1;i++) {
    for(int j=0;j<2;j++) {
      uint_8 v;
      s.Get(v);
      dobj->dsfmt_ptr_->status[i].u[j] = v;
    }
  }
  return;
}

// ---------------------------------------------------------
#ifdef __CXX_PRAGMA_TEMPLATES__
#pragma define_template ObjFileIO<FMTRandGen>
#endif

#if defined(ANSI_TEMPLATES) || defined(GNU_TEMPLATES)
template class ObjFileIO<FMTRandGen>;
#endif

}  /* namespace SOPHYA */
