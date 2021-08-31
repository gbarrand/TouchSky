/*
  ------------------------------------------------------------------------------------
  SOPHYA class library - (C)  UPS+LAL IN2P3/CNRS  , CEA-Irfu 
  Ancillary classes for  Image<T> class
  Jan 2015 - R. Ansari, J.E.Campagne, C. Magneville
  ------------------------------------------------------------------------------------
*/

#include "machdefs.h"
#include "imgphotband.h"
#include <sstream>
#include <iomanip>

using namespace std;

namespace SOPHYA {

/*!
   \class ImgPhotometricBand
   \ingroup SkyMap
*/

/*!
   Subset of standard photometric bands defined through the central wavelength (lambda0), 
   and bandwidth (Delta-lambda) - See the reference below for more information.

   \verbatim 
  --------------------------------------------------  
  FilterId      lambda0 (A)    Delta-lambda (A)
  -------------------------------------------------- 
                    Johnson-Cousin 
  --------------------------------------------------  
  U_Filter         3663  A         650   A
  B_Filter         4361  A         890   A
  V_Filter         5448  A         840   A
  R_Filter         6407  A         1580  A
  I_Filter         7980  A         1540  A
  --------------------------------------------------  

   Michael S. Bessell , STANDARD PHOTOMETRIC SYSTEMS
   Annual Review of Astronomy and Astrophysics
   Vol. 43: 293-336 (Volume publication date August 2005)
   DOI: 10.1146/annurev.astro.41.082801.100251
  \endverbatim
*/
/* --Methode-- */
ImgPhotometricBand::ImgPhotometricBand(StandardPhotometricFilterId fid) 
{
  // Johnson-Cousins central wavelength in Angstrom 
  double jc_lambda0[5]={3663., 4361., 5448., 6407., 7980.};
  // Johnson-Cousins wavelength width in Angstrom 
  double jc_delta_lambda[5]={650., 890., 840., 1580., 1540.};

  PhysQty lambda(Units::meter().micro(), 1.);
  double relbw=0.1; 
  double maxtr=1.; 
  PassBandType pbt=kSquarePassBand;
  
  switch (fid) {
  case NonStandardFilter:
    pbt=kSquarePassBand;
    break;
    //  Johnson-Cousins filters 
  case U_Filter:
    lambda.setValue(jc_lambda0[0]/1.e4);
    relbw=jc_delta_lambda[0]/jc_lambda0[0];
    break;
  case B_Filter:
    lambda.setValue(jc_lambda0[1]/1.e4);
    relbw=jc_delta_lambda[1]/jc_lambda0[1];
    break;
  case V_Filter:
    lambda.setValue(jc_lambda0[2]/1.e4);
    relbw=jc_delta_lambda[2]/jc_lambda0[2];
    break;
  case R_Filter:
    lambda.setValue(jc_lambda0[3]/1.e4);
    relbw=jc_delta_lambda[3]/jc_lambda0[3];
    break;
  case I_Filter:
    lambda.setValue(jc_lambda0[4]/1.e4);
    relbw=jc_delta_lambda[4]/jc_lambda0[4];
    break;
  default:
    break;
  }
  defineFilter(fid, lambda, relbw, maxtr, pbt);
}

/* --Methode-- */
ImgPhotometricBand::ImgPhotometricBand(PhysQty const& lambda_nu_0, double relbandw, double mxtrans, PassBandType typ)
{
  StandardPhotometricFilterId fid=NonStandardFilter;
  if (lambda_nu_0.getUnit().isSameDimension(Units::meter()))  
    defineFilter(fid, lambda_nu_0, relbandw, mxtrans, typ);
  else {
    if (lambda_nu_0.getUnit().isSameDimension(Units::hertz())) {
      PhysQty lambda0 = PhysQty::SpeedofLight()/lambda_nu_0;
      defineFilter(fid, lambda0, relbandw, mxtrans, typ);
    }
    else {
      ParmError("ImgPhotometricBand::ImgPhotometricBand(lambda_nu_0 ...): lambda_nu_0 not a wavelength or frequency");
    }
  }
}

/* --Methode-- */

/* --Methode-- 
ImgPhotometricBand::~ImgPhotometricBand()
{
}
*/

/* --Methode-- */
string ImgPhotometricBand::getFilterName()  const 
{
  switch (stdfilterid_) {
  case NonStandardFilter:
    return "NonStandardFilter";
    break;
    //  Johnson-Cousins filters 
  case U_Filter:
    return "U_Filter";
    break;
  case B_Filter:
    return "B_Filter";
    break;
  case V_Filter:
    return "V_Filter";
    break;
  case R_Filter:
    return "R_Filter";
    break;
  case I_Filter:
    return "I_Filter";
    break;
  default:
    return "????_Filter";
    break;
  }
  return "";
}

/* --Methode-- */
ostream& ImgPhotometricBand::Print(ostream& os) const 
{
  os << "ImgPhotometricBand["<<getFilterName()<<" , lambda0="<<lambda0_<<" relBW="<<relbandwidth_<<" MaxTr="<<maxtrans_<<"]";
  return os;
}


//----------------------------------------------------------
// Classe pour la gestion de persistance de ImgPhotometricBand
// ObjFileIO<ImgPhotometricBand>
//----------------------------------------------------------

/* --Methode-- */
DECL_TEMP_SPEC  /* equivalent a template <> , pour SGI-CC en particulier */
void        ObjFileIO<ImgPhotometricBand>::WriteSelf(POutPersist& s) const
{
  if (dobj == NULL)
    throw NullPtrError("ObjFileIO<ImgPhotometricBand>::WriteSelf() dobj=NULL");
  int_4 ver;
  ver = 1;
  s.Put(ver);   // ecriture numero de version PPF
  uint_2 fid = dobj->stdfilterid_;
  uint_2 pbt = dobj->pbtype_;
  s << fid << dobj->lambda0_ << dobj->relbandwidth_ << dobj->maxtrans_ << pbt;
}

/* --Methode-- */
DECL_TEMP_SPEC  /* equivalent a template <> , pour SGI-CC en particulier */
void        ObjFileIO<ImgPhotometricBand>::ReadSelf(PInPersist& s)
{
  int_4 ver;
  s.Get(ver);   // Lecture numero de version PPF
  uint_2 fid;
  uint_2 pbt;
  s >> fid >> dobj->lambda0_ >> dobj->relbandwidth_ >> dobj->maxtrans_ >> pbt;
  dobj->stdfilterid_ = ImgPhotometricBand::StandardPhotometricFilterId(fid);
  dobj->pbtype_ = ImgPhotometricBand::PassBandType(pbt);
}

/* --Methode-- */

#ifdef __CXX_PRAGMA_TEMPLATES__
#pragma define_template ObjFileIO<ImgPhotometricBand>
#endif

#if defined(ANSI_TEMPLATES) || defined(GNU_TEMPLATES)
template class ObjFileIO<ImgPhotometricBand>;
#endif

} // FIN namespace SOPHYA 
