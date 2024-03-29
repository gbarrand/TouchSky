//-----------------------------------------------------------
// Class RPNExpressionEvaluator ,  RPNExprException 
// RPN (Reverse Polish Notation) algebraic computation .
//  Like HP (Hewlett-Packard) calculators 
// SOPHYA class library - (C)  UPS+LAL IN2P3/CNRS  , CEA-Irfu
//           Mars 2004: Extrait de commander.cc
//-----------------------------------------------------------

#ifndef RPNEVALUATOR_H_SEEN
#define RPNEVALUATOR_H_SEEN

#include "machdefs.h"
#include "pexceptions.h"
#include <string>
#include <vector>
#include <stack>

namespace SOPHYA {

/*!
  \ingroup SysTools
  \brief Exception class used by  RPNExpressionEvaluator
  \sa RPNExpressionEvaluator
*/
class RPNExprException : public PException {
  public:
  explicit RPNExprException(const char * m) throw() : PException(m) {}
  explicit RPNExprException(const std::string& m) throw() :  PException(m) {}
};


//! Class for evaluation of arithmetic expressions in RPN (Reverse Polish Notation)
class RPNExpressionEvaluator {
public:
  explicit RPNExpressionEvaluator(std::string const & sex);
  explicit RPNExpressionEvaluator(std::vector<std::string> & exe, int off=0);
  virtual ~RPNExpressionEvaluator();
//! Return the stack top \c (x) as the result of the expression evaluation
  virtual double Evaluate() const;  // Return the value of the stack top
//! Alias for Evaluate()
  inline double Value() const { return Evaluate(); }

private:
  int EvalRPNExpr(std::vector<std::string> & args, int off=0);
  inline bool CheckStack(double& x) const 
  {  
    if (rpnstack_.empty()) return true; 
    else { x = rpnstack_.top();  return false; }
  }
  inline bool CheckStack(double& x, double& y)
  {
    if (rpnstack_.size() < 2) return true; 
    else { 
      x = rpnstack_.top(); rpnstack_.pop(); 
      y = rpnstack_.top();  return false; 
    }
  }

  void PrintStack();
  int SumStack(double& sx, double& sx2);
  int ProductStack(double& px); 
  
  std::stack<double> rpnstack_;
};

} // namespace SOPHYA

/* end of ifdef  RPNEVALUATOR_H_SEEN */
#endif
