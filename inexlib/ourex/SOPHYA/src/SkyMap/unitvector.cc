//------------------------------------------------------------------------------------
// class UnitVector  
// SOPHYA class library - (C)  UPS+LAL IN2P3/CNRS  , (C) CEA-Irfu-SPP 
//   Utility functions for spherical trigonometry 
// B. Revenu , G. Le Meur    2000  ,  R. Ansari 2006, 2015  
//------------------------------------------------------------------------------------

//   3-D Geometry - classe UnitVector 
//      B. Revenu , G. Le Meur  2000
//      R. Ansari 2006, 2015 
// LAL (Orsay) / IN2P3-CNRS  IRFU/SPP (CEA)

#include <math.h>
#include "unitvector.h"

using namespace std;

namespace SOPHYA {

/*!
   \class UnitVector
   \ingroup SkyMap
   \brief Specialisation of Vector3d class for representing unit (length=1) 3-vectors. 

   \code
   UnitVector ux=UnitVector::ux();
   UnitVector uy=UnitVector::uy();
   UnitVector uz = ux^uy;
   UnitVector ua(ux+uy);
   UnitVector ub(ux+uy+uz);
   cout << "ua:"<<ua<<" ub:"<<ub<<endl;
   cout << "ux^uy(=?uz) "<<uz<<" ua.ux:"<<ua*ux<<" ua^uz:"<<(ua^uz)<<endl;
   cout << "ux.ur: "<<ux.ur()<<" ux.utheta: "<<ux.utheta()<<" ux.uphi: "<<ux.uphi()<<endl;
   UnitVector uphi=ux.ur()^ux.utheta();
   cout << "ux.ur^ux.utheta: "<<uphi<<" ==ux.uphi() ? "<<((uphi==ux.uphi())?"true":"false")<<endl;
   cout << "ua.utheta: "<<ua.utheta()<<" ua.uphi: "<<ua.uphi()<<endl;
   \endcode
*/


// Constructor: Unit vector along the x,y,z cartesian coordinates
UnitVector::UnitVector(double x, double y, double z) : Vector3d(x,y,z) 
{
  this->NormalizeSelf();
}

//  constructor from a Vector3d object 
UnitVector::UnitVector(const Vector3d& v) : Vector3d(v) 
{
  this->NormalizeSelf();
}
//! print the vector on stream os
std::ostream& UnitVector::Print(std::ostream& os, bool fgnn) const 
{
  os << "UnitVector(X=" << _x << ",Y=" << _y << ",Z=" << _z << ")"; 
  if (fgnn) os << endl;
  return os;

}

} // FIN namespace SOPHYA 
