/* ------------------------------------------------------------------------------------
  SOPHYA class library - (C)  UPS+LAL IN2P3/CNRS , (C) CEA-Irfu-SPP
  Theta-Phi spherical pixelisation base class 
  R. Ansari 2015 
  ------------------------------------------------------------------------------------ */

#include <math.h>

#include "machdefs.h"
#include "pexceptions.h"
#include "sphthetaphibase.h"

using namespace std;

namespace SOPHYA {

//---------------------------------------------------------------------------------
//------------------------- Implementation de SphThetaPhiBase -------------------------
//---------------------------------------------------------------------------------

/*!
   \class SphThetaPhiBase
   \ingroup SkyMap 

   \brief base class implementing Theta-Phi pixelisation (IGLOO like)

   In Theta-Phi pixelisation, the sphere is divided into \b NTheta iso-latitude 
   (same theta) rings. Each ring is divided into pixels corresponding to 
   equi-distant meridians. The first and last ring (north and south polar caps) 
   have a single pixel each. If number of rings or slices (NTheta) is odd, 
   there is one ring (NTheta/2+1) centered on the equator (theta=pi/2).
   If it is even, then the equator separates the two rings  (NTheta/2) and 
   (NTheta/2+1). 
   DeltaTheta = Pi / (NTheta-1) :  
   There are two pixels, one centered on each pole. These pixels have a radius in
   theta (ThetaPole) corresponding to half of the other rings extension in theta. 
   The pixels at the pole have thus a diameter equal to other pixels size in theta:
   DeltaTheta=2 ThetaPole

   Two variants of pixelisation are available through this class. 
   * The first one which corresponds more or less to the original SphereThetaPhi 
   introduced in Sophya in 2000, divides the equatorial ring into 2*(NTheta-1) pixels.
   The number of pixels along other rings proportional to 2*NTheta*cos(theta), 
   and a single pixel on each pole. This ensures pixels with aspect ratio 
   close to 1 (square pixels), and nearly equal area for all pixels, except near 
   the poles.
   * The second variant, the rings are divided into a number pixels multiple of 3 
   and pixel area are kept close to the polar cap pixel. The choice of the second 
   variant is controlled by the  FgBy3_  (to be set to true)
 
   The schematic views below gives the pixel indexing for few values of NTheta.
\verbatim
          < FgBy3=false >                         < FgBy3=true >                               
* NTheta = 3  : sphere divided into three rings, 6 pixels, 8 pixels
 
              0            <- NorthPole ->              0                         
        ------------         (theta=pi/3)        --------------
         1  2  3  4       <-  Equator  ->      1  2  3  4  5  6 
        ------------         (theta=2pi/3)       --------------
              5            <- SouthPole ->              7

* NTheta = 4  : sphere divided into 4 rings, 14 pixels, 14 pixels  
               0            <- NorthPole ->             0                         
          -----------        (theta=pi/4)        --------------
        1  2   ...    6                         1  2  3  4  5  6
      -------------------   <-  Equator  ->   -------------------
        7 8  ...     12                         7  8  9  ...  12
          -----------        (theta=3pi/4)       -------------
               13           <- SouthPole ->            13

\endverbatim

  For a given NTheta, map pixel resolution would be ~180/NTheta degrees, 
  approximate resolution in arcmin given below for few values of NTheta:
  128 -> 85' ; 256 -> 42' ; 512 -> 21' ; 1024 -> 10.5' ; 2048 -> 5' ; 4096 -> 2.5' ; 10800 -> 1' 

*/

/*-- Methode --*/
SphThetaPhiBase::SphThetaPhiBase(bool fgby3) 
  : SphericalPixelisationBase(SPix_ThetaPhi),
    FgBy3_(fgby3), NTheta_(0), NPix_(0), DeltaTheta_(0.), Omega_(0.), Resol_(0.), CNphi_() 
{
}

void SphThetaPhiBase::copyFrom(SphThetaPhiBase const& a)
{
  FgBy3_=a.FgBy3_;  NTheta_=a.NTheta_;  NPix_=a.NPix_;  
  DeltaTheta_=a.DeltaTheta_; ThetaPole_=a.ThetaPole_; Omega_=a.Omega_;  Resol_=a.Resol_;
  CNphi_.Clone(a.CNphi_);
}

/*!
  can throw ParmError exception if \b ntheta is invalid  

  \param ntheta : number of rings (or slices along theta)  3 <= ntheta <= 1048576
*/

/*-- Methode --*/
void SphThetaPhiBase::setNbRings(int_4 ntheta)
{
  if (ntheta<2 || ntheta>1048576) {
    cerr << "SphThetaPhiBase::setNbThetaSlices() ntheta=" << ntheta <<" out of range [2,1048576]" << endl;
    throw ParmError("SphThetaPhiBase::setNbThetaSlices() out of range nside");
  }
  NTheta_ = ntheta;  //  nb of slices 
  NDataBlock<int_4>  Nphi(NTheta_);
  DeltaTheta_=Angle::OnePiCst()/(double)(ntheta-1);
  ThetaPole_=0.5*DeltaTheta_;
  CNphi_.ReSize(NTheta_+1, false);
  if (ntheta==2) {  // sphere decoupee en 2 hemispheres 
    Nphi(0)=Nphi(1)=1;
  }
  else if (ntheta==3) {  // 3 rings 
    Nphi(0)=Nphi(2)=1;
    if (FgBy3_) Nphi(1)=6;
    else Nphi(1)=4;
  }
  else if (ntheta==4) {  // 4 rings 
    Nphi(0)=Nphi(3)=1;
    if (FgBy3_) Nphi(1)=Nphi(2)=6;
    else Nphi(1)=Nphi(2)=6;
  }
  else if (ntheta==5) {  // 5 rings 
    Nphi(0)=Nphi(4)=1;
    if (FgBy3_) { Nphi(1)=Nphi(3)=6; Nphi(2)=9; } 
    else { Nphi(1)=Nphi(3)=4;  Nphi(2)=8; } 
  }
  else {   // more than 5 rings 
    double dtheta = DeltaTheta_;
    //UNUSED    double omegpole=1.-cos(dtheta);
    Nphi(0)=Nphi(ntheta-1)=1;
    size_t kmx=ntheta/2; 
    if (ntheta%2 != 0)  kmx++;
    if (FgBy3_) {  // nombre de pixels multiple de 6 // de 3
      Nphi(1)=Nphi(ntheta-2)=6;
      //      Nphi(2)=Nphi(ntheta-3)=12;
      double omeg6=cos(0.5*dtheta)-cos(1.5*dtheta);
      for(size_t k=2; k<kmx; k++) {
	int nphi = (6.*(cos(((double)k-0.5)*dtheta)-cos(((double)k+0.5)*dtheta))/omeg6)+0.5;
	if ((nphi%3)<2) nphi=(nphi/3)*3;
	else nphi=(nphi/3)*3+3;
	Nphi(k)=Nphi(ntheta-k-1)=nphi;
      }  
    }  
    else {  // non multiple de 3 
      for(size_t k=1; k<kmx; k++) {
	int nphi = 2*(ntheta-1)*sin((double)k*dtheta);
	if (nphi<1) nphi=1;
	Nphi(k)=Nphi(ntheta-k-1)=nphi;
      }
    }
  }
  CNphi_(0)=0;
  for(size_t k=0; k<(size_t)ntheta; k++) {
    CNphi_(k+1)=CNphi_(k)+Nphi(k);
  }
  NPix_=CNphi_(ntheta);
  // solid angle per pixel   
  Omega_= 4.0*Angle::OnePiCst()/NPix_;
  Resol_=Angle(sqrt(Omega_));
  //DBG  cout << "***DBG*** SphThetaPhiBase::setNbRings() NTheta="<<NTheta_<<" NPix="<<NPix_<<(FgBy3_?" FgBy3":"")<<endl;
  //DBG for(int i=0; i<NTheta_; i++) cout << getRingNbPixels(i) << " ; ";  cout << endl;
  return;
}

/*-- Methode --*/
/*!
  This method calls  setNbRings() and can throw ParmError exception if \b m is invalid  

  \param m : number of rings (or slices along theta)  3 <= m <= 1048576
*/
void SphThetaPhiBase::setPixelisationParameter(int_4 m)
{
  setNbRings(m);
  return;
}

/*-- Methode --*/
int_4 SphThetaPhiBase::getPixelisationParameter() const
{
  return NTheta_; 
}

/*-- Methode --*/
size_t SphThetaPhiBase::getNbPixels() const
{
  return NPix_;
}

/*-- Methode --*/
int_8 SphThetaPhiBase::getIndex(LongitudeLatitude const& ll) const
{
  int_4 kt = ((ll.Theta()+ThetaPole_)/DeltaTheta_);
  if (kt == (NTheta_))  kt=NTheta_-1;
  if ((kt<0) || (kt>NTheta_))  { 
    //DBG    cerr << " SphThetaPhiBase::getIndex(LongitudeLatitude(ll="<<ll<<") -> theta="<<ll.Theta()<<" -> kt="<<kt<<endl;
    throw RangeCheckError("SphThetaPhiBase::getIndex(ll) - out of range theta, not in [0,Pi]");
  }
  int_4 nphi=CNphi_(kt+1)-CNphi_(kt);
  double dphi=Angle::TwoPiCst()/(double)(nphi);
  int_4 jp = ll.Phi()/dphi;
  if (jp==nphi)  jp=nphi-1;
  if ((jp<0)||(jp>nphi))   throw RangeCheckError("SphThetaPhiBase::getIndex(ll) - out of range [phi, not in [0,2Pi]");
  return (CNphi_(kt)+(int_8)jp);
}


/*-- Methode --*/
LongitudeLatitude SphThetaPhiBase::getLongLat(int_8 idx) const
{
  if (idx==0)  return LongitudeLatitude(0.,0.);
  else if (idx==(int_8)NPix_-1)  return LongitudeLatitude(Angle::OnePiCst(),0.);
  int_4 kt, jp;
  getThetaPhiIndex(idx, kt, jp);
  int_4 nphi=CNphi_(kt+1)-CNphi_(kt);
  double phi=Angle::TwoPiCst()/(double)(nphi)*((double)(jp)+0.5);
  double theta=((double)kt)*DeltaTheta_;
  //DBG  cout << "***DBG*** SphThetaPhiBase::getLongLat(idx="<<idx<<" -> kt,jp="<<kt<<','<<jp<<" nphi="<<nphi<<" phi="<<phi<< "theta="<<theta<<endl;
  return LongitudeLatitude(theta, phi); 
}

/*-- Methode --*/
double SphThetaPhiBase::getPixSolAngle(int_8 idx) const
{
  if ((idx==0)||(idx==(int_8)NPix_-1))  return(Angle::TwoPiCst()*(1.-cos(ThetaPole_)));
  int_4 kt, jp;
  getThetaPhiIndex(idx, kt, jp);
  double theta=((double)kt-0.5)*DeltaTheta_;
  double domega=Angle::TwoPiCst()*(cos(theta)-cos(theta+DeltaTheta_));
  //DBG  cout << "***DBG*** SphThetaPhiBase::getPixSolAngle(idx="<<idx<<" -> kt,jp="<<kt<<','<<jp<<" domega="<<domega<<" CNphi:"
  //DBG     <<CNphi_(kt)<<","<<CNphi_(kt+1)<<endl;
  return (domega/(double)(CNphi_(kt+1)-CNphi_(kt))); 
}

/*-- Methode --*/
Angle SphThetaPhiBase::getAngularResolution() const 
{
  return Resol_; 
}

/*-- Methode --*/
int_4 SphThetaPhiBase::getIndex(int_4 kt, int_4 jp) const
{
  if ((kt<0)||(kt>=NTheta_))  throw RangeCheckError("SphThetaPhiBase::getIndex(kt,jp) - out of range kt");
  int_4 nphi=CNphi_(kt+1)-CNphi_(kt);
  if ((jp<0)||(jp>=nphi))  throw RangeCheckError("SphThetaPhiBase::getIndex(kt,jp) - out of range jp");
  return CNphi_(kt)+jp;
}

/*-- Methode --*/
void SphThetaPhiBase::getThetaPhiIndex(int_8 idx, int_4& kt, int_4& jp) const
{
  if ((idx<0)||(idx>=(int_8)NPix_))  throw RangeCheckError("SphThetaPhiBase::getThetaPhiIndex(idx) idx out of range[0,NPix-1]");
  int_4 kt1=0, kt2=NTheta_;
  while (kt1 != (kt2-1))  {
    int_4 ktm = (kt1+kt2)/2;
    if (idx<CNphi_(ktm))  kt2=ktm;
    else kt1=ktm;
  }
  kt=kt1;
  jp=idx-CNphi_(kt);
}

/*-- Methode --*/
int_4 SphThetaPhiBase::getRingNbPixels(int_4 kt) const
{
  if ((kt<0)||(kt>=NTheta_))  throw RangeCheckError("SphThetaPhiBase::getRingNbPixels(kt) kt out of range[0,NTheta-1]");
  return ( CNphi_(kt+1)-CNphi_(kt) );
}


/*-- Methode --*/
double SphThetaPhiBase::getRingTheta(int_4 kt) const
{
  if ((kt<0)||(kt>=NTheta_))  throw RangeCheckError("SphThetaPhiBase::getRingTheta(kt,...) kt out of range[0,NTheta-1]");
  return  ((double)kt)*DeltaTheta_ ;
}

/*-- Methode --*/
int_4 SphThetaPhiBase::getRingBoundary(int_4 kt, double& th1, double& th2) const
{
  if ((kt<0)||(kt>=NTheta_))  throw RangeCheckError("SphThetaPhiBase::getRingBoundary(kt,...) kt out of range[0,NTheta-1]");
  if (kt==0)  { th1=0.; th2=ThetaPole_; }
  else if (kt==NTheta_-1) { th2=Angle::OnePiCst(); th1=th2-ThetaPole_; }
  else {
    th1=((double)kt-0.5)*DeltaTheta_;
    th2=th1+DeltaTheta_;
  }
  return ( CNphi_(kt+1)-CNphi_(kt) );
}

/*-- Methode --*/
void SphThetaPhiBase::getPixelBoundary(int_8 idx, double& th1, double& th2, double& phi1, double& phi2) const 
{
  if ((idx==0)||(idx==(int_8)NPix_-1))  {
    phi1=0.; phi2=Angle::TwoPiCst();
    if (idx==0)  { th1=0.; th2=ThetaPole_; }
    else { th2=Angle::OnePiCst(); th1=th2-ThetaPole_; }
    return;
  }
  int_4 kt, jp;
  getThetaPhiIndex(idx, kt, jp);
  th1=((double)kt)*DeltaTheta_;
  th2=th1+DeltaTheta_;
  int_4 nphi=CNphi_(kt+1)-CNphi_(kt);
  double dphi=Angle::TwoPiCst()/(double)(nphi);
  phi1=(double)jp*dphi;
  phi2=phi1+dphi;
  return;
}

/*-- Methode --*/
void SphThetaPhiBase::getThetaPhiBoundary(int_4 kt, int_4 jp, double& th1, double& th2, double& phi1, double& phi2) const 
{
  if ((kt<0)||(kt>=NTheta_))  throw RangeCheckError("SphThetaPhiBase::getThetaPhiBoundary(kt,jp...) kt out of range[0,NTheta-1]");
  int_4 nphi=CNphi_(kt+1)-CNphi_(kt);
  if ((jp<0)||(jp>=nphi))  throw RangeCheckError("SphThetaPhiBase::getThetaPhiBoundary(kt,jp...) jp out of range[0,NPhi[kt]-1]");
  if ((kt==0)||(kt==NTheta_-1)) {
    phi1=0.; phi2=Angle::TwoPiCst();
    if (kt==0)  { th1=0.; th2=ThetaPole_;  }
    else { th2=Angle::OnePiCst(); th1=th2-ThetaPole_; }
    return; 
  }
  th1=((double)kt)*DeltaTheta_;
  th2=th1+DeltaTheta_;
  double dphi=Angle::TwoPiCst()/(double)(nphi);
  phi1=(double)jp*dphi;
  phi2=phi1+dphi;
  return;
}

/*-- Methode --*/
int_4 SphThetaPhiBase::ResolutionToNTheta(double res)
{
  return Angle::OnePiCst()/res;
}

/*-- Methode --*/
double SphThetaPhiBase::NThetaToResolution(int_4 ntheta)
{
  return Angle::OnePiCst()/(double)ntheta;
}


} // Fin du namespace
