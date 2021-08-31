//-----------------------------------------------------------
// Classe SophyaInitiator
// Classe d'initialiseur de module de SOPHYA
// SOPHYA class library - (C)  UPS+LAL IN2P3/CNRS  , CEA-Irfu 
// R. Ansari          (C) UPS+LAL IN2P3/CNRS  2000 
//-----------------------------------------------------------

#ifndef SOPHYAINIT_H_SEEN
#define SOPHYAINIT_H_SEEN

#include "machdefs.h"
#include <string>

namespace SOPHYA {

void PrintSophyaVersion(void);
double SophyaVersion(void); /* Version + (Revision/1000) */

// Classe d''initialisation pour les modules SOPHYA

//! Initializer class for sophya modules
class SophyaInitiator {
private:
  static int FgInit;
public:
  			SophyaInitiator();
  virtual		~SophyaInitiator();

  static double         GetVersion(std::string& svers);
  static void           PrintVersion(bool fglist=false);

  static int            RegisterModule(const char* name, double version);
  static int            ListModules(std::ostream& os);
};

#define SophyaInit() SophyaInitiator sophyainitiator

} // Fin du namespace

#endif

