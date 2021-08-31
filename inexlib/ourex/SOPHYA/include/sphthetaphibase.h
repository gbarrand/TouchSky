//------------------------------------------------------------------------------------
// SOPHYA class library - (C)  UPS+LAL IN2P3/CNRS , (C) CEA-Irfu-SPP
//  Classe de base implementant la pixelisation theta-phi
//------------------------------------------------------------------------------------

#ifndef SPHTHETAPHIBASE_H_SEEN
#define SPHTHETAPHIBASE_H_SEEN
#include "sphpixbase.h"
#include "ndatablock.h"

namespace SOPHYA {

//--------------------------------------------------------------------------
//---- SphThetaPhiBase : base class for Theta-Phi pixelisation of a sphere 
//--------------------------------------------------------------------------


class SphThetaPhiBase : public SphericalPixelisationBase {
public:
  //! Creates a SphThetaPhiBase - number of pixels in each theta ring will be multiple of 3 if \b fgby3 is true
  explicit SphThetaPhiBase(bool fgby3=false); 

  //! return true if number of pixels in each ring (theta slice) is multiple o three
  inline bool IsRingNPixMultipleOf3() const { return FgBy3_; }
  //! define or changes pixelisation resolution by specifying the number of rings (or slices) along theta 
  virtual  void setNbRings(int_4 nring); 
  //! define or changes pixelisation resolution by specifying the number of rings in theta - alias for setNbRings()
  inline void setNbThetaSlices(int_4 nslicetheta)
  { return setNbRings(nslicetheta); }
  //! define the pixelisation resolution - \b nring rings along theta , from 0 to Pi 
  inline void setNbRings(int_4 nring, bool fgby3) 
  { FgBy3_=fgby3;  setNbRings(nring); }
  //! return the number of rings (slices in theta)
  inline int_4 getNbRings() const { return NTheta_; }
  //! return the number of rings (slices in theta) - alias for getNbRings()
  inline int_4 getNbThetaSlices() const { return NTheta_; }
  //! return the total number of pixels  
  inline size_t size() const { return NPix_; }
  //! return the average solid angle covered by a pixel in steradian
  inline double getPixSolidAngle() const { return Omega_; }

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

  //---------- some specific methods - faut-il les garder virtual ?  (Reza, June 2015) 
  //! return the linear pixel index for the \b jp pixel in slice \b kt 
  virtual int_4  getIndex(int_4 kt, int_4 jp) const; 
  //! return the slice number and the index in the slice from the pixel number 
  virtual void   getThetaPhiIndex(int_8 idx, int_4& kt, int_4& jp) const;
  //! return the number of pixels in the  the slice \b kt 
  virtual int_4  getRingNbPixels(int_4 kt) const;
  //! return the central theta value for slice \b kt
  virtual double getRingTheta(int_4 kt) const;
  //! return the boundary (limits) in theta for slice \b kt : the return value is the number of pixels in the slice
  virtual int_4  getRingBoundary(int_4 kt, double& th1, double& th2) const;
  //! return the boundary (limits) in theta,phi for pixel \b idx 
  virtual void   getPixelBoundary(int_8 idx, double& th1, double& th2, double& phi1, double& phi2) const;
  //! return the boundary (limits) in theta,phi for slice \b kt, and the pixel \b jp in the slice 
  virtual void   getThetaPhiBoundary(int_4 kt, int_4 jp, double& th1, double& th2, double& phi1, double& phi2) const;

  //! return true if same pixelisation geometry (same scheme, same nombuer of pixels ...) 
  inline bool isSamePixelisation(SphThetaPhiBase const& b) const 
  { return ( (FgBy3_==b.FgBy3_)&&(NPix_==b.NPix_) ); }

  //--- some static methods 
  //! return the best NTheta value providing a angular resolution equal or higher to \b res 
  static int_4 ResolutionToNTheta(double res);
  //! return the angular resolution corresponding to a given \b ntheta 
  static double NThetaToResolution(int_4 ntheta);
  
protected:
  //! use this method in derived classes to implement copy constructor or operator ( = ) 
  void copyFrom(SphThetaPhiBase const& a);

  bool FgBy3_;          //!< true -> number of pixels in each theta ring is multiple of 3
  int_4 NTheta_;        //!< number of slices in theta 
  size_t NPix_;         //!< total number of pixels 
  double DeltaTheta_;   //!< ring extension along theta  
  double ThetaPole_;    //!< Pixels at pole radius = DeltaTheta_/2
  double Omega_;        //!< average per pixel solid angle
  Angle Resol_;         //!< average angular resolution 
  NDataBlock<int_8>  CNphi_;  //!< cumulated number of pixels, up to - and not including - the corresponding ring/slice
};

} // Fin du namespace

#endif

