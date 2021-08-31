//------------------------------------------------------------------------------------
// SOPHYA class library - (C)  UPS+LAL IN2P3/CNRS  , CEA-Irfu 
// Useful definitions for coordinate systems (WCS) - Sky / Earth 
// Jan-May 2015 - R. Ansari, J.E.Campagne, C. Magneville
// Updated Jan 2019 (Introduction of class WCSCoordTransformInterface
//------------------------------------------------------------------------------------

#ifndef WCSDEF_H_SEEN
#define WCSDEF_H_SEEN

#include <string>

#include "longlat.h"

namespace SOPHYA {

/*! \brief WCS definition (World Coordinate System  - 2D positions on a sphere) 
  Definition of the coordinate system being used :
  \verbatim
  * WCS_Unknown : undefined coordinate system 
  * WCS_Earth : Longitude-Latitude on earth 
  * WCS_Equatorial : Equatorial Celestial coordinates  (right ascension, declination) 
  * WCS_Galactic : Galatic longitude,latitude Celestial coordinates 
  * WCS_Ecliptic : Ecliptic Celestial coordinates (with reference for longitude = vernal equinox)
  * WCS_HelioEcliptic : Ecliptic Celestial coordinates (with reference for longitude = sun vector ?? )
  * WCS_SuperGalactic : Super Galactic  Celestial coordinates 
  \endverbatim 
*/

// ===> DO NOT CHANGE EXISTING enum type VALUES - should be >=0 and < 65535 
enum WCSId { WCS_Unknown=0, 
	     WCS_Earth=1, 
	     WCS_Equatorial=11,  
	     WCS_Galactic=21,
	     WCS_Ecliptic=31,  
	     WCS_HelioEcliptic=41,  
	     WCS_SuperGalactic=51,  
};

//! convert a WCSId enum to integer 
unsigned short  WCSIdToInteger(WCSId wcsid);

//! convert an integer to  WCSId enum 
WCSId  IntegerToWCSId(unsigned short id);

//! convert a WCSId enum to the corresponding string representation 
std::string WCSIdToString(WCSId wcsid);


/*! \brief WCS convention / Reference system (corresponding to FITS-WCS RADESYS keyword 
  \verbatim
  * WCS_RS_Earth : (might change in future) 
  * WCS_RS_ICRS : undefined coordinate system 
  * WCS_RS_FK5 :  
  * WCS_RS_FK4 :  
   \endverbatim 
*/

enum WCS_RS_Id { WCS_RS_Unknown=0, 
		 WCS_RS_Earth=101, 
		 WCS_RS_ICRS=111, 
		 WCS_RS_FK5=121,
		 WCS_RS_FK4=131,  
};

//! convert a WCS_RS_Id enum to integer 
unsigned short  WCS_RS_IdToInteger(WCS_RS_Id wcsrsid);

//! convert an integer to  WCS_RS_Id enum 
WCS_RS_Id  IntegerToWCS_RS_Id(unsigned short id);

//! convert a WCS_RS_Id enum to the corresponding string representation 
std::string WCS_RS_IdToString(WCS_RS_Id wcsrsid);

/*!
  \class WCSCoordTransformInterface
  \ingroup SkyMap 

  Abstract interface definition for coordinate transformations between image (pixel level) coordinates 
  and WCS (World Coordinate System) 
  See http://www.atnf.csiro.au/people/mcalabre/WCS/ 
*/

class  WCSCoordTransformInterface {
public:
  WCSCoordTransformInterface() { } 
  virtual ~WCSCoordTransformInterface() { }
  /*! \brief Convert the image coordinates to WCS angular coordinates 
    \param ximg , yimg : (x,y) pixel level coordinate (along 1st, 2nd array axis)  (input) 
    \param radec : position on a sphere (output, two angles)  
  */
  virtual void ImageCoordToWCS(double ximg, double yimg, LongitudeLatitude& radec) const = 0;
  /*! \brief Convert WCS coordinates to image coordinates 
    \param radec : position on a sphere (input, two angles)  
    \param ximg , yimg : (x,y) pixel level coordinate (along 1st, 2nd array axis)  (output) 
  */
  virtual void WCSToImageCoord(LongitudeLatitude const& radec, double & ximg, double & yimg) const = 0;
  
};    
  
}  // End of namespace SOPHYA

#endif 
