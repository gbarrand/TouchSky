//-----------------------------------------------------------
// Classes CmdInterpreter , CmdExecutor , Commander , ...
// simple command interpreter with c-shell like syntax
// SOPHYA class library - (C)  UPS+LAL IN2P3/CNRS  , CEA-Irfu 
// R. Ansari          (C) UPS+LAL IN2P3/CNRS  1997-2003  
//-----------------------------------------------------------

// Classe interpreteur de commande pour piapp 
// Reza      Aout 97 , Juillet,Aout 98
//           Octobre 2003: de PIext -> SysTools
// LAL-IN2P3/CNRS  DAPNIA/CEA

#ifndef COMMANDER_H_SEEN
#define COMMANDER_H_SEEN

#include "machdefs.h"
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <list>
#include <stack>
#include <map>
#include <functional>

#include "pdlmgr.h"
#include "dvlist.h"
#include "ctimer.h"

#include "cmdexecutor.h"

namespace SOPHYA {

////// Classe definissant l'interface pour un interpreteur de commande

/*!
  \ingroup SysTools
  \brief  Interface definition for a generic command interpreter.
*/
class CmdInterpreter {
public:
  virtual		~CmdInterpreter() {} ;
//! Returns the interpreter's name
  virtual std::string	Name()=0;
//! Returns the interpreter's short name (used for selection) 
  virtual std::string	ShortName() { return Name(); }
//! Method to be called in order to interpret a line or string.
  virtual int		Interpret(std::string& line)=0;
//! If the CmdInterpreter object is added to the Commander object, this method is called for every command added to the Commander class
  virtual void          RegisterCommandNotify(std::string const & keyw) { } ;
};


// classes CmdExecutor et CmdExecutor2 definies dans cmdexecutor.h   (Aout 2020) 


//--- forward class declaration for various helper classes for   Commander

class CommanderBloc;    // Bloc de type foreach / for de l'interpreteur Commander
class CommanderScript;  // Script de commandes defini ds l'interpreteur Commander
class CommandExeThr;    // Thread d'execution de commande

//! A simple command interpreter with c-shell like syntax and  dynamic load capability.

class Commander : public CmdInterpreter  {
public:
  static Commander*	GetInterpreter();

			Commander(bool fgsigzt=true);
  virtual		~Commander();
  virtual std::string	Name();

  virtual void		AddHelpGroup(std::string& grp, std::string& desc);
  virtual void		RegisterCommand(std::string& keyw, std::string& usage, CmdExecutor * ce, 
                                        std::string& grp, bool fgcex2=false);
  inline  void		RegisterCommand(std::string& keyw, std::string& usage, CmdExecutor * ce, 
                                        const char* grp)
  { std::string sgrp = grp; RegisterCommand(keyw, usage, ce, sgrp); }
  //! Command executor registration for CmdExecutor2 (new/extended interface)  
  inline  void		RegisterCommand2(std::string& keyw, std::string& usage, CmdExecutor2 * ce, 
                                        std::string& grp)
  { RegisterCommand(keyw, usage, ce, grp, true); }
  //! Command executor registration for CmdExecutor2 (new/extended interface)  
  inline  void		RegisterCommand2(std::string& keyw, std::string& usage, CmdExecutor2 * ce, 
                                        const char* grp)
  { std::string sgrp = grp; RegisterCommand(keyw, usage, ce, sgrp, true); }

  virtual void		RegisterHelp(std::string& keyw, std::string& usage, std::string& grp);

  virtual void		LoadModule(std::string& fnameso, std::string& name);

  virtual bool		AddInterpreter(CmdInterpreter * cl);
  virtual void		RemoveInterpreter(CmdInterpreter * cl);
  virtual bool		SelInterpreter(std::string const& name);
  inline  bool          SelInterpreter(const char * name) { std::string sname=name; return SelInterpreter(sname); }
  virtual void		SelDefaultInterpreter();

  virtual int		Interpret(std::string& line);
  virtual void		StopExecution();

  virtual int		ExecuteCommand(std::string& keyw, std::vector<std::string>& args, std::string& toks);
  virtual CmdEx_Param	ExecuteCommand(std::string& keyw, CmdEx_ParamList & plist);

  virtual int		ExecFile(std::string& file, std::vector<std::string>& args);
  virtual int		CShellExecute(std::string cmd);
  virtual std::string&	GetUsage(const std::string& kw);

  inline  void		SetMaxLoopLimit(int_8 lim=0) { maxlooplimit_ = lim; }
  inline  int_8         GetMaxLoopLimit() { return maxlooplimit_; }

  std::string		GetCurrentPrompt() { return curprompt; }


  virtual void 		HelptoLaTeX(std::string const & flnm);

  //! return the current selected interpreter (default : this)
  inline  CmdInterpreter* CurrentInterpreter() { return(curcmdi); }

  // ----- Action / gestion des variables propres de l'interpreteur
  // Verifie l'existence de la variable nomme vn et retourne sa valeur ds vv
  // Retourne false si la variable n'existe pas
  virtual bool    GetVar(std::string const & vn, std::string & vv);
  virtual bool    GetVar(std::string const & vn, int idx, std::string & vv);
  virtual bool    GetVar(std::string const & vn, std::vector<std::string> & vv);
  virtual bool    SetVar(std::string const & vn, std::string const & vv);
  virtual bool    SetVar(std::string const & vn, int idx, std::string const & vv);
  virtual bool    SetVar(std::string const & vn, std::vector<std::string> const & vv);
  virtual bool    CheckVarName(std::string const & vn);
  virtual bool    DeleteVar(std::string const & vn);
  virtual void    ListVar();
  // Variables de l'environnement application 
  virtual bool    GetVarApp(std::string const & vn, std::string & vv);
  virtual bool    SetVarApp(std::string const & vn, std::string const & vv);
  virtual bool    DeleteVarApp(std::string const & vn);
  virtual void    ListVarApp();
  // Variables d'environnement globales 
  virtual bool    GetVarEnv(std::string const & vn, std::string & vv);
  virtual bool    SetVarEnv(std::string const & vn, std::string const & vv);
  virtual bool    DeleteVarEnv(std::string const & vn);
  virtual void    ListVarEnv();

  //   Utilitaire pour decoupage en mot
  static  int   LineToWords(std::string& line, std::string& kw, std::vector<std::string>& tokens,
			    std::vector<bool>& qottoks, std::string& toks, bool uq=true);
protected:
  virtual int	ParseLineExecute(std::string& line, bool qw=true);

  virtual int   ExecuteCommandLine(std::string & keyw, std::vector<std::string> & args,
				   std::string & toks);

  virtual bool	CheckHelpGrp(std::string& grp, int& gid, std::string& desc);
  inline  bool	CheckHelpGrp(std::string& grp, int& gid)
  { std::string desc=""; return CheckHelpGrp(grp, gid, desc); }

  virtual int   SubstituteVars(std::string & s, std::string & s2);  
  int	        EvaluateTest(std::vector<std::string> & args, 
			     std::string & line, bool & res);

  // variable de l'interpreteur = valeur - accepte la syntaxe de type varname[index] 
  virtual bool    SetVariable(std::string const & vn, std::string const & vv);
  // Acces aux variables 
  virtual bool    Var2Str(std::string const & vn, std::string & vv);
  inline  bool    Var2Str(std::string const & vn, int idx, std::string & vv)
  { return GetVar(vn, idx, vv); }
  inline  bool    Var2Str(std::string const & vn, std::vector<std::string> & vv)
  { return GetVar(vn, vv); }

  virtual std::string  GetTmpDir();

  virtual void  SetCurrentPrompt(const char* pr);
  inline void   SetCurrentPrompt(std::string const & pr) { SetCurrentPrompt(pr.c_str()); }
  inline void   SetDefaultPrompt(std::string const & pr) { defprompt = pr; }

  virtual void	ShowMessage(const char * msg, int att);

  void          PushStack(std::vector<std::string> * pargs);
  inline void   PushStack() { PushStack(NULL);  return; }
  inline void   PushStack(std::vector<std::string> & args) { PushStack(&args);  return; }
  void          PopStack(bool psta=true);

  //   Gestion des threads d'execution de commandes 
  void          ListThreads();
  void          StopThr(uint_8 thrid, bool fgkill=true);
  void          CleanThrList();
  void          WaitThreads();

// ------ Attributs et variables ------ 
  CmdInterpreter* curcmdi;

// Gestion des variables
  typedef std::map< std::string,  std::vector<std::string>, std::less<std::string> > CmdVarList;
  CmdVarList variables;

// Pour enregistrer la liste de commandes et leurs executeurs et le help
/*! \cond   Pour supprimer la documentation par doxygen */
//  Command executor registration - For Commander internal use - fgcex2=true if CmdExecutor2 
  struct cmdex {int group; std::string us; CmdExecutor * cex; bool fgcex2; } ;
//  Help text registration - For Commander internal use
  struct hgrpst {int gid; std::string desc; } ;               // Identification+description d'un groupe de help
  /*! \endcond */

  typedef std::map<std::string, hgrpst, std::less<std::string> > CmdHGroup;   // Liste des groupes de commandes
  CmdHGroup cmdhgrp;
  int cmdgrpid;                                        // Numero de groupe courant
  typedef std::map<std::string, cmdex, std::less<std::string> > CmdExmap;  
  CmdExmap cmdexmap;                                   // Liste des commandes et leurs executeurs
  CmdExmap helpexmap;                                  // Pour les helps sans commande
// Pour garder la liste des threads d'execution de commande
  std::list<CommandExeThr *> CmdThrExeList;
  uint_8 ThrId;

// Pour garder la liste des modules 
  typedef std::map<std::string, PDynLinkMgr* , std::less<std::string> > Modmap;
  Modmap modmap;

// Pour garder la liste des interpreteur
  typedef std::map<std::string, CmdInterpreter*, std::less<std::string> > InterpMap;
  InterpMap interpmap;
 
//  Pour stocker les scripts definis ds l'interpreteur
  typedef std::map<std::string, CommanderScript*, std::less<std::string> > ScriptList;
  ScriptList mScripts;  // Liste des scripts
  CommanderScript* curscript; // Script en cours de definition

  // Code de retour execution commande
  int _xstatus;
  // Valeur de retour (par l'instruction return) - 
  std::string _retstr;

  //  Pour stocker les alias definies par l'interpreteur
  typedef map<std::string, std::string, less<std::string> > CmdStrList;
  CmdStrList mAliases;  // Liste des alias 

  // Le stack pour les arguments des .pic et des scripts
  std::stack< std::vector<std::string> > ArgsStack;
  // Stack pour les Prompts
  std::stack<std::string> PromptStack; 

  // Gestion des blocs de commandes et tests (if) 
  std::stack< CommanderBloc * > CmdBlks;  // Bloc de commande courant (foreach, ...)
  int felevel_;                    // foreach-for level 
  int_8 maxlooplimit_;            // Limite maximum des boucles
  /*! \cond   Pour supprimer la documentation par doxygen */
  typedef struct {std::list<char> tstlist; std::list<char>::iterator tstresit; bool tstcurres; } TstStatus;
  /*! \endcond */
  std::stack< TstStatus > TestsStack; // Stack des resultats de test
  std::list<char>::iterator tresit_;       // Test courant
  bool curtestresult_;             // Resultat courant des tests 

  // Controle du flot d'execution 
  bool fgexebrk; 

  // Commande splitees sur plusieurs lignes
  bool mulinefg;            // Bloc multi-lignes (ligne suite)
  std::string mulinecmd;         // Commande multi-lignes

  // Texte de prompt (attente de commande)
  std::string spromptmul;        // Prompt console avant multi-ligne
  std::string curprompt;         // Prompt courant
  std::string defprompt;         // Prompt par defaut 

  // Gestion d'historique, trace, timing des commandes
  ofstream hist;       //  History file 
  bool histon;        //  True ->  history file
  bool trace;          // Trace flag
  bool timing;         // Display CPU Time
  Timer* gltimer;      // pour Display CPU Time
  bool varcexp;      // true -> decodage nom de variables lors d'evaluation d'expression 
friend class CommanderBloc;  
friend class CommanderScript;  

};

} // namespace SOPHYA

/* end of ifdef COMMANDER_H_SEEN */
#endif  

