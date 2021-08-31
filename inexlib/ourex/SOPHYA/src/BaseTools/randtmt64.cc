//-----------------------------------------------------------
// Classes TinyMT64RandGen
// Generateur aleatoire utilisant le code TinyMT 64 bits 
// SOPHYA class library - (C)  UPS+LAL IN2P3/CNRS  , CEA-Irfu 
// R. Ansari          (C) UPS+LAL IN2P3/CNRS  2009
// C. Magneville      (C) DAPNIA/SPP  CEA     2009
//-----------------------------------------------------------

#include <math.h> 
#include <stdlib.h>

#include "randtmt64.h"

// pas definit en C++ (seulement en C)
#ifndef UINT64_C
#  define UINT64_C(v) (v ## ULL) 
#endif
#include "tinymt64.h"

using namespace std;

namespace SOPHYA {

//----------------------------------------------------------------
// Implementation d'un generateur aleatoire utilisant le code 
// Tiny Mersenne Twister (TinyMT).
// http://www.math.sci.hiroshima-u.ac.jp/~m-mat/MT/emt.html


TinyMT64RandGen::TinyMT64RandGen(uint_8 seed)
{
  // On est oblige de faire new car la structure tinymt64_t pas defini ds randtmt64.h 
  tinymt64_ptr_ = new tinymt64_t;
  tinymt64_init(tinymt64_ptr_,seed);
}

TinyMT64RandGen::~TinyMT64RandGen()
{
// Ne pas oublier de faire le delete 
  delete tinymt64_ptr_;
}

void TinyMT64RandGen::ShowRandom()
{
  cout<<"RandomGenerator is TinyMT64RandGen"<<endl;
}

void TinyMT64RandGen::SetSeed(uint_8 seed)
{
  tinymt64_init(tinymt64_ptr_,seed);
}

void TinyMT64RandGen::SetSeed(vector<uint_8> seed)
{
  if(seed.size()<=0) return;
  int key_length = (int)seed.size();
  uint_8* init_key = new uint_8[key_length];
  for(int i=0;i<key_length;i++) init_key[i] = seed[i];

  tinymt64_init_by_array(tinymt64_ptr_,(const uint64_t *)(init_key),key_length);

  delete [] init_key;
}

r_8 TinyMT64RandGen::Next()
{
  return tinymt64_generate_double(tinymt64_ptr_);
}

void TinyMT64RandGen::AutoInit(int lp)
// init automatique avec 2 mots de 64 bits
{
  vector<uint_2> seed;
  GenerateSeedVector(5,seed,lp);
  vector<uint_8> s;
  uint_8 s8;
  s8 = uint_8(seed[0]) | (uint_8(seed[1])<<16) | (uint_8(seed[2])<<32) | (uint_8(seed[3])<<48);
  s.push_back(s8);
  s8 = uint_8(seed[4]) | (uint_8(seed[5])<<16) | (uint_8(seed[6])<<32) | (uint_8(seed[7])<<48);
  s.push_back(s8);
  SetSeed(s);
}

//----------------------------------------------------------
// Classe pour la gestion de persistance
// ObjFileIO<TinyMT64RandGen>
//----------------------------------------------------------

/* --Methode-- */
DECL_TEMP_SPEC  /* equivalent a template <> , pour SGI-CC en particulier */
void ObjFileIO<TinyMT64RandGen>::WriteSelf(POutPersist& s) const
{
  if (dobj == NULL)
    throw NullPtrError("ObjFileIO<TinyMT64RandGen>::WriteSelf() dobj=NULL");
  for(int i=0;i<2;i++)s.Put((uint_8)dobj->tinymt64_ptr_->status[i]);
  s.Put((uint_4)dobj->tinymt64_ptr_->mat1);
  s.Put((uint_4)dobj->tinymt64_ptr_->mat2);
  s.Put((uint_8)dobj->tinymt64_ptr_->tmat);
  return;
}

/* --Methode-- */
DECL_TEMP_SPEC  /* equivalent a template <> , pour SGI-CC en particulier */
void ObjFileIO<TinyMT64RandGen>::ReadSelf(PInPersist& s)
{
  if(dobj == NULL) dobj = new TinyMT64RandGen();
  uint_4 v4; uint_8 v8;
  for(int i=0;i<2;i++) {
    s.Get(v8);
    dobj->tinymt64_ptr_->status[i] = v8;
  }
  s.Get(v4); dobj->tinymt64_ptr_->mat1 = v4;
  s.Get(v4); dobj->tinymt64_ptr_->mat2 = v4;
  s.Get(v8); dobj->tinymt64_ptr_->tmat = v8;
  return;
}

// ---------------------------------------------------------
#ifdef __CXX_PRAGMA_TEMPLATES__
#pragma define_template ObjFileIO<TinyMT64RandGen>
#endif

#if defined(ANSI_TEMPLATES) || defined(GNU_TEMPLATES)
template class ObjFileIO<TinyMT64RandGen>;
#endif

}  /* namespace SOPHYA */
