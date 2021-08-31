// This may look like C code, but it is really -*- C++ -*-
//-----------------------------------------------------------
// Class  ProgressBar : to indicate code progress 
// SOPHYA class library - (C)  UPS+LAL IN2P3/CNRS  , CEA-Irfu 
// R. Ansari      UPS+LAL IN2P3/CNRS  2014 
//-----------------------------------------------------------

#ifndef PROGBAR_H_SEEN
#define PROGBAR_H_SEEN

#include "machdefs.h"
#include "ctimer.h"

namespace SOPHYA {

//--- Class for providing information on execution progress 
//! enum which defines the progress bar operation mode
enum ProgressBarMode {ProgBarM_None, ProgBarM_Percent, ProgBarM_Time};

class ProgressBar 
{
public:
  //! constructor; arguments: maxloop count and indicator mode: ProgBarM_None, ProgBarM_Percent, ProgBarM_Tim
  ProgressBar(size_t maxcnt, ProgressBarMode mode=ProgBarM_Percent);

  ~ProgressBar() { }
  //! call this method with the loop counter value to update the progress info 
  inline void update(size_t cnt) 
  {
    if (mode_==ProgBarM_None) return;
    else return update_P(cnt);
  }

  //! return progress indicator mode 
  inline ProgressBarMode getMode() { return mode_; }
  //! set/changes the progress indicator mode 
  inline void setMode(ProgressBarMode mode) { mode_=mode; }
  //! reset the object, optionaly changing the max-count if maxcnt>0
  void reset(size_t maxcnt=0);

  //! set/changes the progress indicator character (or mark) 
  inline void setProgressMark(char c='.')  { mark_=c; }
  //! return the progress indicator character (or mark) 
  inline char getProgressMark()  { return mark_; }
  //! set/changes the default progress indicator character (or mark) used by the class instances
  static void setDefaultProgressMark(char c='.') { def_mark_=c; }
  //! return the default progress indicator character (or mark) used by the class instances
  static char getDefaultProgressMark() { return def_mark_; }

protected:
  void update_P(size_t cnt);

  ProgressBarMode mode_;  //!< progress indicator mode:  ProgBarM_None, ProgBarM_Percent, ProgBarM_Time
  size_t maxcnt_;         //!< max count and progresspercentage counters 
  size_t lastpercent_, last5percent_;  //!< progresspercentage counters 
  Timer tm_;              //!< timer used in ProgBarM_Time mode 
  char mark_;             //!< character used as progress indicator  
  static char def_mark_;  //!< default progress indicator character 
};

} // namespace SOPHYA


#endif
