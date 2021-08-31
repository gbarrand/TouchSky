//   SOPHYA class library - (C)  UPS+LAL IN2P3/CNRS  , (C) CEA-Irfu-SPP 
// Classes SphereCoordSys , SpherePosition
//      G. Le Meur  2000
//      R. Ansari 2006 (documentation/commentaire) 
// LAL (Orsay) / IN2P3-CNRS  DAPNIA/SPP (Saclay) / CEA

// 04/01/00 : implantation de la persistance par classe deleguee - Guy Le Meur

#include "spherepos.h"
#include "datatype.h"
#include <typeinfo>


//................. SphereCoordSys class .................

/*!
   \class SOPHYA::SphereCoordSys
   \ingroup SkyMap
   \brief  Class which describes the coordinate system used in spherical maps
   Current implementation (~ 2006 ) does NOT perform any coordinate transformation

   \sa SphericalMap
*/

using namespace std;

namespace SOPHYA {

/* --Methode-- */
SphereCoordSys::SphereCoordSys(WCSId wid)
  : wcsid_(wid), description_(), fgrotated_(false), euler_()
{
}

/* --Methode-- */
SphereCoordSys::SphereCoordSys(const SphereCoordSys& a )
  : wcsid_(a.wcsid_), description_(a.description_), fgrotated_(a.fgrotated_), euler_(a.euler_)
{
}

/* --Methode-- */
SphereCoordSys& SphereCoordSys::operator = (const SphereCoordSys& a)
{
  wcsid_=a.wcsid_;
  description_=a.description_;
  fgrotated_=a.fgrotated_;
  euler_=a.euler_;
  return *this;
}

/* --Methode-- */
std::ostream& SphereCoordSys::Print(std::ostream& os) const
{
  os << " SphereCoordSys("<< WCSIdToString( wcsid_ ) << " ) - "<<description_;
  if (fgrotated_) os << " [Rotated]";
  os << endl;
  if (fgrotated_) os << euler_;
  return os;
}

////////////////////////////////////////////////////////////////
// -------------------------------------------------------------------------
//   Les objets delegues pour la gestion de persistance 
// -------------------------------------------------------------------------

FIO_SphereCoordSys::FIO_SphereCoordSys()
  {
    dobj=new SphereCoordSys;
    ownobj=true;
  }
FIO_SphereCoordSys::FIO_SphereCoordSys(string const & filename)
  {
    dobj=new SphereCoordSys;
    ownobj=true; 
    Read(filename);
  }
FIO_SphereCoordSys::FIO_SphereCoordSys(const SphereCoordSys & obj)
  {
    dobj = new SphereCoordSys(obj);
    ownobj=true; 
  }
FIO_SphereCoordSys::FIO_SphereCoordSys(SphereCoordSys * obj)
  {
    dobj = obj;
    ownobj=false; 
  }
FIO_SphereCoordSys::~FIO_SphereCoordSys()
  {
    if (ownobj && dobj) delete dobj;
  }
AnyDataObj* FIO_SphereCoordSys::DataObj()
  {
    return(dobj);
  }

void  FIO_SphereCoordSys::SetDataObj(AnyDataObj & o) 
{
  SphereCoordSys * po = dynamic_cast<SphereCoordSys *>(&o);
  if (po == NULL) {
    char buff[160];
    sprintf(buff,"FIO_SphereCoordSys::SetDataObj(%s) - Object type  error ! ",
	    typeid(o).name());
    throw TypeMismatchExc(PExcLongMessage(buff));    
  }

  if (ownobj && dobj) delete dobj;  
  dobj = po; ownobj = false;
}


void  FIO_SphereCoordSys::ReadSelf(PInPersist& is)
{
  uint_8 itab[3];
  string description;
  is.Get(itab, 3);
  is.GetStr(description);
  WCSId wcsid=WCS_Unknown;
  EulerRotation3D erot;
  bool fgrot=false;
  if (itab[0]<2) {  // lecture ancienne version - on cree un objet par defaut 
    cerr << "FIO_SphereCoordSys::ReadSelf()/Warning : reading old PPF V=1 version -> default SphereCoordSys()"<<endl;
  }
  else {
    wcsid=IntegerToWCSId(itab[2]);
    if (itab[1] & 1) {
      fgrot=true;
      is >> erot;
    }
  }
  if (dobj == NULL) dobj = new SphereCoordSys(wcsid);
  else *dobj= SphereCoordSys(wcsid);
  dobj->setDescription(description);
  if (fgrot)  dobj->RotateFrame(erot);
  return;
}
           
void       FIO_SphereCoordSys::WriteSelf(POutPersist& os) const
{
  if (dobj == NULL)   return;  // Attention - $CHECK$ Guy 04/01/00
  //  On ecrit 3 uint_8 
  //  0 : Numero de version,  1: flags (each bit is a flag)  2: WCSId  
  //  itab[1] : Bit 0 set -> is rotated 
  uint_8 itab[3];
  itab[0] = 2;  // numero de version = 2 (Juin 2015) 
  itab[1] = 0;  // bit-array (flags) 
  if (dobj->isRotated())  itab[1] |= 1;
  itab[2] = WCSIdToInteger(dobj->getWCSId());  // WCSId 
  os.Put(itab, 3);
  os.PutStr(dobj->getDescription());
  if (dobj->isRotated())  {
    EulerRotation3D erot=dobj->getRotation();
    os << erot;
  }
  return;
} 

} // FIN namespace SOPHYA 




