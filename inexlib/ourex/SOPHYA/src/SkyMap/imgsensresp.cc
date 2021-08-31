/*
  ------------------------------------------------------------------------------------
  SOPHYA class library - (C)  UPS+LAL IN2P3/CNRS  , CEA-Irfu 
  Ancillary classes for  Image<T> class , class ImgSensorResponse
  Jan 2015 - R. Ansari, J.E.Campagne, C. Magneville
  ------------------------------------------------------------------------------------
*/

#include "machdefs.h"
#include "imgsensresp.h"

using namespace std;

namespace SOPHYA {

/*!
   \class ImgSensorResponse
   \ingroup SkyMap

   Image sensor response, extensing the ImgPhotometricBand class, adding 
   the sensor output dynamic range, gain and read-out noise charactersitics. 
*/

/* --Methode-- */
ImgSensorResponse::ImgSensorResponse()
  : ImgPhotometricBand() , minADU_(-9.e99), maxADU_(9.e99), offset_(0.), gain_(1.), ronoise_(0.), qe_(1.) 
{
}

/* --Methode-- */
ImgSensorResponse::ImgSensorResponse(double minADU, double maxADU, double offset, double gain, double ronoise)
  : ImgPhotometricBand() , minADU_(minADU), maxADU_(maxADU), offset_(offset), gain_(gain), ronoise_(ronoise), qe_(1.) 
{
}



/* --Methode-- */
ostream& ImgSensorResponse::Print(ostream& os) const 
{
  os << "ImgSensorResponse[min,maxADU="<<minADU_<<","<<maxADU_<<" Offset="<<offset_<<" Gain="<<gain_
     << " RONoise="<<ronoise_<<" QE="<<qe_<<"\n"
     << " ...PhotBand:"<<getFilterName()<<" , lambda0="<<lambda0_<<" relBW="<<relbandwidth_
     <<" MaxTr="<<maxtrans_<<"]"<<endl;
  return os;
}


//----------------------------------------------------------
// Classe pour la gestion de persistance de ImgPhotometricBand
// ObjFileIO<ImgPhotometricBand>
//----------------------------------------------------------

/* --Methode-- */
DECL_TEMP_SPEC  /* equivalent a template <> , pour SGI-CC en particulier */
void        ObjFileIO<ImgSensorResponse>::WriteSelf(POutPersist& s) const
{
  if (dobj == NULL)
    throw NullPtrError("ObjFileIO<ImgSensorResponse>::WriteSelf() dobj=NULL");
  int_4 ver;
  ver = 1;
  s.Put(ver);   // ecriture numero de version PPF
  // Ecriture de la partie ImgPhotometricBand
  uint_2 fid = dobj->stdfilterid_;
  uint_2 pbt = dobj->pbtype_;
  s << fid << dobj->lambda0_ << dobj->relbandwidth_ << dobj->maxtrans_ << pbt;
  // Ecriture de la partie additionnelle ImgSensorResponse
  s << dobj->minADU_ << dobj->maxADU_ << dobj->offset_ << dobj->ronoise_ << dobj->qe_; 
}

/* --Methode-- */
DECL_TEMP_SPEC  /* equivalent a template <> , pour SGI-CC en particulier */
void        ObjFileIO<ImgSensorResponse>::ReadSelf(PInPersist& s)
{
  int_4 ver;
  s.Get(ver);   // Lecture numero de version PPF
  // Lecture de la partie ImgPhotometricBand
  uint_2 fid;
  uint_2 pbt;
  s >> fid >> dobj->lambda0_ >> dobj->relbandwidth_ >> dobj->maxtrans_ >> pbt;
  dobj->stdfilterid_ = ImgPhotometricBand::StandardPhotometricFilterId(fid);
  dobj->pbtype_ = ImgPhotometricBand::PassBandType(pbt);
  // Lecture de la partie additionnelle ImgSensorResponse
  s >> dobj->minADU_ >> dobj->maxADU_ >> dobj->offset_ >> dobj->ronoise_ >> dobj->qe_; 
}

/* --Methode-- */

#ifdef __CXX_PRAGMA_TEMPLATES__
#pragma define_template ObjFileIO<ImgSensorResponse>
#endif

#if defined(ANSI_TEMPLATES) || defined(GNU_TEMPLATES)
template class ObjFileIO<ImgSensorResponse>;
#endif

} // FIN namespace SOPHYA 
