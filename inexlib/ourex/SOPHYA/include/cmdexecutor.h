//------------------------------------------------------------------------------------
// Classes  CmdExecutor et  PIACmd_Param , PIACmd_NamedParam , PIACmd_ParamList
// (passage de parametres et exceution de commandes entre l'appli et l'interpreteur)
// SOPHYA class library  - (C)  UPS+LAL IN2P3/CNRS  , CEA-Irfu 
//    Reza Ansari  , Univ. Paris-Sud   Aout 2020
//------------------------------------------------------------------------------------

#ifndef CMDEXECUTOR_H_SEEN
#define CMDEXECUTOR_H_SEEN

#include "machdefs.h"
#include <iostream>
#include <string>
#include <vector>

#include "mutyv.h"

namespace SOPHYA {

//--------------------------------------------------------------------------------------------
//------------------------------------    CmdEx_Param    ------------------------------------
//--------------------------------------------------------------------------------------------

/*!
  \ingroup SysTools
  \brief  class to hold parameters as well as return value for a command execution 

  This class holds the information exchanged between the application and a command interpreter, while executing commands 
*/
class CmdEx_Param {
public:
  //! type of values hold by the object 
  enum Type { CmdExP_None, CmdExP_Value, CmdExP_Pointer, CmdExP_ObjectId, CmdExP_WidgetId, CmdExP_ValueList, CmdExP_ObjectIdList} ;

  //! default constructor 
  explicit CmdEx_Param()
  { Init(); }
  //! constructor from a floating value 
  explicit CmdEx_Param(double val)
  { Init();  setValueD(val); }
  //! constructor from a string   
  explicit CmdEx_Param(std::string & sval)
  { Init();  setValueS(sval); }
  //! constructor from a string   
  explicit CmdEx_Param(const char * s)
  { Init();  setValueS(s); }
  //! copy constructor  
  CmdEx_Param(CmdEx_Param const & a)
  { CopyFrom(a);  }

  //! copy (equal) operator   
  inline CmdEx_Param & operator = (CmdEx_Param const & a)
  { CopyFrom(a);  return (*this); }

  //! return the type of value held by the object  
  inline CmdEx_Param::Type getType() const { return ptype_; }

  //! set the return code (when used as a return value)
  inline void setRC(int rc)  { rc_=rc;  err_msg_=""; }
  //! set the return code and associated message (usually an error message) 
  inline void setRC(int rc, std::string & msg)  { rc_=rc;  err_msg_=msg; }
  //! set the return code and associated message (usually an error message) 
  inline void setRC(int rc, const char * msg)  { rc_=rc;  if (msg) err_msg_=msg; else err_msg_=""; }
  //! return the return code (when used as a return value)
  inline int  getRC() const { return rc_; }
  //! return the error message 
  inline const char * getErrorMsg() const { return err_msg_.c_str(); }
  //! return the error message 
  inline string getErrorMsgS() const { return err_msg_; }

  //! fill the object with an Object identifier (handled by the application) 
  inline void  setObjectId(int id) { ptype_=CmdEx_Param::CmdExP_ObjectId;  obj_id_=id; }
  //! return Object Id (handled by the application) 
  int getObjectId() const
  { if (ptype_==CmdEx_Param::CmdExP_ObjectId)  return obj_id_; else return -1;}

  //! fill the object with a window/widget identifiers (handled by the GUI type application) 
  void  setWidgetId(int winid, int wdgid, int drwid)
  { ptype_=CmdEx_Param::CmdExP_WidgetId;  win_id_=winid, wdg_id_=wdgid; drw_id_=drwid; }
  //! return the window/widger identifiers 
  void  getWidgetId(int & winid, int & wdgid, int & drwid)  const 
  {
    winid = wdgid = drwid=-1;
    if (ptype_==CmdEx_Param::CmdExP_WidgetId) { winid=win_id_; wdgid=wdg_id_; drwid=drw_id_; }
    return;
  }

  inline void setValue(SOPHYA::MuTyV & mtv) { ptype_=CmdEx_Param::CmdExP_Value;  value_=mtv; }
  inline void setValueI(int_8 iv) { ptype_=CmdEx_Param::CmdExP_Value;  value_=iv; }
  inline void setValueD(double v) { ptype_=CmdEx_Param::CmdExP_Value;  value_=v; }
  inline void setValueS(std::string & s) { ptype_=CmdEx_Param::CmdExP_Value;  value_=s; }
  inline void setValueS(const char * s) { ptype_=CmdEx_Param::CmdExP_Value;  value_=s; }

  inline SOPHYA::MuTyV  getValue() const { return value_; }
  inline double         getValueD() const { double rv; return value_.Convert(rv); }
  inline int_8          getValueI() const { int_8  iv; return value_.Convert(iv); }
  inline std::string    getValueS() const { std::string rs; return value_.Convert(rs); }

  inline void setPointer(void * ptr) { ptype_=CmdEx_Param::CmdExP_Pointer;  ptr_=ptr; }
  inline void *         getPointer() { return ptr_; }

  //! Fill the object with a vector of values 
  void setValList(std::vector<SOPHYA::MuTyV> & v_mtv)
  {  ptype_=CmdEx_Param::CmdExP_ValueList;  val_vec_=v_mtv; }
  //! return the vector of values   
  inline std::vector<SOPHYA::MuTyV> const & getValList() const { return val_vec_; }
  //! return the vector of values   
  inline std::vector<SOPHYA::MuTyV> & getValList() { return val_vec_; }
  //! return the vector of values converted to float    
  inline std::vector<double>  getValListD() const
  {
    double x; std::vector<double> rv(val_vec_.size());
    for(size_t i=0; i<rv.size(); i++)  rv[i]=val_vec_[i].Convert(x);
    return rv;
  }
  //! return the vector of values converted to string    
  inline std::vector< std::string >  getValListS() const
  {
    std::string s; std::vector< std::string > rv(val_vec_.size());
    for(size_t i=0; i<rv.size(); i++)  rv[i]=val_vec_[i].Convert(s);
    return rv;
  }
  //! return the vector of values converted to string    
  inline std::vector< int >  getValListI() const
  {
    int_4 iv; std::vector< int > rv(val_vec_.size());
    for(size_t i=0; i<rv.size(); i++)  rv[i]=val_vec_[i].Convert(iv);
    return rv;
  }
  
  //! fill the object with a list of Object identifier (handled by the application) 
  void  setObjIdList(std::vector<int> vid)
  {
    ptype_=CmdEx_Param::CmdExP_ObjectIdList;
    val_vec_.resize(vid.size());
    for(size_t i=0; i<vid.size(); i++)  val_vec_[i]=MuTyV((int_8)vid[i]);
  }
  //! return number of ObjectId's in the list - return 0 if this is not an ObjectIdList 
  size_t getObjectIdListSize() const
  { if (ptype_==CmdEx_Param::CmdExP_ObjectIdList)  return val_vec_.size(); else return 0;}
  //! return  the n 'th ObjectId in the list - return -1 if not an ObjectIdList of n > list_size
  int getObjectIdFromList(size_t n) const
  {
    if ((ptype_==CmdEx_Param::CmdExP_ObjectIdList)&&(n<val_vec_.size()))  {
      int_4 iv;
      return val_vec_[n].Convert(iv);
    }
    return -1;
  }
  //! return  the n 'th ObjectId in the list as a CmdEx_Param
  CmdEx_Param getOId(size_t n) const
  {
    CmdEx_Param rp;
    rp.setObjectId(getObjectIdFromList(n));
    return rp;
  }
  //! a short text description of the object on ostream os
  std::ostream & print(std::ostream & os) const
  {
    os<<"CmdEx_Param: Type="<<getTypeS()<<" ObjId="<<obj_id_<<" WinId="<<win_id_<<" WdgId="<<wdg_id_<<" DrwId="<<drw_id_
      << " Value="<<value_<<" Ptr="<<std::hex<<ptr_<<std::dec<<" (Rc="<<rc_<<")";
    return os;
  }
  inline std::ostream & print() const { return print(std::cout); }
  
  std::string getTypeS() const
  {
    switch (ptype_) {
    case CmdEx_Param::CmdExP_None:
      return "None"; break;
    case CmdEx_Param::CmdExP_Value:
      return "Value"; break;
    case CmdEx_Param::CmdExP_Pointer:
      return "Pointer"; break;
    case CmdEx_Param::CmdExP_ObjectId:
      return "ObjectId"; break;
    case CmdEx_Param::CmdExP_WidgetId:
      return "WidgetId"; break;
    case CmdEx_Param::CmdExP_ValueList:
      return "ValueList"; break;
    case CmdEx_Param::CmdExP_ObjectIdList:
      return "ObjectIdList"; break;
    default:
      return "None"; break;
    }
    return "None";
  }
protected:
  void Init()
  {
    ptype_=CmdEx_Param::CmdExP_None;  rc_=0;
    obj_id_=-1; win_id_=wdg_id_=drw_id_=-1;
    value_ = 0;  ptr_=NULL;
  }
  void CopyFrom(CmdEx_Param const & a)
  {
    ptype_=a.ptype_; rc_=a.rc_;  err_msg_=a.err_msg_;
    obj_id_=a.obj_id_; win_id_=a.win_id_; wdg_id_=a.wdg_id_; drw_id_=a.drw_id_;
    value_=a.value_;   ptr_=a.ptr_; val_vec_=a.val_vec_;
  }
  
  CmdEx_Param::Type ptype_;
  int rc_;
  std::string err_msg_; 
  int obj_id_;
  int win_id_, wdg_id_, drw_id_;
  SOPHYA::MuTyV value_;
  void * ptr_;
  std::vector< SOPHYA::MuTyV > val_vec_; 
};

//!  print operator << for CmdEx_Param class 
inline std::ostream & operator << (std::ostream & os, SOPHYA::CmdEx_Param const & p)
{ return p.print(os); }

//--------------------------------------------------------------------------------------------
//----------------------------------    CmdEx_NamedParam   ----------------------------------
//--------------------------------------------------------------------------------------------

/*!
  \ingroup SysTools
  \brief  class to hold a named parameters: a CmdEx_Param object associated with a string (the name)
*/
class CmdEx_NamedParam {
public:
  explicit CmdEx_NamedParam()  { } 
  explicit CmdEx_NamedParam(std::string & name, CmdEx_Param const & p) : name_(name), param_(p)
  { }
  explicit CmdEx_NamedParam(const char * name, CmdEx_Param const & p) : name_(name), param_(p)
  { } 
  explicit CmdEx_NamedParam(std::string & name, double val) : name_(name), param_(val)
  { } 
  explicit CmdEx_NamedParam(const char * name, double val) : name_(name), param_(val)
  { }
  explicit CmdEx_NamedParam(std::string & name, std::string & sval)  : name_(name), param_(sval)
  { } 
  explicit CmdEx_NamedParam(std::string & name, const char * sval)  : name_(name), param_(sval)
  { }
  explicit CmdEx_NamedParam(const char * name, std::string & sval)  : name_(name), param_(sval)
  { }
  explicit CmdEx_NamedParam(const char * name, const char * sval)  : name_(name), param_(sval)
  { }
  std::string const &   getName() const { return name_; } 
  CmdEx_Param const &  get() const { return param_; } 
  CmdEx_Param &        get()  { return param_; } 
protected:
  std::string name_;
  CmdEx_Param param_;
};

//!  print operator << for CmdEx_NamedParam object 

inline std::ostream & operator << (std::ostream & os, SOPHYA::CmdEx_NamedParam const & np)
  { os<< "Name=" << np.getName() << " : " << np.get();  return os; }

//--------------------------------------------------------------------------------------------
//----------------------------------    CmdEx_ParamList    ----------------------------------
//--------------------------------------------------------------------------------------------

/*!
  \ingroup SysTools
  \brief  class to hold a list of parameters CmdEx_Param as well as a list of named parameters 
*/

class CmdEx_ParamList {
public:
  CmdEx_ParamList() { }

  size_t add(CmdEx_Param const & p)
  { v_param_.push_back(p); return v_param_.size(); }
  size_t add(CmdEx_NamedParam const & np)
  { v_named_params_.push_back(np);  return v_named_params_.size(); }
  inline  size_t add(double v)
  { CmdEx_Param p(v); return add(p); }
  inline  size_t add(std::string & s)
  { CmdEx_Param p(s); return add(p); }
  inline  size_t add(const char * s)
  { CmdEx_Param p(s); return add(p); }
  
  inline size_t getNbParams() const { return v_param_.size(); }
  inline size_t getNbNamedParams() const { return v_named_params_.size(); }

  CmdEx_Param & get(size_t n)
  {
    if (n >= v_param_.size())  return default_.get();
    return v_param_[n];
  }
  CmdEx_Param & get(std::string name)
  {
    for(size_t i=0; i<v_named_params_.size(); i++)
      if (v_named_params_[i].getName()==name)  return v_named_params_[i].get();
    return default_.get();
  }

  CmdEx_NamedParam & getNamed(size_t n)
  {
    if (n >= v_named_params_.size())  return default_;
    return v_named_params_[n];
  }

  inline CmdEx_Param & operator[] (size_t n) { return get(n); }
  inline CmdEx_Param & operator[] (string & name) { return get(name); }

  std::ostream & print(std::ostream & os)  const 
  {
    os << "CmdEx_ParamList - NbParams= "<<getNbParams()<<" NbNamedParams= "<<getNbNamedParams()<<std::endl;
    for(size_t i=0; i<getNbParams(); i++) os<<"["<<i<<"] "<< v_param_[i]<<std::endl;
    for(size_t i=0; i<getNbNamedParams(); i++) os<<"["<<i<<"] "<< v_named_params_[i]<<std::endl;
    return os;
  }

protected:
  std::vector< CmdEx_Param > v_param_;
  std::vector< CmdEx_NamedParam > v_named_params_;
  CmdEx_NamedParam default_;
};

//!  print operator << for CmdEx_ParamList class 
inline std::ostream & operator << (std::ostream & os, SOPHYA::CmdEx_ParamList const & pl)
{ return pl.print(os); }


/////// Classe definissant l'interface pour un executeur de commande

/*!
  \ingroup SysTools
  \brief Interface definition for command executor, to be used with Commander

  A command is defined by a keyword and a number of argument
*/

class CmdExecutor {
public:
  virtual	~CmdExecutor() {} ;
  // keyw : Le mot cle associe , args: Arguments de la commande
  /*! 
    \brief command execution method for a command defined by keyword and its arguments.

    The command is identified by the keyword \b kw , and a set of arguments \b args.
    The string \b toks represent the whole line (or string) parsed by the interpreter, 
    except the keyword itself.
  */
  virtual int	Execute(std::string& keyw, std::vector<std::string>& args, std::string& toks)=0;
  //! Return true if the command \b keyw is thread compatible (can be executed in a separate thread)
  virtual bool  IsThreadable(std::string const & keyw) { return false; }
};

/*!
  \ingroup SysTools
  \brief Implements the new, extended command execution interface, using CmdEx_ParamList for parameter list
  
  This class has been introduced in summer 2020, for implementing piapp command interpreter through LUA. 
*/

class CmdExecutor2 : public CmdExecutor {
public:
  //! command execution method for a command defined by the keyword \b keyw and argument list \b plist 
  virtual CmdEx_Param  Execute(std::string& keyw, CmdEx_ParamList & plist)=0;
  //! implement the base class Execute() method , through filling a CmdEx_ParamList and calling the above method. 
  virtual int	Execute(std::string& keyw, std::vector<std::string>& args, std::string& toks);
};


} // namespace SOPHYA

/* end of ifdef CMDEXECUTOR_H_SEEN */
#endif
