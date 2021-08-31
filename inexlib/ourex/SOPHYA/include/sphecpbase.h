//------------------------------------------------------------------------------------
// SOPHYA class library - (C)  UPS+LAL IN2P3/CNRS , (C) CEA-Irfu-SPP
//  Classe de base implementant la pixelisation ECP : Equatorial Cylindrical Projection
//     R. Ansari, LAL/IN2P3-CNRS, Univ. Paris Sud - 2015  
//------------------------------------------------------------------------------------

#ifndef SPHECPBASE_H_SEEN
#define SPHECPBASE_H_SEEN
#include "sphpixbase.h"

namespace SOPHYA {

//--------------------------------------------------------------------------
//---- SphECPBase : base class for ECP pixelisation of a sphere 
//----     ECP : Equatorial Cylindrical Projection 
//--------------------------------------------------------------------------


class SphECPBase : public SphericalPixelisationBase {
public:
  //! Creates a SphECPBase - default constructor - full 4 pi map 
  explicit SphECPBase(); 
  //! Constructor: full (4 pi) coverage, m slices in theta over pi (rings) -  2m in phi over 2 pi
  explicit SphECPBase(int m);
  //! Constructor: full (4 pi) coverage, ntet slices in theta over pi (rings) - nphi in phi over 2 pi 
  explicit SphECPBase(int ntet, int nphi);
  //! Constructor, partial coverage: ntet rings with tet1<=theta<teta2 and nphi pixels in each ring phi1<=phi<phi2
  explicit SphECPBase(r_8 tet1, r_8 tet2, int ntet, r_8 phi1, r_8 phi2, int nphi);

// Informations sur carte partielle/complete + zone de couverture
//! return true if the map do not cover the full sky
  inline  bool	 IsPartial() const { return _partial; }
//! return the lower bound of the map in theta (0. for a ECP map with full coverage) 
  inline  double getMinTheta() const { return _theta1; }
//! return the upper bound of the map in theta (Pi for a ECP map with full coverage) 
  inline  double getMaxTheta() const { return _theta2; }
//! return the map pixel size along theta (in radian) 
  inline  double getDeltaTheta() const { return _dtheta; }
//! return the lower bound of the map in phi (0. for a ECP map with full coverage) 
  inline  double getMinPhi() const { return _phi1; }
//! return the upper bound of the map in phi (2Pi for a ECP map with full coverage) 
  inline  double getMaxPhi() const { return _phi2; }
//! return the map pixel size along phi (in radian) 
  inline  double getDeltaPhi() const { return _dphi; }

  //! return the index of the pixel defined by 
  inline int_4   getIndexTP(int_4 ktheta, int_4 jphi)  { return( ktheta*_nphi+jphi);  }

  //! return the total number of pixels  
  inline size_t size() const { return _npix; }
  //! return the number of rings (slices in theta) 
  inline int_4 getNbThetaSlices() const { return _ntheta; }
  //! return the number of phi slices 
  inline int_4 getNbPhiSlices() const { return _nphi; }

  //---- the SphericalPixelisationBase interface implementation -----
  //! set or changes the pixelisation resolution / total number of pixels by specifying the pixelisation parameter \b m 
  virtual void  setPixelisationParameter(int_4 m);
  //! return the value of the pixelisation parameter - which is equal to NbTheta
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

//! return the total solid angle covered by the map in steradians  
  inline double TotSolAngle(void) const
  { return fabs((_phi2-_phi1)*(cos(_theta2)-cos(_theta1))); }

  //--- some static methods 
  //! return the best NTheta value providing a angular resolution equal or higher to \b res 
  static int_4 ResolutionToNTheta(double res);
  //! return the angular resolution corresponding to a given \b ntheta 
  static double NThetaToResolution(int_4 ntheta);
  
protected:
  //! copy pixelisation geometry - use this method in derived classes to implement copy constructor or operator ( = ) 
  void  copyFrom(const SphECPBase& a);
  //! return true if the two SphECPBase maps DO have the same pixelisation geometry 
  bool  CheckSameGeometry(const SphECPBase& a);

  //----- data members 
  bool _partial;
  int_4 _ntheta, _nphi;
  size_t _npix;
  r_8 _theta1,_theta2;
  r_8 _phi1, _phi2;
  r_8 _dtheta, _dphi;
};

} // Fin du namespace

#endif

