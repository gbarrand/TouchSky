// This may look like C code, but it is really -*- C++ -*-
// Classe DVList : Liste dynamique de variables (valeurs) 
// identifiees par un nom    Reza Ansari - Dec 96

#ifndef DVLIST_H_SEEN
#define DVLIST_H_SEEN

#include "objfio.h"

#include <stdio.h>
#include <iostream>

#include "mutyv.h"
#include <list>
#include <map>

namespace SOPHYA {

//  Classe liste de variables  Dynamic Variable List  

//! Dynamic Variable List class. 
class DVList : public AnyDataObj {
public:

                    DVList();
                    DVList(const DVList&);
                    DVList(const char *flnm);

  virtual           ~DVList();

  DVList&           operator= (const DVList&);

  void              Clear();
  DVList&           Merge(const DVList&);

  //! Returns the number of elements (variables) in DVList object 
  inline size_t     Size() const { return(mvlist.size()); }
  //! Returns the number of elements (variables) in DVList object 
  inline size_t     NVar() const { return(mvlist.size()); }

  bool              GetB(std::string const& key, bool def=false) const;
  char              GetC(std::string const& key, char def='\0') const;
  int_8             GetI(std::string const& key, int_8 def=-1) const;
  r_8               GetD(std::string const& key, r_8 def=-9.e19) const;
  std::complex<r_8> GetZ(std::string const& key, std::complex<r_8> def=-9.e19) const;
  std::string       GetS(std::string const& key, const char* def="") const;
  TimeStamp         GetT(std::string const& key, TimeStamp def=TimeStamp()) const;

  std::string       GetComment(std::string const& key) const;

  bool              DeleteKey(std::string const& key);
  bool              HasKey(std::string const& key) const;

  void              SetB(std::string const& key, bool val);
  void              SetC(std::string const& key, char val);
  void              SetI(std::string const& key, int_8 val);
  void              SetD(std::string const& key, r_8 val);
  void              SetZ(std::string const& key, std::complex<r_8> val);
  void              SetS(std::string const& key, const char *  val);
  void              SetS(std::string const& key, std::string const& val);
  void              SetT(std::string const& key, TimeStamp const& val);
  void              SetComment(std::string const& key, std::string const& comm);

  MuTyV             Get(std::string const& key) const ;
  MuTyV&            Get(std::string const& key);
/*! Returns the value associated with the name \b key */
  inline MuTyV      operator()  (std::string const& key) const { return Get(key); }
/*! Returns the value associated with the name \b key */
  inline MuTyV      operator[]  (std::string const& key) const { return Get(key); }
/*! Returns the global comment string associated with the object */
  inline std::string   Comment() const { return(comment); }
/*! Returns the value associated with the name \b key */
  inline MuTyV&     operator()  (std::string const& key)  { return Get(key); }
/*! Returns the value associated with the name \b key */
  inline MuTyV&     operator[]  (std::string const& key)  { return Get(key); }
/*! Returns the global comment string associated with the object */
  inline std::string&  Comment() { return(comment); }

/*! Prints a brief description of object on \b cout */
  inline  void      Show() const { Show(std::cout); }
  virtual void      Show(std::ostream& os)  const;
/*! Prints the list of variables on \b cout */
  inline  void      Print() const  { Print(std::cout); }  
  virtual void      Print(std::ostream& os)  const;

//!  \cond  
// Chaque element dans un DVList est constitue desormais d'un MuTyV
// et d'une chaine de caracteres (commentaire) regroupe dans la structure
// dvlElement. Ces elements sont associes aux noms de variables dans un 
// map<...> ValList.            Reza 02/2000
  struct dvlElement {MuTyV elval; std::string elcomm; } ;
  typedef std::map<std::string, dvlElement, std::less<std::string> >  ValList; 
//!  \endcond
/*! Returns an iterator pointing on the first variable in the list */
  inline ValList::const_iterator Begin() const { return(mvlist.begin()); } 
/*! Returns the iterator end value */
  inline ValList::const_iterator End() const { return(mvlist.end()); } 
  

private:
  
  ValList mvlist;
  std::string comment;
};

/*! operator << overloading - Prints the list on the stream \b s */
inline std::ostream& operator << (std::ostream& s, DVList const & dvl)
  {  dvl.Print(s);  return(s);  }

/*! Writes the object in the POutPersist stream \b os */
inline POutPersist& operator << (POutPersist& os, DVList & obj)
{ ObjFileIO<DVList> fio(&obj);  fio.Write(os);  return(os); }
/*! Reads the object from the PInPersist stream \b is */
inline PInPersist& operator >> (PInPersist& is, DVList & obj)
{ ObjFileIO<DVList> fio(&obj); is.SkipToNextObject(); fio.Read(is); return(is); }

// Classe pour la gestion de persistance
// ObjFileIO<DVList>

} // namespace SOPHYA

#endif /* DVLIST_H__SEEN */


