//-----------------------------------------------------------
// GenericFunction --- ClassFunc1D/2D/3D ...
// SOPHYA class library - (C)  UPS+LAL IN2P3/CNRS  , CEA-Irfu 
// Abstract definition for functions viewed as classes
// Original version:   S.Plaszczynski 29/11/02
// updated/extended : R. Ansari  June 2013, Sep 2019 
//-----------------------------------------------------------

#include "classfunc.h"
#include <typeinfo>
#include <iostream>

using namespace std;

namespace SOPHYA {

//--- Implementation de  GFuncWrapper  
static double _GFW_zero1_(double x) { return 0.; }
static double _GFW_zero2_(double x, double y) { return 0.; }
static double _GFW_zero3_(double x, double y, double z) { return 0.; }


/*! 
  \brief constructor /default constructor : the wrapper holds the provided GenericFunction pointer 

  if the NULL pointer is given for gf, the object will hold the default function f_Zero() , returning 0. 
  if adel==true , the destructor deletes the function pointer 
*/
GFuncWrapper::GFuncWrapper(GenericFunction * gf, bool adel)
  :gfp_(NULL), adel_(adel)
{
  Init(gf);
}

/*!  \brief constructor from a GenericFunction reference 

  tries to clone gf by calling CloneGenFunc(), will hold f_Zero() function returning 0 if CloneGenFunc() fails ...
*/
GFuncWrapper::GFuncWrapper(GenericFunction const & gf)
  :gfp_(NULL), adel_(true)
{
  GenericFunction* gfp=NULL;
  try {
    gfp = gf.CloneGenFunc();
    Init(gfp);
  }
  catch (NotAvailableOperation & exc) {
    Init(NULL);
  }
}

/* --Methode-- */
void GFuncWrapper::Init(GenericFunction * gf)
{
  //DBG  cout << "*DBG* GFuncWrapper::Init(gf="<<hex<<gf<<")"<<endl;
  if (gf == NULL) {
    gfp_ = new Function1D(_GFW_zero1_);   adel_=true; 
  }
  else gfp_ = gf;
  ClassFunc1D* f1p = dynamic_cast< ClassFunc1D * >(gfp_);
  if (f1p == NULL) {
    f1dp_ = new Function1D(_GFW_zero1_);   adel1_=true; 
  }
  else { f1dp_ = f1p; adel1_=false;  }
  ClassFunc2D* f2p = dynamic_cast< ClassFunc2D * >(gfp_);
  if (f2p == NULL) {
    f2dp_ = new Function2D(_GFW_zero2_);   adel2_=true; 
  }
  else { f2dp_ = f2p; adel2_=false;  }
  ClassFunc3D* f3p = dynamic_cast< ClassFunc3D * >(gfp_);
  if (f3p == NULL) {
    f3dp_ = new Function3D(_GFW_zero3_);   adel3_=true; 
  }
  else { f3dp_ = f3p; adel3_=false;  }
  //DBG  cout << "*DBG* Init()... gfp_"<<hex<<gfp_<<" f1dp_="<<hex<<f1dp_<<"  f2dp_="<<hex<<f2dp_<<" f3dp_="<<hex<<f3dp_<<dec<<endl;
}

/* --Methode-- */
GFuncWrapper::~GFuncWrapper()
{
  if (gfp_ && adel_) delete gfp_; 
  if (f1dp_ && adel1_) delete f1dp_; 
  if (f2dp_ && adel2_) delete f2dp_; 
  if (f3dp_ && adel3_) delete f3dp_; 
}

}    /* --Fin de namespace SOPHYA-- */ 
