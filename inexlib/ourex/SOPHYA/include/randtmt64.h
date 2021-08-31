//-----------------------------------------------------------
// Classes TinyMT64RandGen
// Generateur aleatoire utilisant le code TinyMT 64 bits 
// SOPHYA class library - (C)  UPS+LAL IN2P3/CNRS  , CEA-Irfu 
// R. Ansari          (C) UPS+LAL IN2P3/CNRS  2009
// C. Magneville      (C) DAPNIA/SPP  CEA     2009
//-----------------------------------------------------------

#ifndef RANDTMT64_H_SEEN
#define RANDTMT64_H_SEEN


#include "machdefs.h"
#include "objfio.h"

#include "randinterf.h"

#include <vector>

//--- Declaration de la struct definie ds tinymt64.h - inclus uniquement dans le .cc  
struct TINYMT64_T ;
typedef struct TINYMT64_T tinymt64_t;
//------------------- 

namespace SOPHYA {

//! Pseudorandom number generator class using dSFMT code 
class TinyMT64RandGen : public RandomGeneratorInterface {

 public:
  TinyMT64RandGen(uint_8 seed=12345);
  virtual ~TinyMT64RandGen();

  virtual void SetSeed(uint_8 seed);
  virtual void SetSeed(std::vector<uint_8> seed);
  virtual void AutoInit(int lp=0);
  virtual void ShowRandom();

  friend class ObjFileIO<TinyMT64RandGen> ;

 protected:
  virtual r_8 Next();
  tinymt64_t* tinymt64_ptr_;   // Pointeur car tinymt64_t pas encore defini

};

// Classe pour la gestion de persistance PPF :  ObjFileIO<TinyMT64RandGen>

/*! Writes the random generator object state in the POutPersist stream \b os */
inline POutPersist& operator << (POutPersist& os, TinyMT64RandGen & obj)
{ ObjFileIO<TinyMT64RandGen> fio(&obj);  fio.Write(os);  return(os); }
/*! Reads the random generator object state from the PInPersist stream \b is */
inline PInPersist& operator >> (PInPersist& is, TinyMT64RandGen & obj)
{ ObjFileIO<TinyMT64RandGen> fio(&obj); is.SkipToNextObject(); fio.Read(is); return(is); }

} /* namespace SOPHYA */

#endif 
