//-----------------------------------------------------------
// Classe FIO_SphereECP<T>
//   PPF handler for sphereECP<T>   
// SOPHYA class library - (C)  UPS+LAL IN2P3/CNRS  , CEA-Irfu 
// R. Ansari , Univ. Paris-Sud, LAL IN2P3/CNRS   2004 
//-----------------------------------------------------------

// Gestion de persistance PPF pour les sphereECP 
// R. Ansari, Septembre 2004 

#ifndef FIOSPHEREECP_SEEN
#define FIOSPHEREECP_SEEN

#include "sphereecp.h"
#include "ppersist.h"


namespace SOPHYA {

template <class T>
class FIO_SphereECP : public PPersist  
{
public:

FIO_SphereECP();
FIO_SphereECP(std::string const & filename); 
FIO_SphereECP(const SphereECP<T>& obj);
FIO_SphereECP(SphereECP<T>* obj);
virtual ~FIO_SphereECP();
virtual AnyDataObj* DataObj();
void SetDataObj(AnyDataObj & o);
inline operator SphereECP<T>() { return(*dobj); }

protected :

virtual void ReadSelf(PInPersist&);           
virtual void WriteSelf(POutPersist&) const;  
SphereECP<T>* dobj;
bool ownobj;
};

/*! \ingroup SkyMap \fn operator<<(POutPersist&,SphereECP<T>&)
  \brief Write SphereECP \b obj into POutPersist stream \b os */
template <class T>
inline POutPersist& operator << (POutPersist& os, SphereECP<T> & obj)
{ FIO_SphereECP<T> fio(&obj);  fio.Write(os);  return(os); }

/*! \ingroup SkyMap \fn operator>>(PInPersist&,SphereECP<T>&)
  \brief Read SphereECP \b obj from PInPersist stream \b os */
template <class T>
inline PInPersist& operator >> (PInPersist& is, SphereECP<T> & obj)
{ FIO_SphereECP<T> fio(&obj); is.SkipToNextObject(); fio.Read(is); return(is); }


} // Fin du namespace

#endif
