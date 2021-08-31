/* ------------------------------------------------------------------------------------
  SOPHYA class library - (C)  UPS+LAL IN2P3/CNRS  , CEA-Irfu 
  Ancillary classes for  Image<T> class - class ImgCoordTransform
  Jan 2015 - R. Ansari, J.E.Campagne, C. Magneville
        Updated Jan 2018 : introduction of CDij matrix, EulerRotation, 
	limited WCS-FITS support 
  ------------------------------------------------------------------------------------ */

#include "machdefs.h"
#include "pexceptions.h"
#include "imgcoordtr.h"
#include "unitvector.h"
#include <sstream>
#include <iomanip>

using namespace std;

namespace SOPHYA {

/*!
   \class ImgCoordTransform
   \ingroup SkyMap

   - Image coordinate system : cartesian (ximg, yimg) 
   - Local coordinate system (LCS) : cartesian (x,y) 
   - World coordinate system (WCS) : spherical LongitudeLatitude 
   
   Jan 2019 , SkyMap V 2.1 , partial implementation of initialisation from FITS - WCS  
     see http://www.atnf.csiro.au/people/mcalabre/WCS/  

   The easiest way to use this class would be to initialize it from FITS header WCS keywords, 
   as illustrated in the example below.

   \code
   string filename="myimage.fits";
   FitsInOutFile is(filename, FitsInOutFile::Fits_RO);
   DVList dvlh;
   is.GetHeaderRecords(dvlh);
   ImgCoordTransform ctr;
   ctr.InitFromWCSFITSKeywords(dvlh);
   cout << ctr;
   double pixcrd1[3]={0.,300.,300.};
   double pixcrd2[3]={0.,300.,500.};
   LongitudeLatitude ll;
   for (int i=0; i<3; i++) {
     ctr.ImageCoordToWCS(pixcrd1[i], pixcrd2[i], ll);
     cout<<" ImageCoordToWCS["<<i<<"] "<<pixcrd1[i]<<" , "<<pixcrd2[i]<<" -> LongLat="<<ll<<endl; 
   }
   \endcode
*/


/* --Methode-- */
ImgCoordTransform::ImgCoordTransform()
  :wcsid_(WCS_Unknown), wcsrsid_(WCS_RS_Unknown), wc_radecorg_(), rot_()
{
  fgset_wcsref_=false;
  for(int i=0; i<2; i++) {
    img_org_[i]=0.; 
    img_scale_[i]=1.;
    lc_org_[i]=0.;
  }
  lc_scale_=1.;
  alpha_=0.;  fginvx_=fginvy_=false;
  mx_CD_[0]=mx_CD_[3]=1.;
  mx_CD_[1]=mx_CD_[2]=0.;
  mx_CD_inv_[0]=mx_CD_inv_[3]=1.;
  mx_CD_inv_[1]=mx_CD_inv_[2]=0.;
  fgset_CD_=false;
}

/* --Methode-- */
ImgCoordTransform::~ImgCoordTransform()
{
}

/* --Methode-- */
void ImgCoordTransform::CopyFrom(ImgCoordTransform const& a)
{
  wcsid_=a.wcsid_; 
  wcsrsid_=a.wcsrsid_; 
  wc_radecorg_=a.wc_radecorg_;
  fgset_wcsref_=a.fgset_wcsref_;
  for(int i=0; i<2; i++) {
    img_org_[i]=a.img_org_[i]; 
    img_scale_[i]=a.img_scale_[i];
    lc_org_[i]=a.lc_org_[i];
  }
  lc_scale_=a.lc_scale_;
  alpha_=a.alpha_;  fginvx_=a.fginvx_;   fginvy_=a.fginvy_;
  for(int i=0; i<4; i++) {
    mx_CD_[i]=a.mx_CD_[i];
    mx_CD_inv_[i]=a.mx_CD_inv_[i];
  }
  fgset_CD_=a.fgset_CD_;
  rot_=a.rot_;
  return;
}

/* --Methode-- */
/*! The transformation parameter is initialized from the following FITS-WCS keywords, extracted from the DVList object.
  - CTYPE1 , CTYPE2   : Coordinate type and projection type - currently only TAN (Tangent plane projection supported)
  - RADESYS
  - CRPIX1 , CRPIX2 : Image plane reference pixel coordinate 
  - CUNIT1 , CUNIT2 : angular unit (rad,deg,arcmin,arcsec,mas) for CRVAL and CDELT and CDij  
  - CDELT1 , CDELT2 : angular extent of a pixel on the image 
  - CRVAL1 , CRVAL2 : WCS coordinate (sky coordinate) corresponding to the reference pixel 
  - CD1_1 , CD1_2 , CD2_1 , CD2_2  : CDij matrix
*/
void ImgCoordTransform::InitFromWCSFITSKeywords(DVList const & dvl)
{
  string key;

  const char * kctyp[2]={"CTYPE1","CTYPE2"};
  key=kctyp[0];  string sct1=(string)dvl[key];
  key=kctyp[1];  string sct2=(string)dvl[key];

  string sct1s=sct1.substr(0,4);  
  string sct2s=sct2.substr(0,4);  
  WCSId wcsid=WCS_Unknown;
  if ((sct1s == "RA--")&&(sct2s == "DEC-"))  wcsid=WCS_Equatorial;
  else if ((sct1s == "GLON")&&(sct2s == "GLAT"))  wcsid=WCS_Galactic;
  else if ((sct1s == "ELON")&&(sct2s == "ELAT"))  wcsid=WCS_Ecliptic;
  else if ((sct1s == "HLON")&&(sct2s == "HLAT"))  wcsid=WCS_HelioEcliptic;
  else if ((sct1s == "SLON")&&(sct2s == "SLAT"))  wcsid=WCS_SuperGalactic;
  else cout << "InitFromWCSFITSKeywords()/Warning unknown Coordinate system (WCS) ... -> WCS_Unknown"<<endl;
  sct1s=sct1.substr(4,4);   sct2s=sct2.substr(4,4); 
  if ((sct1s != "-TAN")||(sct2s == "-TAN"))  {
    cout << "InitFromWCSFITSKeywords()/Warning CTYPE1,2= "<< sct1 << " , " << sct2 
	 << "  projection type != -TAN -> Coord transformation results might be inaccurate or wrong !" << endl;
  }

  WCS_RS_Id wcsrsid=WCS_RS_Unknown;
  key="RADESYS";  sct1=(string)dvl[key];
  if (sct1=="ICRS")  wcsrsid_=WCS_RS_ICRS;
  else if (sct1=="FK5")  wcsrsid_=WCS_RS_FK5;
  else if (sct1=="FK4")  wcsrsid_=WCS_RS_FK4;
  else cout << "InitFromWCSFITSKeywords()/Warning unknown WCS reference/convention ("<<sct1<<") -> WCS_RS_Unknown"<<endl;
  setWCSId(wcsid, wcsrsid);

  const char * kcrpix[2]={"CRPIX1","CRPIX2"};
  double CRPix[2];
  int nk=0;
  for(int i=0; i<2; i++) {
    key=kcrpix[i];
    if (!dvl.HasKey(key))  continue;
    CRPix[i]=(double)dvl[key];
    nk++;
  }
  if (nk==2) {
    //DBG    cout << "InitFromWCSFITSKeywords*DBG* setImageReferencePoint("<<CRPix[0]<<","<<CRPix[1]<<") "<<endl;
    setImageReferencePoint(CRPix[0],CRPix[1]);
  }
  const char * kun[2]={"CUNIT1","CUNIT2"};
  double cangf[2]={1.,1.};   // angular conversion factor to radian 
  for(int i=0; i<2; i++) {
    key=kun[i];
    if (!dvl.HasKey(key))  continue;
    string sun=(string)dvl[key];
    if (sun.substr(0,3)=="rad")  cangf[i]=1.;  // radian -> default 
    else if (sun.substr(0,3)=="deg")  cangf[i]=Angle(1.,Angle::Degree).ToRadian();
    else if (sun.substr(0,6)=="arcmin")  cangf[i]=Angle(1.,Angle::ArcMin).ToRadian();
    else if (sun.substr(0,6)=="arcsec")  cangf[i]=Angle(1.,Angle::ArcSec).ToRadian();
    else if (sun.substr(0,3)=="mas")  cangf[i]=Angle(1.,Angle::ArcSec).ToRadian()*0.001;
    cout << " InitFromWCSFITSKeywords*DBG* sun="<<sun<<" cangf[i]="<<cangf[i]<<endl;
  }
  const char * kcdelt[2]={"CDELT1","CDELT2"};
  double CDELT[2]={1.,1.};
  nk=0;
  for(int i=0; i<2; i++) {
    key=kcdelt[i];
    if (!dvl.HasKey(key))  continue;
    CDELT[i]=((double)dvl[key])*cangf[i];
    nk++;
  }
  if (nk==2) {
    //DBG    cout << "InitFromWCSFITSKeywords*DBG* setImageScale("<<CDELT[0]<<","<<CDELT[1]<<") "<<endl;
    setImageScale(Angle(CDELT[0]),Angle(CDELT[1]));
  }
  const char * kcrval[2]={"CRVAL1","CRVAL2"};
  double CRVAL[2]={1.,1.};
  nk=0;
  for(int i=0; i<2; i++) {
    key=kcrval[i];
    if (!dvl.HasKey(key))  continue;
    CRVAL[i]=((double)dvl[key])*cangf[i];
    nk++;
  }
  if (nk==2) {
    //DBG    cout << "InitFromWCSFITSKeywords*DBG* setWCSReferencePoint("<<CRVAL[0]<<","<<CRVAL[1]<<") "<<endl;
    Angle alng(CRVAL[0]), alat(CRVAL[1]);
    LongitudeLatitude ll(alng, alat, true);
    setWCSReferencePoint(ll);
  }
  double CD[4]={1.,0.,0.,1.};
  int ncd=0;
  const char * kcd[4]={"CD1_1","CD1_2","CD2_1","CD2_2"};
  for(int i=0; i<4; i++) {
    key=kcd[i];
    if (!dvl.HasKey(key))  continue;
    CD[i]=((double)dvl[key])*cangf[i%2];
    ncd++;
  }
  if (ncd==4) {
    //DBG    cout << "InitFromWCSFITSKeywords*DBG* setProjectionCDMatrix()"<<endl;
    setProjectionCDMatrix(CD);
  }
  return;
}

/* --Methode-- */
/*! The following FITS-WCS keywords are append to the DVList object, with values from the ImgCoordTransform object
  - CTYPE1 , CTYPE2   : Coordinate type and projection type - currently only TAN (Tangent plane projection supported)
  - RADESYS
  - CRPIX1 , CRPIX2 : Image plane reference pixel coordinate 
  - CUNIT1 , CUNIT2 : angular unit (rad,deg,arcmin,arcsec,mas) for CRVAL and CDELT and CDij  
  - CDELT1 , CDELT2 : angular extent of a pixel on the image 
  - CRVAL1 , CRVAL2 : WCS coordinate (sky coordinate) corresponding to the reference pixel 
  - CD1_1 , CD1_2 , CD2_1 , CD2_2  : CDij matrix
*/

void ImgCoordTransform::FillWCSFITSKeywords(DVList & dvl) const 
{
  string key;

  string sct1, sct2;
  if (wcsid_ == WCS_Equatorial) { sct1="RA--"; sct2="DEC-"; }
  else if (wcsid_ == WCS_Galactic) { sct1="GLON"; sct2="GLAT"; }
  else if (wcsid_ == WCS_Ecliptic) { sct1="ELON"; sct2="ELAT"; }
  else if (wcsid_ == WCS_HelioEcliptic) { sct1="HLON"; sct2="HLAT"; }
  else if (wcsid_ == WCS_SuperGalactic) { sct1="SLON"; sct2="SLAT"; }
  sct1 += "-TAN";  sct2 += "-TAN";
  const char * kctyp[2]={"CTYPE1","CTYPE2"};
  key=kctyp[0];  dvl[key]=sct1;
  key=kctyp[1];  dvl[key]=sct2;
  sct1="";
  if (wcsrsid_ ==  WCS_RS_ICRS)  sct1="ICRS";
  else if (wcsrsid_ ==  WCS_RS_FK5)  sct1="FK5";
  else if (wcsrsid_ ==  WCS_RS_FK4)  sct1="FK4";
  key="RADESYS";   dvl[key]=sct1;

  const char * kun[2]={"CUNIT1","CUNIT2"};
  for(int i=0; i<2; i++) {
    key=kun[i];
    dvl[key]="deg     ";
  }
  const char * kcrpix[2]={"CRPIX1","CRPIX2"};
  for(int i=0; i<2; i++) {
    key=kcrpix[i];
    dvl[key]=img_org_[i];
  }
  const char * kcrval[2]={"CRVAL1","CRVAL2"};
  for(int i=0; i<2; i++) {
    key=kcrval[i];
    if (i==0)  dvl[key]=wc_radecorg_.Longitude().ToDegree(); 
    else dvl[key]=wc_radecorg_.Latitude().ToDegree(); 
  }
    const char * kcd[4]={"CD1_1","CD1_2","CD2_1","CD2_2"};
  for(int i=0; i<4; i++) {
    key=kcd[i];
    dvl[key]=mx_CD_[i]*180./M_PI;  // conversion to degrees
  }
  return;
}

/* --Methode-- */
void ImgCoordTransform::setWCSReferencePoint(LongitudeLatitude const& radec0)
{
 wc_radecorg_=radec0;
 fgset_wcsref_=true;
  /* Original frame (R): WCS spherical coord system 
     Rotated frame (RR) : the local angular coord system with the Oz axis corresponding to the projection pole 
     going through the image reference point 
  */
  double theta=wc_radecorg_.Theta(); 
  double phi=wc_radecorg_.Phi(); 
  double rphi=phi+Angle::PioTwoCst(); 
  if (rphi>Angle::TwoPiCst()) rphi-=Angle::TwoPiCst();
  double rtet=theta;
  cout << "setWCSReferencePoint*DBG* radec0="<<radec0<<" -> rphi="<<rphi<<" rtet="<<rtet<<endl;
  EulerRotation3D euler(rphi, rtet, 0.);
  //DEL  EulerRotation3D euler(phi, theta, 0.);
  rot_=euler;
  cout << rot_;
}

/* --Methode-- */
void ImgCoordTransform::setReferencePoint(double ximg0, double yimg0, LongitudeLatitude const& radec0)
{
  img_org_[0]=ximg0; img_org_[1]=yimg0;  
  setWCSReferencePoint(radec0);
}

/* --Methode-- */
void ImgCoordTransform::setImageScale(Angle xscale, Angle yscale)
{
  img_scale_[0]=xscale.ToRadian();  img_scale_[1]=yscale.ToRadian();
  setRotationSymmetryMatrix(Angle(alpha_), fginvx_, fginvy_);
  return;
}

/* --Methode-- */
void ImgCoordTransform::setRotationSymmetryMatrix(Angle alpha, bool fginvx, bool fginvy)
{
  double alp=alpha.ToRadian();
  double ca=cos(alp);
  double sa=sin(alp);
  double mx_rotsym[4];
  mx_rotsym[0]=ca;   mx_rotsym[1]=-sa;
  mx_rotsym[2]=sa;   mx_rotsym[3]=ca; 
  mx_rotsym[0]*=img_scale_[0];   mx_rotsym[1]*=img_scale_[1];
  mx_rotsym[2]*=img_scale_[0];   mx_rotsym[3]*=img_scale_[1];
  if (fginvx) {
    mx_rotsym[0]=-mx_rotsym[0];   
    mx_rotsym[2]=-mx_rotsym[2];
  }
  if (fginvy) {
    mx_rotsym[1]=-mx_rotsym[1];   
    mx_rotsym[3]=-mx_rotsym[3];
  }
  setProjectionCDMatrix(mx_rotsym);
  alpha_=alp;  fginvx_=fginvx;  fginvy_=fginvy;
  fgset_CD_=false;
  return;
}

/* --Methode-- */
void ImgCoordTransform::setProjectionCDMatrix(double * CDij)
{
  mx_CD_[0]=CDij[0];   mx_CD_[1]=CDij[1];
  mx_CD_[2]=CDij[2];   mx_CD_[3]=CDij[3];
  double det=mx_CD_[0]*mx_CD_[3]-mx_CD_[1]*mx_CD_[2];
  if (fabs(det)<1.e-19)  throw SingMatrixExc("ImgCoordTransform::setProjectionCDMatrix(CDij) Singular CDij matrix");
  det = 1./det;
  mx_CD_inv_[0]=mx_CD_[3]*det;   mx_CD_inv_[1]=-mx_CD_[1]*det; 
  mx_CD_inv_[2]=-mx_CD_[2]*det;  mx_CD_inv_[3]=mx_CD_[0]*det;  
  fgset_CD_ = true;
}

/* --Methode-- */
void ImgCoordTransform::ImageCoordToWCS(double ximg, double yimg, LongitudeLatitude& radec) const
{
  double dxy[2], da[2];
  //  dxy[0]=(ximg-img_org_[0])*img_scale_[0];
  //  dxy[1]=(yimg-img_org_[1])*img_scale_[1];
  dxy[0]=(ximg-img_org_[0]);
  dxy[1]=(yimg-img_org_[1]);
  da[0]=mx_CD_[0]*dxy[0]+mx_CD_[1]*dxy[1];
  da[1]=mx_CD_[2]*dxy[0]+mx_CD_[3]*dxy[1];
  // le calcul suivant est pour le moment approximatif , il faudra tenir compte du mode de projection 
  /*  double theta=sqrt(da[0]*da[0]+da[1]*da[1]);
  if (theta>1.)  
    throw MathExc("ImgCoordTransform::ImageCoordToWCS() computed theta=sqrt(da[0]^2+da[1]^2) > 1");  */
  double theta=atan(sqrt(da[0]*da[0]+da[1]*da[1]));
  double phi=atan2(da[1],da[0]);
  // here are the coordinates in the rotated (RR) frame corresponding to the local projection around the reference point
  UnitVector uv(LongitudeLatitude(theta,phi));
  // We rotate back to the original (R) frame which corresponds to the WCS (Sky) coordinates 
  Vector3d ruv = rot_.RotateBack(uv);
  //DBG  cout << " ImageCoordToWCS()*DBG*  da[0]="<<da[0]<<" da[1]="<<da[1]<<" theta="<<theta<<" phi="<<phi
  //     <<" UV="<<uv<<" -> RUV="<<ruv<<endl;
  radec = LongitudeLatitude(ruv);
  /*  // $CHECK$  ATTENTION : il faut gerer les passages par 0/Pi/2Pi 
  double lng=wc_radecorg_.Longitude().ToRadian()+da[0]/sin(wc_radecorg_.Theta());
  double lat=wc_radecorg_.Latitude().ToRadian()+da[1];
  radec.SetLongLat(Angle(lng,Angle::Radian), Angle(lat,Angle::Radian));  */
  return;
}

/* --Methode-- */
void ImgCoordTransform::WCSToImageCoord(LongitudeLatitude const& radec, double & ximg, double & yimg) const
{
  UnitVector uv(radec);
  Vector3d ruv = rot_.RotateBack(uv);
  LongitudeLatitude ll(ruv);
  double dxy[2], da[2];
  double r=tan(ll.Theta());
  double phi=ll.Phi();
  da[0]=r*cos(phi);
  da[1]=r*sin(phi);
  /* //  $CHECK$  ATTENTION : il faut gerer les passages par 0/Pi/2Pi 
  da[0]=(radec.Longitude().ToRadian()-wc_radecorg_.Longitude().ToRadian())*sin(wc_radecorg_.Theta());
  da[1]=radec.Latitude().ToRadian()-wc_radecorg_.Latitude().ToRadian();  */
  dxy[0]=mx_CD_inv_[0]*da[0]+mx_CD_inv_[1]*da[1];
  dxy[1]=mx_CD_inv_[2]*da[0]+mx_CD_inv_[3]*da[1];  
  ximg=dxy[0]+img_org_[0];
  yimg=dxy[1]+img_org_[1];
  /* ximg=(dxy[0]/img_scale_[0])+img_org_[0];
     yimg=(dxy[1]/img_scale_[1])+img_org_[1]; */
  return;  
}

/* --Methode-- */
void ImgCoordTransform::ImageCoordToLCS(double ximg, double yimg, double& x, double& y) const
{
  double dxy[2], da[2];
  dxy[0]=(ximg-img_org_[0]);
  dxy[1]=(yimg-img_org_[1]);
  da[0]=mx_CD_[0]*dxy[0]+mx_CD_[1]*dxy[1];
  da[1]=mx_CD_[2]*dxy[0]+mx_CD_[3]*dxy[1];
  x=da[0]*lc_scale_+lc_org_[0];
  y=da[1]*lc_scale_+lc_org_[1];
  return;
}

/* --Methode-- */
void ImgCoordTransform::LCSToImageCoord(double x, double y, double & ximg, double & yimg) const
{
  double dxy[2], da[2];
  da[0]=(x-lc_org_[0])/lc_scale_;
  da[1]=(y-lc_org_[1])/lc_scale_;
  dxy[0]=mx_CD_inv_[0]*da[0]+mx_CD_inv_[1]*da[1];
  dxy[1]=mx_CD_inv_[2]*da[0]+mx_CD_inv_[3]*da[1];  
  ximg=dxy[0]+img_org_[0];
  yimg=dxy[1]+img_org_[1];
  return;
}

/* --Methode-- */
ostream& ImgCoordTransform::Print(ostream& os)  const
{
  os <<"ImgCoordTransform: WCSId="<<WCSIdToString(wcsid_)<<"  WCS_RS_Id="<<WCS_RS_IdToString(wcsrsid_)<<"\n"
     <<" Img Org="<<img_org_[0]<<","<<img_org_[1]<<" Scale="<<img_scale_[0]<<","<<img_scale_[1]<<"\n"
     <<" LC Org="<<lc_org_[0]<<","<<lc_org_[1]<<" Scale="<<lc_scale_<<" WCS Org="<<wc_radecorg_<<"\n"
     <<" Mx_CD[ [ "<<mx_CD_[0]<<","<<mx_CD_[1]<<" ] ; [ "<<mx_CD_[2]<<","<<mx_CD_[3]<<" ] ]\n"
     <<" Mx_CDInv[ [ "<<mx_CD_inv_[0]<<","<<mx_CD_inv_[1]
     <<" ] ; [ "<<mx_CD_inv_[2]<<","<<mx_CD_inv_[3]<<" ] ]"<<endl;
  return os;
}

//----------------------------------------------------------
// Classe pour la gestion de persistance de ImgCoordTransform
// ObjFileIO<ImgCoordTransform>
//----------------------------------------------------------

/* --Methode-- */
DECL_TEMP_SPEC  /* equivalent a template <> , pour SGI-CC en particulier */
void        ObjFileIO<ImgCoordTransform>::WriteSelf(POutPersist& s) const
{
  if (dobj == NULL)
    throw NullPtrError("ObjFileIO<ImgCoordTransform>::WriteSelf() dobj=NULL");
  int_4 ver;
  ver = 2;
  s.Put(ver);   // ecriture numero de version PPF
  uint_2 wcsid = WCSIdToInteger(dobj->wcsid_);
  uint_2 wcsrsid = WCS_RS_IdToInteger(dobj->wcsrsid_);
  uint_2 flags=0;
  if (dobj->fgset_CD_)  flags |= 1;
  if (dobj->fginvx_)  flags |= 2;
  if (dobj->fginvy_)  flags |= 4;
  if (dobj->fgset_wcsref_)  flags |= 8;
  uint_2 tabui2[5]={0,0,0,0,0};
  tabui2[0]=wcsid;
  tabui2[1]=wcsrsid;
  tabui2[2]=flags;
  s.PutU2s(tabui2,5);

  r_8 dbuff[16];
  for(int i=0; i<2; i++) {
    dbuff[i]=dobj->img_org_[i];
    dbuff[2+i]=dobj->img_scale_[i];
    dbuff[4+i]=dobj->lc_org_[i];
  }
  dbuff[6]=dobj->lc_scale_;
  dbuff[7]=dobj->wc_radecorg_.Theta();
  dbuff[8]=dobj->wc_radecorg_.Phi();
  dbuff[9]=0.;
  s.PutR8s(dbuff,10);
  for(int i=0; i<4; i++) {
    dbuff[i]=dobj->mx_CD_[i];
    dbuff[4+i]=dobj->mx_CD_inv_[i];
  }
  dbuff[8]=dobj->alpha_;  dbuff[9]=0.;
  s.PutR8s(dbuff,10);
}

/* --Methode-- */
DECL_TEMP_SPEC  /* equivalent a template <> , pour SGI-CC en particulier */
void        ObjFileIO<ImgCoordTransform>::ReadSelf(PInPersist& s)
{
  int_4 ver;
  s.Get(ver);   // Lecture numero de version PPF
  if (ver<2)  throw IOExc("ObjFileIO<ImgCoordTransform>::ReadSelf() reading V=1 NOT supported ");
  uint_2 tabui2[5]={0,0,0,0,0};
  s.GetU2s(tabui2,5);
  uint_2 wcsid = tabui2[0];
  uint_2 wcsrsid = tabui2[1];
  uint_2 flags = tabui2[2];  
  if (dobj==NULL)  dobj=new ImgCoordTransform;

  dobj->wcsid_=IntegerToWCSId(wcsid);
  dobj->wcsrsid_=IntegerToWCS_RS_Id(wcsrsid);

  if (flags&1) dobj->fgset_CD_=true; 
  if (flags&2) dobj->fginvx_=true;   
  if (flags&4) dobj->fginvy_=true;   
  if (flags&8) dobj->fgset_wcsref_=true;
  r_8 dbuff[16];
  s.GetR8s(dbuff,10);
  for(int i=0; i<2; i++) {
    dobj->img_org_[i]=dbuff[i];
    dobj->img_scale_[i]=dbuff[2+i];
    dobj->lc_org_[i]=dbuff[4+i];
  }
  dobj->lc_scale_=dbuff[6];
  dobj->wc_radecorg_.SetThetaPhi(dbuff[7], dbuff[8]);
  s.GetR8s(dbuff,10);
  for(int i=0; i<4; i++) {
    dobj->mx_CD_[i]=dbuff[i];
    dobj->mx_CD_inv_[i]=dbuff[4+i];
  }
  dobj->alpha_=dbuff[8];
}

#ifdef __CXX_PRAGMA_TEMPLATES__
#pragma define_template ObjFileIO<ImgCoordTransform>
#endif

#if defined(ANSI_TEMPLATES) || defined(GNU_TEMPLATES)
template class ObjFileIO<ImgCoordTransform>;
#endif

} // FIN namespace SOPHYA 
