//-----------------------------------------------------------
// Classe BitVector :
// SOPHYA class library - (C)  UPS+LAL IN2P3/CNRS  , CEA-Irfu 
// C. Magneville      (C) DAPNIA/SPP  CEA     
//-----------------------------------------------------------

#ifndef BITVECTOR_H_SEEN
#define BITVECTOR_H_SEEN

#include "machdefs.h"
#include <vector> 

#include "ndatablock.h"
#include "ppersist.h"
#include "objfio.h"

namespace SOPHYA {

  // type d'element et nombre de bits dans un element du vecteur
  typedef uint_8 BitVec_Type;
  #define BitVec_BITPERWORD_  64ULL
  ////typedef uint_1 BitVec_Type;
  ////#define BitVec_BITPERWORD_  8ULL

class BitVector : public AnyDataObj {
  friend class ObjFileIO<BitVector>; //  Pour la gestion de persistance

  public:
  BitVector(uint_8 nbits,uint_4 vecsize=128);
  BitVector(BitVector const& bv);
  BitVector(void);
  virtual ~BitVector(void);

  BitVector& operator = (const BitVector& bv);
  BitVector& operator = (bool v);
  BitVector& Negate(void);
  //! make AND (without changing extra bits if any)
  BitVector& operator &= (const BitVector& bv) {return AND(bv,0);}
  BitVector& AND(const BitVector& bv,int putextra=0);
  //! make OR (without changing extra bits if any)
  BitVector& operator |= (const BitVector& bv) {return OR(bv,0);}
  BitVector& OR(const BitVector& bv,int putextra=0);

  inline uint_8 Size(void) const {return nbits_;}
  bool operator()(uint_8 k) const;
  void Set(uint_8 k,bool v=true);
  void Set(uint_8 k1,uint_8 k2,bool v=true);

  uint_8 NValues(bool v=true);

  void Print(uint_8 k1=0,int_8 dk=0);

  private:
  void extend_(uint_8 nbitsnew);
  void delete_(void);

  uint_8 nbits_;
  uint_4 vecsize_;
  uint_8 bitpervec_;
  uint_8 nvector_;
  std::vector< NDataBlock<BitVec_Type> > vv_;
};

/*! Writes the object in the POutPersist stream \b os */
inline POutPersist& operator << (POutPersist& os, BitVector & obj)
{ ObjFileIO<BitVector> fio(&obj);  fio.Write(os);  return(os); }
/*! Reads the object from the PInPersist stream \b is */
inline PInPersist& operator >> (PInPersist& is, BitVector & obj)
{ ObjFileIO<BitVector> fio(&obj); is.SkipToNextObject(); fio.Read(is); return(is); }

} // namespace SOPHYA

#endif
