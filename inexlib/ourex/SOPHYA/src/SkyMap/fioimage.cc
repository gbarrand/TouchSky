/* ------------------------------------------------------------------------------------
   SOPHYA class library - (C)  UPS+LAL IN2P3/CNRS  , CEA-Irfu 
   PPF handler for classe Image<T> 
   Initial version:   R.Ansari, C.Magneville 07/2000  
   Major redesign: Jan 2015 
   R. Ansari, J.E.Campagne, C. Magneville
   ------------------------------------------------------------------------------------ */

#include "datatype.h"
#include "fioimage.h"

using namespace std;

namespace SOPHYA {

/*!
  \class FIO_Image
  \ingroup SkyMap 

  PPF handler for Image<T> class
 */

//! Default constructor
template <class T>
FIO_Image<T>::FIO_Image()
  : FIO_TArray<T>()
{
}


//! Constructor from the file \b filename
template <class T>
FIO_Image<T>::FIO_Image(string const & filename) 
  : FIO_TArray<T>(filename) 
{
}

//! Constructor from the Image \b obj
template <class T>
FIO_Image<T>::FIO_Image(const Image<T> & obj) 
  : FIO_TArray<T>()
{ 
  this->dobj = new Image<T>(obj, true);
  this->ownobj=true; 
}

//! Connect with a Image \b obj
template <class T>
FIO_Image<T>::FIO_Image(Image<T> * obj) 
  : FIO_TArray<T>(obj)
{ 
}


//! Connect Image \b o
template <class T>
void FIO_Image<T>::SetDataObj(AnyDataObj & o)
{
  Image<T> * po = dynamic_cast< Image<T> * >(&o);
  if (po == NULL) {
    char buff[160];
    sprintf(buff,"FIO_Image<%s>::SetDataObj(%s) - Object type  error ! ",
	    DataTypeInfo<T>::getTypeName().c_str(), typeid(o).name());
    throw TypeMismatchExc(PExcLongMessage(buff));        
  }
  if (this->ownobj && this->dobj) delete this->dobj;
  this->dobj = po; this->ownobj = false;
} 

template <class T>
void FIO_Image<T>::ReadSelf(PInPersist& is)
{
  if (this->dobj == NULL) this->dobj = new Image<T>;
  Image<T> * img = dynamic_cast<Image<T> * > (this->dobj);
  if (img == NULL)  throw TypeMismatchExc("FIO_Image<T>::ReadSelf() dobj is NOT an Image<T> !");
//  On lit 3 uint_4 .... 
//  itab[0] : numero de version , 
//  itab[1] = flags , bit 0 = fgtrset_ , bit 1 = fgsrset_   
//  itab[2] : reserve 
  uint_4 itab[3];
  is.Get(itab,3);
// V < 2 pas pris en charge 
  if (itab[0]<2) throw IOExc("FIO_Image<T>::ReadSelf V=1 not readable");
  // Reading the Image<T> specific attribute part 
  img->fgtrset_ = (itab[1] & 1); 
  img->fgsrset_ = (itab[1] & 2); 
  is >> img->coordtr_;
  is >> img->sensresp_;
// Reading the TArray part 
  FIO_TArray<T>::ReadSelf(is);
}

template <class T>
void FIO_Image<T>::WriteSelf(POutPersist& os) const
{
  if (this->dobj == NULL)   return;
  Image<T> * img = dynamic_cast<Image<T> * > (this->dobj);
  if (img == NULL)  throw TypeMismatchExc("FIO_Image<T>::WriteSelf() dobj is NOT an Image<T> !");

//  On ecrit 3 uint_4 .... 
//  itab[0] : numero de version , 
//  itab[1] = flags , bit 0 = fgtrset_ , bit 1 = fgsrset_   
//  itab[2] : reserve 
  uint_4 itab[3];
  itab[0] = 2;  // Numero de version a 2  (en Jan 2015)
  itab[1] = 0; 
  itab[2] = 0; 
  if (img->fgtrset_)  itab[1] |= 1; 
  if (img->fgsrset_)  itab[1] |= 2; 
  os.Put(itab,3);
// Writing the Image<T> specific attribute part 
  os << img->coordtr_;
  os << img->sensresp_;
// Writing the TArray part 
  FIO_TArray<T>::WriteSelf(os);
}


///////////////////////////////////////////////////////////////
#ifdef __CXX_PRAGMA_TEMPLATES__
#pragma define_template FIO_Image<uint_1>
#pragma define_template FIO_Image<uint_2>
#pragma define_template FIO_Image<uint_4>
#pragma define_template FIO_Image<uint_8>
#pragma define_template FIO_Image<int_1>
#pragma define_template FIO_Image<int_2>
#pragma define_template FIO_Image<int_4>
#pragma define_template FIO_Image<int_8>
#pragma define_template FIO_Image<r_4>
#pragma define_template FIO_Image<r_8>
#pragma define_template FIO_Image< complex<r_4> >
#pragma define_template FIO_Image< complex<r_8> >
#ifdef SO_LDBLE128
#pragma define_template FIO_Image<r_16>
#pragma define_template FIO_Image< complex<r_16> >
#endif
#endif

#if defined(ANSI_TEMPLATES) || defined(GNU_TEMPLATES)
template class FIO_Image<uint_1>;
template class FIO_Image<uint_2>;
template class FIO_Image<uint_4>;
template class FIO_Image<uint_8>;
template class FIO_Image<int_1>;
template class FIO_Image<int_2>;
template class FIO_Image<int_4>;
template class FIO_Image<int_8>;
template class FIO_Image<r_4>;
template class FIO_Image<r_8>;
template class FIO_Image< complex<r_4> >;
template class FIO_Image< complex<r_8> >;
#ifdef SO_LDBLE128
template class FIO_Image<r_16>;
template class FIO_Image< complex<r_16> >;
#endif
#endif

} // FIN namespace SOPHYA 
