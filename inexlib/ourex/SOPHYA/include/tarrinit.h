//-----------------------------------------------------------
// classe TArrayInitiator 
// Classe d'initialisation du module TArray 
// SOPHYA class library - (C)  UPS+LAL IN2P3/CNRS  , CEA-Irfu 
// R. Ansari - 2000-2020 
//-----------------------------------------------------------

#ifndef TARRAYINIT_H_SEEN
#define TARRAYINIT_H_SEEN


// Classe d''initialisation pour le module de TArray 
#include "sophyainit.h"

namespace SOPHYA {

//! Array Matrices and Vector initiator
class TArrayInitiator : public SophyaInitiator {
private:
  static int FgInit;
public:
  			TArrayInitiator();
  virtual		~TArrayInitiator();
};

#undef SophyaInit
#define SophyaInit() TArrayInitiator tarrayinitiator

} // Fin du namespace

#endif

