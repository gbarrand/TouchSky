//-----------------------------------------------------------
// Classe TinyMT32RandGen
// Generateur aleatoire utilisant le code TinyMT 32 bits 
// SOPHYA class library - (C)  UPS+LAL IN2P3/CNRS  , CEA-Irfu 
// R. Ansari          (C) UPS+LAL IN2P3/CNRS  2009
// C. Magneville      (C) DAPNIA/SPP  CEA     2009
//-----------------------------------------------------------

#include <math.h> 
#include <stdlib.h>

#include "randtmt32.h"

// pas definit en C++ (seulement en C)
#ifndef UINT32_C
#  define UINT32_C(v) (v ## UL) 
#endif
#include "tinymt32.h"

using namespace std;

namespace SOPHYA {

//----------------------------------------------------------------
// Implementation d'un generateur aleatoire utilisant le code 
// Tiny Mersenne Twister (TinyMT).
// http://www.math.sci.hiroshima-u.ac.jp/~m-mat/MT/emt.html


TinyMT32RandGen::TinyMT32RandGen(uint_4 seed)
{
  // On est oblige de faire new car la structure tinymt32_t pas defini ds randtmt32.h 
  tinymt32_ptr_ = new tinymt32_t;
  tinymt32_init(tinymt32_ptr_,seed);
}

TinyMT32RandGen::~TinyMT32RandGen()
{
// Ne pas oublier de faire le delete 
  delete tinymt32_ptr_;
}

void TinyMT32RandGen::ShowRandom()
{
  cout<<"RandomGenerator is TinyMT32RandGen"<<endl;
}

void TinyMT32RandGen::SetSeed(uint_4 seed)
{
  tinymt32_init(tinymt32_ptr_,seed);
}

void TinyMT32RandGen::SetSeed(vector<uint_4> seed)
{
  if(seed.size()<=0) return;
  int key_length = (int)seed.size();
  uint_4* init_key = new uint_4[key_length];
  for(int i=0;i<key_length;i++) init_key[i] = seed[i];

  tinymt32_init_by_array(tinymt32_ptr_,init_key,key_length);

  delete [] init_key;
}

r_8 TinyMT32RandGen::Next()
{
  return (r_8)tinymt32_generate_float(tinymt32_ptr_);
}

void TinyMT32RandGen::AutoInit(int lp)
// init automatique avec 2 mots de 32 bits
{
  vector<uint_2> seed;
  GenerateSeedVector(1,seed,lp);
  vector<uint_4> s;
  uint_4 s4;
  s4 = uint_4(seed[0]) | (uint_4(seed[1])<<16);
  s.push_back(s4);
  s4 = uint_4(seed[2]) | (uint_4(seed[3])<<16);
  s.push_back(s4);
  SetSeed(s);
}

//----------------------------------------------------------
// Classe pour la gestion de persistance
// ObjFileIO<TinyMT32RandGen>
//----------------------------------------------------------

/* --Methode-- */
DECL_TEMP_SPEC  /* equivalent a template <> , pour SGI-CC en particulier */
void ObjFileIO<TinyMT32RandGen>::WriteSelf(POutPersist& s) const
{
  if (dobj == NULL)
    throw NullPtrError("ObjFileIO<TinyMT32RandGen>::WriteSelf() dobj=NULL");
  for(int i=0;i<4;i++)s.Put((uint_4)dobj->tinymt32_ptr_->status[i]);
  s.Put((uint_4)dobj->tinymt32_ptr_->mat1);
  s.Put((uint_4)dobj->tinymt32_ptr_->mat2);
  s.Put((uint_4)dobj->tinymt32_ptr_->tmat);
  return;
}

/* --Methode-- */
DECL_TEMP_SPEC  /* equivalent a template <> , pour SGI-CC en particulier */
void ObjFileIO<TinyMT32RandGen>::ReadSelf(PInPersist& s)
{
  if(dobj == NULL) dobj = new TinyMT32RandGen();
  uint_4 v;
  for(int i=0;i<4;i++) {
    s.Get(v);
    dobj->tinymt32_ptr_->status[i] = v;
  }
  s.Get(v); dobj->tinymt32_ptr_->mat1 = v;
  s.Get(v); dobj->tinymt32_ptr_->mat2 = v;
  s.Get(v); dobj->tinymt32_ptr_->tmat = v;
  return;
}

// ---------------------------------------------------------
#ifdef __CXX_PRAGMA_TEMPLATES__
#pragma define_template ObjFileIO<TinyMT32RandGen>
#endif

#if defined(ANSI_TEMPLATES) || defined(GNU_TEMPLATES)
template class ObjFileIO<TinyMT32RandGen>;
#endif

}  /* namespace SOPHYA */
