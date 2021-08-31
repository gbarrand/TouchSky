//-----------------------------------------------------------
// Utlitaires string 
// SOPHYA class library - (C)  UPS+LAL IN2P3/CNRS  , CEA-Irfu 
// C. Magneville - 1996-2003  
//-----------------------------------------------------------

#ifndef STRUTILXX_H_SEEN
#define STRUTILXX_H_SEEN

#include "machdefs.h"
#include <vector>
#include <string>

namespace SOPHYA {

  void FillVStringFrString(std::string const & s,std::vector<std::string>& vs,char sep = ' ');
  void SplitStringToVString(std::string const & s,std::vector<std::string>& vs,char separator=' ');

} // FIN namespace SOPHYA 

#endif
