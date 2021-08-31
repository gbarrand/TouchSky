//------------------------------------------------------------------------------------
// SOPHYA class library - (C)  UPS+LAL IN2P3/CNRS  , CEA-Irfu 
// Useful definitions for coordinate systems - Sky / Earth 
// Jan-May 2015 - R. Ansari, J.E.Campagne, C. Magneville
//------------------------------------------------------------------------------------

#include "wcsdef.h"

namespace SOPHYA {

//-- convert a WCSId enum to integer 
unsigned short  WCSIdToInteger(WCSId wcsid)
{
  unsigned short ri=0;
  switch(wcsid) {
  case WCS_Unknown:
    ri=0;
    break;
  case WCS_Earth:
    ri=1;
    break;
  case WCS_Equatorial:
    ri=11;
    break;
  case WCS_Galactic:
    ri=21;
    break;
  case WCS_Ecliptic:
    ri=31;
    break;
  case WCS_HelioEcliptic:
    ri=41;
    break;
  case WCS_SuperGalactic:
    ri=51;
    break;
  default:
    break;
  }
  return ri;
}

//-- convert a WCSId enum to integer 
WCSId  IntegerToWCSId(unsigned short id)
{
  WCSId rwcs=WCS_Unknown;
  switch(id) {
  case 0:
    rwcs=WCS_Unknown;
    break;
  case 1:
    rwcs=WCS_Earth;
    break;
  case 11:
    rwcs=WCS_Equatorial;
    break;
  case 21:
    rwcs=WCS_Galactic;
    break;
  case 31:
    rwcs=WCS_Ecliptic;
    break;
  case 41:
    rwcs=WCS_HelioEcliptic;
    break;
  case 51:
    rwcs=WCS_SuperGalactic;
    break;
  default:
    break;
  }
  return rwcs;
}

//-- convert a WCSId enum to the corresponding string representation 
std::string WCSIdToString(WCSId wcsid)
{
  std::string rs="WCS_?????";
  switch(wcsid) {
  case WCS_Unknown:
    rs="WCS_Unknown";
    break;
  case WCS_Earth:
    rs="WCS_Earth";
    break;
  case WCS_Equatorial:
    rs="WCS_Equatorial";
    break;
  case WCS_Galactic:
    rs="WCS_Galactic";
    break;
  case WCS_Ecliptic:
    rs="WCS_Ecliptic";
    break;
  case WCS_HelioEcliptic:
    rs="WCS_HelioEcliptic";
    break;
  case WCS_SuperGalactic:
    rs="WCS_SuperGalactic";
    break;
  default:
    break;
  }
  return rs;
}

// convert a WCS_RS_Id enum to integer 
unsigned short  WCS_RS_IdToInteger(WCS_RS_Id wcsrsid)
{
  unsigned short ri=0;
  switch(wcsrsid) {
  case WCS_RS_Unknown:
    ri=0;
    break;
  case WCS_RS_Earth:
    ri=101;
    break;
  case WCS_RS_ICRS:
    ri=111;
    break;
  case WCS_RS_FK5:
    ri=121;
    break;
  case WCS_RS_FK4:
    ri=131;
    break;
  default:
    break;
  }
  return ri;
}

// convert an integer to  WCS_RS_Id enum 
WCS_RS_Id  IntegerToWCS_RS_Id(unsigned short id)
{
  WCS_RS_Id rwcs=WCS_RS_Unknown;
  switch(id) {
  case 0:
    rwcs=WCS_RS_Unknown;
    break;
  case 101:
    rwcs=WCS_RS_Earth;
    break;
  case 111:
    rwcs=WCS_RS_ICRS;
    break;
  case 121:
    rwcs=WCS_RS_FK5;
    break;
  case 131:
    rwcs=WCS_RS_FK4;
    break;
  default:
    break;
  }
  return rwcs;
}

// convert a WCS_RS_Id enum to the corresponding string representation 
std::string WCS_RS_IdToString(WCS_RS_Id wcsrsid)
{
  std::string rs="WCS_RS_?????";
  switch(wcsrsid) {
  case WCS_RS_Unknown:
    rs="WCS_RS_Unknown";
    break;
  case WCS_RS_Earth:
    rs="WCS_RS_Earth";
    break;
  case WCS_RS_ICRS:
    rs="WCS_RS_ICRS";
    break;
  case WCS_RS_FK5:
    rs="WCS_RS_FK5";
    break;
  case WCS_RS_FK4:
    rs="WCS_RS_FK4";
    break;
  default:
    break;
  }
  return rs;
}




}  // End of namespace SOPHYA
