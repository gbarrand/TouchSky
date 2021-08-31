//------------------------------------------------------------------------------------
// SOPHYA class library - (C)  UPS+LAL IN2P3/CNRS , (C) CEA-Irfu-SPP
//------------------------------------------------------------------------------------

#ifndef SHEALPIXBASE_H_SEEN
#define SHEALPIXBASE_H_SEEN

#include "HEALPixUtils.h"
#include "sphpixbase.h"

namespace SOPHYA {

//--------------------------------------------------------------------------
//---- HEALPixBase : base class for HEALPix pixelisation of a sphere 
//--------------------------------------------------------------------------

class HEALPixBase : public SphericalPixelisationBase {
public:
  //! Creates a HEALPix pixelisation with ring or nested according to \b fgring
  explicit HEALPixBase(bool fgring=true);
  //! set of changes the pixelisation resolution by defining the HEALPix \b nside parameter, and flag specifying RING/NESTED scheme 
  virtual void setNSide(int_4 nside, bool fgring);
  //! set of changes the pixelisation resolution by defining the HEALPix \b nside parameter
  inline  void setNSide(int_4 nside)  
  { setNSide(nside, fgring_); }
  //! return the HEALPix nside parameter - same as GetPixelisationParameter()
  inline int_4 getNSide() const { return nSide_; }
  //! return the total number of pixels in the map 
  inline size_t size() const { return nPix_; }
  //! return the average solid angle covered by a pixel in steradian
  inline double getPixSolidAngle() const { return omeg_; }

  //---- the SphericalPixelisationBase interface implementation -----
  //! set or changes the pixelisation resolution / total number of pixels by specifying the pixelisation parameter \b m 
  virtual void  setPixelisationParameter(int_4 m);
  //! return the value of the pixelisation parameter
  virtual int_4 getPixelisationParameter() const;
  //! return the total number of pixels in the map - calls size()
  virtual size_t getNbPixels() const;
  //! return the index of the pixel defined by \b ll (a negative value if \b ll not in the map )
  virtual int_8 getIndex(LongitudeLatitude const& ll) const;
  //! return the direction of the pixel with index \b idx. can generate RangeCheckError exception. 
  virtual LongitudeLatitude getLongLat(int_8 idx) const; 
  //! return the pixel \b k solid angle coverage in steradians 
  virtual double getPixSolAngle(int_8 k) const; 
  //! return the map average angular resolution ( = sqrt(pixel average solid angle) )
  virtual Angle getAngularResolution() const;

  //--- some static methods 
  //! return the best NSide value providing a angular resolution equal or higher to \b res 
  static inline int_4 ResolutionToNSide(double res)   { return HEALPixUtils::ResolToSizeIndex(res); }
  //! return the angular resolution corresponding to a given \b nside 
  static inline double NSideToResolution(int_4 nside)   { return HEALPixUtils::SizeIndexToResol(nside); }

protected:
  //! use this method in derived classes to implement copy constructor or operator ( = ) 
  inline void copyFrom(HEALPixBase const& a)
  { fgring_=a.fgring_;  nSide_=a.nSide_;  nPix_=a.nPix_;  omeg_=a.omeg_; resol_=a.resol_; }

  bool fgring_;     //!< true -> RING pixelisation , false -> NESTED
  int_4 nSide_;     //!< HEALPix nside parameters nPix=12*nside^2 
  size_t nPix_;     //!< number of pixels 
  double omeg_;     //!< average per pixel solid angle 
  Angle resol_;     //!< average angular resolution
};

} // Fin du namespace

#endif

