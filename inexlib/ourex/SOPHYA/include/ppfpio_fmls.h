// This may look like C code, but it is really -*- C++ -*-
//-----------------------------------------------------------
// Class  PPF_TPointer_IO< FMLString<N> > 
// SOPHYA class library - (C)  UPS+LAL IN2P3/CNRS  , CEA-Irfu 
// R. Ansari      UPS+LAL IN2P3/CNRS  2013
//-----------------------------------------------------------

#ifndef PPFPIOFMLS_H_SEEN
#define PPFPIOFMLS_H_SEEN

#include "fmlstr.h"
#include "ppftpointerio.h"


namespace SOPHYA {

//! PPF_TPointer_IO class specialization for FMLString<N>
/* DECL_TEMP_SPEC   equivalent a template <> , pour SGI-CC en particulier */
template <uint_2 N>
class PPF_TPointer_IO< FMLString<N> > {
public:
  static void Copy(FMLString<N> * dest, FMLString<N> const * src, size_t n)
  { memcpy((void *)dest, (const void *)src, n*sizeof(FMLString<N>)); }
  static void Write(POutPersist & os, FMLString<N> const * arr, size_t n)
  { os.PutBytes((void *)arr,n*sizeof(FMLString<N>)); }
  static void Read(PInPersist & is, FMLString<N> * arr, size_t n)
  { is.GetBytes((void *)arr,n*sizeof(FMLString<N>)); }
  static bool isFixed_IO_RecordSize() { return true; } 
};

/*! Writes the FMLString<N> object \b fms to the POutPersist stream \b os , without object id/tag */
template <uint_2 N>
inline POutPersist& operator << (POutPersist& os, FMLString<N> const & fms)
{ os.PutBytes(fms.cbuff_ptr(),N);  return(os); }
/*! Read N bytes from the PInPersist stream \b is and initializes the \b fms object */
template <uint_2 N>
inline PInPersist& operator >> (PInPersist& is, FMLString<N> & fms)
{ char buff[N];  is.GetBytes(buff,N); buff[N-1]='\0'; fms=buff; return(is); }

} // Fin namespace

#endif
