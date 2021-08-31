// This may look like C code, but it is really -*- C++ -*-
//-----------------------------------------------------------
// Class  PPF_TPointer_IO< FlagVector<N> > 
// SOPHYA class library - (C)  UPS+LAL IN2P3/CNRS  , CEA-Irfu 
// R. Ansari      UPS+LAL IN2P3/CNRS  2014
//-----------------------------------------------------------

#ifndef PPFPIOFLGVEC_H_SEEN
#define PPFPIOFLGVEC_H_SEEN

#include <string.h>
#include "flagvector.h"
#include "ppftpointerio.h"


namespace SOPHYA {

//! PPF_TPointer_IO class specialization for FlagVector<N>
/* DECL_TEMP_SPEC   equivalent a template <> , pour SGI-CC en particulier */
template <uint_2 N>
class PPF_TPointer_IO< FlagVector<N> > {
public:
  static void Copy(FlagVector<N> * dest, FlagVector<N> const * src, size_t n)
  { memcpy((void *)dest, (const void *)src, n*sizeof(FlagVector<N>)); }
  static void Write(POutPersist & os, FlagVector<N> const * arr, size_t n)
  { os.PutBytes((void *)arr,n*sizeof(FlagVector<N>)); }
  static void Read(PInPersist & is, FlagVector<N> * arr, size_t n)
  { is.GetBytes((void *)arr,n*sizeof(FlagVector<N>)); }
  static bool isFixed_IO_RecordSize() { return true; } 
};

/*! Writes the FlagVector<N> object \b fgv to the POutPersist stream \b os , without object id/tag as N bytes*/
template <uint_2 N>
inline POutPersist& operator << (POutPersist& os, FlagVector<N> const & fgv)
{ os.PutBytes(fgv.bit_array(),N);  return(os); }
/*! Read N bytes from the PInPersist stream \b is and initializes the \b fgv object */
template <uint_2 N>
inline PInPersist& operator >> (PInPersist& is, FlagVector<N> & fgv)
{ is.GetBytes(fgv.bit_array(),N); return(is); }

} // Fin namespace

#endif
