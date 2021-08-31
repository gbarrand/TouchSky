/* ------------------------------------------------------------------------------------
  SOPHYA class library - (C)  UPS+LAL IN2P3/CNRS , (C) CEA-Irfu-SPP
  HEALPix spherical pixelisation base class 
  R. Ansari 2015 
  ------------------------------------------------------------------------------------ */

#include <math.h>

#include "machdefs.h"
#include "pexceptions.h"

#include "HEALPixUtils.h"
#include "shealpixbase.h"

using namespace std;

namespace SOPHYA {

//---------------------------------------------------------------------------------
//------------------------- Implementation de HEALPixBase -------------------------
//---------------------------------------------------------------------------------

/*!
   \class HEALPixBase
   \ingroup SkyMap 

   \brief base class which can be used to construct spherical maps with HEALPix pixelisation. 

   The sphere is split in 12 diamond-faces containing each nside^2 pixels.
   The pixels indexing in the NESTED scheme is similar to to the quad-cube.
   In each face the first pixel is in the lowest corner of the diamond.

   In the RING scheme pixels are numbered along the parallels and 
   the first parallel is the one closest to the north pole. On each parallel, 
   pixels are numbered starting from the one closest to phi = 0 (0<=phi<2Pi).

   The pixelisation resolution and total number of pixels is defined by the \b NSide
   pixelisation parameter. Each diamond shaped face is divided into NSide^2 pixels:
   the total number of pixels on the sphere is then NPix = 12 x NSide^2

   The schematic view shows the twelve (12) faces and the (x,y) coordinate on each face:

\verbatim
        .   .   .   .   <--- North Pole
       / \ / \ / \ / \                          ^        ^     
      . 0 . 1 . 2 . 3 . <--- z = 2/3             \      / 
       \ / \ / \ / \ /                        y   \    /  x  
      4 . 5 . 6 . 7 . 4 <--- equator               \  /      
       / \ / \ / \ / \                              \/      
      . 8 . 9 .10 .11 . <--- z = -2/3              (0,0) : lowest corner 
       \ / \ / \ / \ /      
        .   .   .   .   <--- South Pole
\endverbatim

\end
*/

HEALPixBase::HEALPixBase(bool fgring)
: SphericalPixelisationBase(SPix_HEALPix) , fgring_(fgring) , nSide_(0), nPix_(0), omeg_(0.), resol_(0.)
{
}

/*!
  can throw ParmError exception if \b m is invalid  

  \param nside : nside should be a power of 2 ( nside = 2^p ) and 1 <= nside <= 8192   
  \param fgring : true -> RING scheme , false -> NESTED
*/

/*-- Methode --*/
void HEALPixBase::setNSide(int_4 nside, bool fgring)
{
  if (nside<=0 || nside>8192) {
    cerr << "HEALPixBase::setNSide() nside=" << nside <<" out of range [1,8192]" << endl;
    throw ParmError("HEALPixBase::setNSide() out of range nside");
  }
  fgring_=fgring;
  if (nside==nSide_)  return;
  // verifier que m est une puissance de deux 
  int_4 x=nside;
  while (x%2==0) x/=2;
  if(x != 1) {  
    cerr << "HEALPixBase::setNSide() nside" << nside << " != 2^p " << endl;
    throw ParmError("HEALPixBase::setNSide() nside not a power of 2");
  }
  nSide_=nside;
  nPix_= 12*nSide_*nSide_;
  // solid angle per pixel   
  omeg_= 4.0*Angle::OnePiCst()/nPix_;
  resol_=Angle(sqrt(omeg_));
  return;
}

/*-- Methode --*/
/*!
  This method calls setNSide() and can throw ParmError exception if \b m is invalid  

  \param m : m is the HEALPix NSide parameter, which should be a power of 2 ( m = 2^p ) and <= 8192   
*/
void HEALPixBase::setPixelisationParameter(int_4 m)
{
  setNSide(m);
  return;
}

/*-- Methode --*/
int_4 HEALPixBase::getPixelisationParameter() const
{
  return nSide_; 
}

/*-- Methode --*/
size_t HEALPixBase::getNbPixels() const
{
  return nPix_;
}

/*-- Methode --*/
int_8 HEALPixBase::getIndex(LongitudeLatitude const& ll) const
{
  if (fgring_) return HEALPixUtils::ang2pix_ring(nSide_, ll.Theta(), ll.Phi());
  else return HEALPixUtils::ang2pix_nest(nSide_, ll.Theta(), ll.Phi());
}

/*-- Methode --*/
LongitudeLatitude HEALPixBase::getLongLat(int_8 idx) const
{
  if ((idx<0)||(idx>=(int_8)nPix_))  throw RangeCheckError("HEALPixBase::getLongLat(idx) idx out of range[0,NPix-1]");
  double theta, phi;
  if (fgring_) HEALPixUtils::pix2ang_ring(nSide_,idx,theta,phi);
  else HEALPixUtils::pix2ang_nest(nSide_,idx,theta,phi);
  return LongitudeLatitude(theta, phi); 
}

/*-- Methode --*/
double HEALPixBase::getPixSolAngle(int_8 /*k*/) const
{
  return omeg_; 
}

/*-- Methode --*/
Angle HEALPixBase::getAngularResolution() const 
{
  return resol_; 
}

} // Fin du namespace
