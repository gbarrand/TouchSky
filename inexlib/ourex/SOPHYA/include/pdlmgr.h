//-----------------------------------------------------------
// Classe PDynLinkMgr
// Gestionnaire de lien dynamique   -  
// SOPHYA class library - (C)  UPS+LAL IN2P3/CNRS  , CEA-Irfu 
// R. Ansari          (C) UPS+LAL IN2P3/CNRS  1998 
//-----------------------------------------------------------

#ifndef PDYNLINKMGR_SEEN
#define PDYNLINKMGR_SEEN

#include "machdefs.h"
#include <string>


#if defined(HPUX)
#include <dl.h>
/* Les fonction NSCreateObjectFile...() sont deprecated - Il faut utiliser les dlopen()...
   qui sont plus efficaces a partir de 10.4  - On se cale donc sur les versions de GCC */
#elif defined(Darwin) && ( GCC_VERSION <= 3350 )
#include <mach-o/dyld.h>
#else
#include <dlfcn.h>
#endif

namespace SOPHYA {

typedef void (* DlFunction) (void);

//! Dynamic Link Manager. 
class PDynLinkMgr {
public:
  // ---- Methodes globales (static)
  static PDynLinkMgr*   BuildFromCFile(std::string const & fname);  // Compil+link fichier C
  static void		SetTmpDir(std::string const & path);        // Repertoire des fichiers temporaires

  // ---- Constructeur et methodes 
  // Constructeur a partir d'un shared lib - copie en zone temporaire si cp==true
			PDynLinkMgr(std::string& soname, bool cp=true);  
  virtual		~PDynLinkMgr();

  virtual DlFunction	GetFunction(std::string const & funcname);              
  inline  DlFunction	GetFunction(const char * funcname) 
    { std::string fname = funcname;   return GetFunction(fname); }            

  inline  std::string        GetName() const { return soName; }

protected :
  static std::string&	GetTmpDir();
  static int	  numSO;
  static std::string*  tmpDir;

#if defined(HPUX)
  shl_t dlhandle;
#elif defined(Darwin) && ( GCC_VERSION <= 3350 )
  NSObjectFileImage nsobjfile;
  NSModule nsmod;
#else 
  void * dlhandle;
#endif
  bool dylok;
  std::string soName;
  bool copy;
};

} // namespace SOPHYA

#endif
