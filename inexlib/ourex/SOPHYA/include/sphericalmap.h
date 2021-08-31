//------------------------------------------------------------------------------------
//  class SphericalMap<T> : base pour des cartes spheriques avec contenu numerique
// SOPHYA class library - (C)  UPS+LAL IN2P3/CNRS , (C) CEA-Irfu-SPP    2000-2015
//    G. Le Meur LAL IN2P3/CNRS , 2000
//    R. Ansari, Univ. Paris-Sud, LAL IN2P3/CNRS  2015 
//------------------------------------------------------------------------------------

#ifndef SPHERICALMAP_SEEN
#define SPHERICALMAP_SEEN

// valeurs de Pi, 2*Pi, etc                                              
#include "smathconst.h"
#include <math.h>
#include "anydataobj.h"
#include "dvlist.h"
#include "spherepos.h"
#include "datatype.h"
#include "sunitpcst.h"
#include <iostream>
#include <typeinfo>

#include "tvector.h" 

// Map of pixels on a whole sphere.
// Class hierarchy :
//   SphericalMap
//      SphereThetaPhi
//      SphereHEALPix 
//      SphereIco
// 
//      LocalMap




namespace SOPHYA {

//! Set global print level for all spherical maps
void SphericalMap_SetGlobalPrintLevel(int_4 lev=0); 
//! Get global print level for all spherical maps 
int_4 SphericalMap_GetGlobalPrintLevel(); 

/*!
  \class SphericalMap
  \ingroup SkyMap
  Base class for map of pixels in spherical geometry - 
  map coverage might be partial or full 

 Class hierarchy :
\verbatim
  SphericalMap<T>
    SphereHEALPix<T>      (4 Pi coverage)
    SphereThetaPhi<T>     (4 Pi coverage)
    SphereECP<T>          (full or partial coverage)
    LocalMap<T>           (local map : tangent plane) 
\endverbatim  
*/

template<class T>
class SphericalMap : public AnyDataObj  
{  
public :
  //! default constructor - optional specification of the reference coordinate system
  SphericalMap() 
    : cs_(), mInfo_(NULL) 
  { }
  //! destructor 		 
  virtual   ~SphericalMap() 
  { if (mInfo_) delete mInfo_; }

//! Set/Change/the coordinate system
  inline void  SetCoordSys(SphereCoordSys const& cs)  { cs_ = cs; }
//! return the map reference coordinate system
  inline SphereCoordSys const& GetCoordSys() const { return(cs_); }

//! return the total mumber of pixels in the map 
  virtual int_4 NbPixels() const=0;
//! access to the pixel number \b k (return a reference to the pixel content) 
  virtual T& PixVal(int_4 k)=0;
//! access to the pixel number \b k - const version - (return a const reference to the pixel content) 
  virtual T  PixVal(int_4 k) const=0;

//! return true if the map contains the direction specified by the spherical angles \b (theta, phi) 
  virtual bool ContainsSph(double theta, double phi) const=0;
//! return true if the map contains the direction specified by the LongitudeLatitude \b ll
  inline bool Contains(LongitudeLatitude const& ll) const
  { return ContainsSph(ll.Theta(), ll.Phi()); }

//! return the index of the pixel at containing the direction \b (theta,phi) 
  virtual int_4 PixIndexSph(double theta, double phi) const=0;
//! return the index of the pixel at containing the direction \b ll
  inline int_4 PixIndex(LongitudeLatitude const& ll)
  { return PixIndexSph(ll.Theta(), ll.Phi()); } 

//! access to the pixel toward the direction \b (theta,phi) -> return a reference to the pixel content 
  virtual T& PixValSph(double theta, double phi)
  { return PixVal(PixIndexSph(theta,phi)); }
//! access to the pixel toward the direction \b (theta,phi) - const version -> return a const reference to the pixel content 
  virtual T  PixValSph(double theta, double phi) const
  { return PixVal(PixIndexSph(theta,phi)); }

/*! 
  \brief return the spherical coordinates of the center of pixel number \b k 
  theta and phi are filled with the coordinates of pixel number \b k upon return 
*/
  virtual void PixThetaPhi(int_4 k, double& theta, double& phi) const=0;

  //! provides a integer characterizing the pixelization refinement  (depending of the type of the map)             
  virtual int_4 SizeIndex() const=0;
  //! return a string corresponding to the map type (deprecated)
  virtual std::string TypeOfMap() const =0;

  //! return the solid angle covered by pixel \b k (in steradians) 
  virtual double PixSolAngle(int_4 k) const =0; 

  //! Setting blockdata to temporary (see ndatablock documentation) 
  virtual void SetTemp(bool temp=false) const =0;

//! Setting all pixel values to the constant \b v 
  virtual T SetPixels(T v);
//! Setting all pixel values to the constant \b v : alias for SetPixels
  inline  T SetCst(T v) { return SetPixels(v); }

//! show information about the spherical map on stream os
  virtual void  Show(std::ostream& os) const;
//! Show information on \b cout
  inline  void  Show() const { Show(std::cout); }

                     
//! Return a reference to the associated DVList object  
  DVList& Info() const ;
//! access to the DVList info object (alias for Info())
  inline DVList& getInfo() const { return Info(); }
//! return true if the  DVList info Object has been created (not empty)
  inline bool HasInfoObject() const { return ((mInfo_!=NULL)?true:false); }  
//! return a const pointer to the DVLIst info object - might be a null pointer 
  const DVList*  ptrInfo() const  { return mInfo_; }

  //--- Changement et acces a l'unite physique associee
  //! Define or change the physical unit of the data.
  virtual void SetUnits(const Units& un) = 0;
  /*!  \brief Return the physical unit of the data. 
    Flag sdone is set to true if SetUnits() has been called.  */
  virtual Units GetUnits(bool& sdone) const = 0;
  //! Return the physical unit of the data 
  inline Units GetUnits() const  
    {bool fg; return GetUnits(fg); }

// index characterizing the size pixelization : m for SphereThetaPhi
// nside for Gorski sphere...
//! changes the map resolution/size (number of pixels) according to the pixelisation parameter m 
  virtual void Resize(int_4 m)=0;
//! return number of slices in theta 
  virtual uint_4 NbThetaSlices() const=0;
//! return the theta value for slice defined by \b index
  virtual r_8  ThetaOfSlice(int_4 index) const=0;
/*!
  Return true if some theta slices have a symmetric counter-part at (Pi-Theta)
  (the default implementation return false) 
*/
  virtual bool  HasSymThetaSlice() const { return false; }
/*! 
  Return the slice index for the symmetric slice theta=Pi-ThetaOfSlice(idx) 
  an invalid index is returned if the symmetric slice does not exist
*/
  virtual int_4 GetSymThetaSliceIndex(int_4 idx) const { return -1; }
  virtual void GetThetaSlice(int_4 index,r_8& theta, TVector<r_8>& phi, TVector<T>& value) const=0; 
  virtual void GetThetaSlice(int_4 sliceIndex, r_8& theta, r_8& phi0, TVector<int_4>& pixelIndices,TVector<T>& value) const=0 ;
/*!
  If possible return a pointer to the slice pixel data. return NULL otherwise 
*/
  virtual T*   GetThetaSliceDataPtr(int_4 sliceIndex) { return NULL; }
//! return the sum of all map pixel values 
  virtual T Sum() const = 0;
//! return the product of all map pixel values 
  virtual T Product() const = 0;
//! return the product of all map pixel (p) values-squared (Sum[ p^2 ])  
  virtual T SumSq() const = 0;
//  Norme^2 , identique a SumSq pour tableaux reels/integer , sum[el * conj(el)] pour complexe
//! return the minumum and maximum of all map pixel values (can not be applied to maps with complex data) 
  virtual void MinMax(T& min, T& max) const = 0;


protected :
  void UpdateInfo(const SphericalMap<T>& a);
  SphereCoordSys cs_;   //!< Spherical coordinate system used in the map
  mutable DVList* mInfo_;        //!< Infos (pairs of (key,value)) associated with the map

  static int_4 prt_lev_;  //!< Print level
};

//! overloading of of operator << : the Show() method will be called on stream s 
template <class T>
inline std::ostream& operator << (std::ostream& s,  const SphericalMap<T> & a)
{ a.Show(s);   return(s); }


// --- implementation de SetPixels()
template <class T>
T SphericalMap<T>::SetPixels(T v)
{
int k;
for(k=0; k<NbPixels(); k++) PixVal(k) = v;
return(v);
}

// --- implementation de Info()
template <class T>
DVList& SphericalMap<T>::Info()  const
{
  if (mInfo_ == NULL)  mInfo_ = new DVList;
  return(*mInfo_);
}

// --- implementation de Show()
template <class T>
void SphericalMap<T>::Show(std::ostream& os) const
{
os << "SphericalMap<T>::Show() class: " << typeid(*this).name() 
   << " T= " << DataTypeInfo<T>::getTypeName() 
   << " TypeOfMap= " << TypeOfMap() << std::endl;
double solang = 0.;
//DBG cout << " **** DBG **** SphericalMap<T>::Show() - NbPixels()="<< NbPixels() << endl;
if (NbPixels() > 0) solang = PixSolAngle(NbPixels()/2);
double frac = NbPixels()*solang*100/(4.*M_PI);
if (frac > 100.) frac = 100.;
os << " NbPixels()= " << NbPixels() << " ResolArcMin ~=" 
   << Angle(sqrt(solang)).ToArcMin() << " SphereFrac ~= " 
   << frac << " % of 4Pi" << std::endl;
}


template <class T>
void SphericalMap<T>::UpdateInfo(const SphericalMap<T>& a)
{
  if (mInfo_) { delete mInfo_;  mInfo_=NULL; }
  if (a.mInfo_) { mInfo_ = new DVList(*(a.mInfo_)); }
}


} // Fin du namespace




#endif






