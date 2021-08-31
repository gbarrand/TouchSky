//-----------------------------------------------------------
// Classe FMTRandGen
// Generateur aleatoire utilisant le code dSFMT 
// SOPHYA class library - (C)  UPS+LAL IN2P3/CNRS  , CEA-Irfu 
// R. Ansari          (C) UPS+LAL IN2P3/CNRS  2009
// C. Magneville      (C) DAPNIA/SPP  CEA     2009
//-----------------------------------------------------------

#ifndef FMTRANDGEN_H_SEEN
#define FMTRANDGEN_H_SEEN


#include "machdefs.h"
#include "objfio.h"

#include "randinterf.h"

#include <vector>

//--- Declaration de la struct definie ds dSFMT.h - inclus uniquement dans le .cc  
struct DSFMT_T ; 
typedef struct DSFMT_T dsfmt_t;
//------------------- 

namespace SOPHYA {

//! Pseudorandom number generator class using dSFMT code 
class FMTRandGen : public RandomGeneratorInterface {

 public:
  FMTRandGen(uint_4 seed=12345);
  virtual ~FMTRandGen();

  virtual void SetSeed(uint_4 seed);
  virtual void SetSeed(std::vector<uint_4> seed);
  virtual void AutoInit(int lp=0);
  virtual void ShowRandom();

  friend class ObjFileIO<FMTRandGen> ;

 protected:
  virtual r_8 Next();
  dsfmt_t* dsfmt_ptr_;   // Pointeur car dsfmt_t pas encore defini

};

// Classe pour la gestion de persistance PPF :  ObjFileIO<FMTRandGen>

/*! Writes the random generator object state in the POutPersist stream \b os */
inline POutPersist& operator << (POutPersist& os, FMTRandGen & obj)
{ ObjFileIO<FMTRandGen> fio(&obj);  fio.Write(os);  return(os); }
/*! Reads the random generator object state from the PInPersist stream \b is */
inline PInPersist& operator >> (PInPersist& is, FMTRandGen & obj)
{ ObjFileIO<FMTRandGen> fio(&obj); is.SkipToNextObject(); fio.Read(is); return(is); }

} /* namespace SOPHYA */

#endif 
