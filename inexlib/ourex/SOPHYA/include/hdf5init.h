/*    
   --- SOPHYA software - HDF5IOServer module ---
   R. Ansari,   2018
   (C) UPS+LAL IN2P3/CNRS     (C) IRFU-SPP/CEA 

   Classe d'initialisation du module HDF5IOServer
*/

#ifndef HDF5IOSERVERINIT_H_SEEN
#define HDF5IOSERVERINIT_H_SEEN

#include "sophyainit.h"   // Classe d''initialisation pour les modules de SOPHYA

namespace SOPHYA {  

class HDF5IOServerInitiator : public SophyaInitiator {
private:
  static int FgInit;
public:
  			HDF5IOServerInitiator();
  virtual		~HDF5IOServerInitiator();
};

#define HDF5IOServerInit() HDF5IOServerInitiator hdf5iosrvinit 

double HDF5IOServer_Module_Version();
} // namespace SOPHYA

#endif    /* HDF5IOSERVERINIT_H_SEEN */

