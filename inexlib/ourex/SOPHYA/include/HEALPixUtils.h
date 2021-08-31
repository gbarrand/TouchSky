//------------------------------------------------------------------------------------
// SOPHYA class library - (C)  UPS+LAL IN2P3/CNRS , (C) CEA-Irfu-SPP
// Utility functions for HEALPix pixlisation  
// Benoit Revenu, Guy Le Meur  (1998)   minor updates R. Ansari 2015   
//------------------------------------------------------------------------------------

#ifndef HEALPixUtils_SEEN
#define HEALPixUtils_SEEN
#include "machdefs.h"


namespace SOPHYA {

/*!
   \class HEALPixUtils
   \ingroup SkyMap 

   \brief Utility class to compute index to/from angle conversion in HEALPix spherical map pixelisation
*/


class HEALPixUtils {
public :
  //! conversion from NESTED to RING pixel index
  static int_4  nest2ring(int_4 nside,int_4 ipnest) ;
  //! conversion from RING to NESTED pixel index
  static int_4  ring2nest(int_4 nside,int_4 ipring) ;
  //! return the pixel index corresponding to the direction defined by the two angles \b theta and \b phi in RING scheme  
  static int_4  ang2pix_ring(int_4 nside,double theta,double phi) ;
  //! return the pixel index corresponding to the direction defined by the two angles \b theta and \b phi in NESTED scheme  
  static int_4  ang2pix_nest(int_4 nside,double theta,double phi) ;
  //! return the direction corresponding the pixel \b ipix in the RING scheme  
  static void   pix2ang_ring(int_4 nside,int_4 ipix,double& theta,double& phi) ;
  //! return the direction corresponding the pixel \b ipix in the NESTED scheme  
  static void   pix2ang_nest(int_4 nside,int_4 ipix,double& theta,double& phi) ;

  // return the size index nside corresponding to the resolution res 
  static int_4  ResolToSizeIndex(double res);  
  static inline int_4  ResolToNSide(double res) 
  { return   ResolToSizeIndex(res); }
  // return the pixel resolution for a given HEALPix size index (nside) 
  static double SizeIndexToResol(int_4 m);  
  static inline double  NSideToResol(int_4 m) 
  { return  SizeIndexToResol(m); }
};



} // Fin du namespace

#endif
