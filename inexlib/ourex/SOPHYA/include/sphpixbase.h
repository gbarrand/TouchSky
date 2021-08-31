//------------------------------------------------------------------------------------
//   class SphericalPixelisationBase 
//        Interface definition for pixelisation of a sphere
// SOPHYA class library - (C)  UPS+LAL IN2P3/CNRS  , CEA-Irfu 
//      R. Ansari   , Univ. Paris-Sud, LAL IN2P3/CNRS   2015 
//------------------------------------------------------------------------------------

#ifndef SPHEREPIXGEOM_H_SEEN
#define SPHEREPIXGEOM_H_SEEN

#include "longlat.h"

namespace SOPHYA { 


//! Identification of different pixelisation schemes on a sphere 
enum SphPixScheme { SPix_HEALPix, SPix_ThetaPhi, SPix_ECP, SPix_Other };  

/*!
   \class SphericalPixelisationBase
   \ingroup SkyMap 

   \brief Base class defining the minimal interface for the pixelisation of a sphere 
*/

class SphericalPixelisationBase {
public:
  //! constructor, with the specification of the pixelisation scheme 
  SphericalPixelisationBase(SphPixScheme scheme)
  : scheme_(scheme)  { } 
  //! destructor (virtual) 
  virtual ~SphericalPixelisationBase() { }
  //! return the pixelisation scheme 
  inline SphPixScheme getScheme() const { return scheme_; }
  //! set or changes the pixelisation resolution / total number of pixels by specifying the pixelisation parameter \b m
  virtual void  setPixelisationParameter(int_4 m) =0;
  //! return the value of the pixelisation parameter
  virtual int_4 getPixelisationParameter() const =0;
  //! return the total number of pixels in the map 
  virtual size_t getNbPixels() const=0;
  //! return the index of the pixel defined by \b ll (return a negative index if  \b ll not in the map )
  virtual int_8 getIndex(LongitudeLatitude const& ll) const=0;
  //! return the direction of the pixel with index \b idx. can generate RangeCheckError exception. 
  virtual LongitudeLatitude getLongLat(int_8 idx) const=0; 
  //! return the pixel \b k solid angle coverage in steradians 
  virtual double getPixSolAngle(int_8 k) const =0; 
  //! return the map average angular resolution ( = sqrt(pixel average solid angle) )
  virtual Angle getAngularResolution() const =0; 

protected:
  SphPixScheme scheme_;  
};

} // namespace SOPHYA

#endif


