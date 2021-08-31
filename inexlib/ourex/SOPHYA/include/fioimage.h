//------------------------------------------------------------------------------------
// SOPHYA class library - (C)  UPS+LAL IN2P3/CNRS  , CEA-Irfu 
// PPF handler for classe Image<T> 
// Initial version:   R.Ansari, C.Magneville 07/2000  
// Major redesign: Jan 2015 
//                     R. Ansari, J.E.Campagne, C. Magneville
//------------------------------------------------------------------------------------
#ifndef FIOIMAGE_H_SEEN
#define FIOIMAGE_H_SEEN

#include "cimage.h"
#include "fioarr.h"

namespace SOPHYA {

//--------------------------------------------------------------------------------   
//------------------------------  FIO_Image class ---------------------------------
//--------------------------------------------------------------------------------   

//! Class for persistent management of Image
template <class T>
class FIO_Image : public FIO_TArray<T>   {
public:
  FIO_Image();
  FIO_Image(std::string const & filename); 
  FIO_Image(const Image<T> & obj);
  FIO_Image(Image<T> * obj);
  //  virtual ~FIO_Image();  
  virtual void        SetDataObj(AnyDataObj & o);
  inline operator Image<T>() { return(*(dynamic_cast<Image<T> * >(this->dobj))); }
protected :
  virtual void ReadSelf(PInPersist&);           
  virtual void WriteSelf(POutPersist&) const;  
};

/*! \ingroup SkyMap \fn operator<<(POutPersist&,Image<T>&)
  \brief Write Image \b obj into POutPersist stream \b os */
template <class T>
inline POutPersist& operator << (POutPersist& os, Image<T> & obj)
{ FIO_Image<T> fio(&obj);  fio.Write(os);  return(os); }

/*! \ingroup SkyMap \fn operator>>(PInPersist&,Image<T>&)
  \brief Read Image \b obj from PInPersist stream \b os */
template <class T>
inline PInPersist& operator >> (PInPersist& is, Image<T> & obj)
{ FIO_Image<T> fio(&obj);  fio.Read(is);  return(is); }

}  // End of namespace SOPHYA

#endif

