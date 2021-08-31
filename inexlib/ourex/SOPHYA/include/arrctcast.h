#ifndef ARRCTCAST_SEEN
#define ARRCTCAST_SEEN
//----------------------------------------------------
//    SOPHYA class library 
// (C) LAL-IN2P3/CNRS         (C) DAPNIA-CEA  
//             R. Ansari - Oct 2006
//----------------------------------------------------

#include "tarray.h"

namespace SOPHYA {

//-----------------------------------------------------------------------
// Classe de Bridge et fonctions utilitaires pour permettre des 
// cast de type de donnees - sans conversion - pour des tableaux (TArray)
//-----------------------------------------------------------------------

//******************************************
// Classe NDCBridge : Bridge sur NDataBlock, utile pour cast - sans conversion - de type 
/*! 
  \class NDCBridge
  \brief Special Bridge class to be used the ArrayCast class 
  \ingroup TArray 
*/
template <class T>
class NDCBridge : public Bridge {
 public:
  NDCBridge(NDataBlock< T > & db) : dbcs_(db) { 
  //DBG cout << "+++ NDCBridge() this= " << hex << (long int)this << dec << endl; 
  }
  virtual ~NDCBridge() {
  //DBG cout << "--- ~NDCBridge() this= " << hex << (long int)this << dec << endl; 
  }
 protected:
  NDataBlock< T > dbcs_;
};

//******************************************
// Fonction de cast de type de contenu - sans conversion - de tableaux
// pour tableau packe en memoire
/*! 
  \ingroup TArray 
  \brief Function to handle Array content cast without conversion (shared data)
*/
template <class T1, class T2>								    
TArray< T2 > ArrayCast( TArray< T1 > & a, T2 ct, sa_size_t doff=0, sa_size_t dstep=1) 
{
  NDCBridge< T1 > * db = new NDCBridge< T1 >(a.DataBlock());
  //      if (a.AvgStep() != 1)
  if (a.IsPacked() == false)
    throw ParmError("ArrayCast - Pb: Not a packed array");
  //DBG float rs = (float)sizeof(T1)/(float)sizeof(T2);
  //DBG cout << "--DBG-ArrayCast SizeOf T1= " << sizeof(T1)
  //DBG     << " T2= " << sizeof(T2) << " rs=T1/T2= " << rs << endl;
  
  int_4 ndim = a.Rank();
  sa_size_t sz[BASEARRAY_MAXNDIMS];
  for(int_4 k=0; k<BASEARRAY_MAXNDIMS; k++)   sz[k] = 0;
  for(int_4 k=0; k<ndim; k++)  sz[k] = a.Size(k);
  // Pour gerer correctement les vecteurs colonnes
  int kszsc = 0;
  if ((sz[0] == 1) && (sz[1] > 1))   kszsc = 1;
  if (dstep > 1) 
    sz[kszsc] = sz[kszsc]*sizeof(T1)/sizeof(T2)/dstep;
  else 
    sz[kszsc] = sz[kszsc]*sizeof(T1)/sizeof(T2);
  //DBG  cout << " --DBG-ArrayCast SzX=" << a.SizeX() 
  //     << " rs=" << rs << " dstep=" << dstep << " --> SzX=" << sz[0] << endl;
  
  sa_size_t step = dstep; 
  sa_size_t off = doff; 
  TArray< T2 > ac(ndim, sz, (T2 *)(a.Data()), step, off, db);
  ac.SetMemoryMapping(a.GetMemoryMapping());
  ac.SetTemp(true);
  return ac;
}


//******************************************
// cast de contenu - sans conversion - tableau complex vers float
/*! 
  \ingroup TArray 
  \brief Complex to real array cast - without conversion (shared data) 
*/
template <class T>
TArray<T> ArrCastC2R(TArray< std::complex<T> > & a)  
{
  T x = 0;
  return ArrayCast(a, x);
}

//******************************************
// cast de contenu - sans conversion - tableau float vers complex
/*! 
  \ingroup TArray 
  \brief Real to complex array cast - without conversion (shared data)
*/
template <class T>
TArray< std::complex<T> > ArrCastR2C(TArray< T > & a)  
{
  std::complex<T> x = 0;
  //  return ArrayCast< TArray< T > , complex<T> > (a, x);
  return ArrayCast(a, x);
}


//******************************************
// Acces a la partie reelle d'un tableau complex - en shared data
// To access real part of a complex array (shared data)
/*! 
  \ingroup TArray 
  \brief Return the real part of a complex array (shared data)
*/
template <class T>
TArray<T> SDRealPart(TArray< std::complex<T> > & a)  
{
  T x = 0;
  return ArrayCast(a, x, 0, 2);
}

//******************************************
// Acces a la partie imaginaire d'un tableau complex - en shared data
// To access imaginary part of a complex array (shared data)
/*! 
  \ingroup TArray 
  \brief Return the imaginary part of a complex array (shared data)
*/
template <class T>
TArray<T> SDImagPart(TArray< std::complex<T> > & a)  
{
  T x = 0;
  return ArrayCast(a, x, 1, 2);
}

} // Fin du namespace

#endif
