//-----------------------------------------------------------
// Classe TinyMT32RandGen
// Generateur aleatoire utilisant le code TinyMT 32 bits 
// SOPHYA class library - (C)  UPS+LAL IN2P3/CNRS  , CEA-Irfu 
// R. Ansari          (C) UPS+LAL IN2P3/CNRS  2009
// C. Magneville      (C) DAPNIA/SPP  CEA     2009
//-----------------------------------------------------------

#ifndef RANDTMT32_H_SEEN
#define RANDTMT32_H_SEEN


#include "machdefs.h"
#include "objfio.h"

#include "randinterf.h"

#include <vector>

//--- Declaration de la struct definie ds tinymt32.h - inclus uniquement dans le .cc  
struct TINYMT32_T ;
typedef struct TINYMT32_T tinymt32_t;
//------------------- 

namespace SOPHYA {

//! Pseudorandom number generator class using dSFMT code 
class TinyMT32RandGen : public RandomGeneratorInterface {

 public:
  TinyMT32RandGen(uint_4 seed=12345);
  virtual ~TinyMT32RandGen();

  virtual void SetSeed(uint_4 seed);
  virtual void SetSeed(std::vector<uint_4> seed);
  virtual void AutoInit(int lp=0);
  virtual void ShowRandom();

  friend class ObjFileIO<TinyMT32RandGen> ;

 protected:
  virtual r_8 Next();
  tinymt32_t* tinymt32_ptr_;   // Pointeur car tinymt32_t pas encore defini

};

// Classe pour la gestion de persistance PPF :  ObjFileIO<TinyMT32RandGen>

/*! Writes the random generator object state in the POutPersist stream \b os */
inline POutPersist& operator << (POutPersist& os, TinyMT32RandGen & obj)
{ ObjFileIO<TinyMT32RandGen> fio(&obj);  fio.Write(os);  return(os); }
/*! Reads the random generator object state from the PInPersist stream \b is */
inline PInPersist& operator >> (PInPersist& is, TinyMT32RandGen & obj)
{ ObjFileIO<TinyMT32RandGen> fio(&obj); is.SkipToNextObject(); fio.Read(is); return(is); }

} /* namespace SOPHYA */

#endif 
