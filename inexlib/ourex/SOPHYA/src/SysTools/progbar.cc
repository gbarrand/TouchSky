#include "progbar.h"

namespace SOPHYA {

/*!
  \class ProgressBar
  \ingroup SysTools

  \brief Class for providing information on execution progress in loops

  This class can be used in loops to provide information on execution progress
  in loops with three modes : 
  - ProgBarM_None : no information provided when update() is called 
  - ProgBarM_Percent : indicator mark for each percent + 5% marks 
  - ProgBarM_Time : indicator mark for each percent + 5% marks + CPU/elapsed time every 10% 

  \code
  //--- the loop size 
  size_t N = 10000000; 
  //--- creating the progress bar 
  ProgressBar pgb(N);
  double s=0.;
  //--- simple loop with progress indicator 
  for(size_t i=0; i<N; i++) {
    s += (double)i;
    pgb.update(i);
  } 
  cout << "----- done Loop1 Sum[i, 0<=i<"<<N<<"]= "<<s<<endl;

  //--- reseting the progress indicator for a second loop, changing the mode
  pgb.setProgressMark('+');
  pgb.setMode(ProgBarM_Time);
  pgb.reset();
  //--- a more complex loop
  s=0.; double x; 
  for(size_t i=0; i<N; i++) {
    double x = (double)i*M_PI/373.;
    for(size_t j=0; j<10; j++) {
      s+=x; x=sin(x); 
    }
    pgb.update(i);
  }
  cout << "----- done Loop2 Sum[i, 0<=i<"<<N<<" (Sum[j=1,10; sin]) ]= "<<s<<endl;

  \endcode
*/

//--- default character used as progress indicator mark by class instances 
char ProgressBar::def_mark_ = '.';

/* --Methode-- */
ProgressBar::ProgressBar(size_t maxcnt, ProgressBarMode mode)
  : mode_(mode), maxcnt_(maxcnt), lastpercent_(0), last5percent_(5), tm_(false), mark_(def_mark_)
{
  if (maxcnt_<1) maxcnt_=1;
}

/* --Methode-- */
void ProgressBar::reset(size_t maxcnt)
{
  if (maxcnt>0) maxcnt_=maxcnt;
  lastpercent_=0; last5percent_=5;
  tm_.SplitQ();
}

/* --Methode-- */
void ProgressBar::update_P(size_t cnt)
{
  size_t perccnt = cnt*100/maxcnt_;
  if (cnt==maxcnt_-1) perccnt=100;
  if (perccnt <= lastpercent_) return;
  for(size_t ipp=lastpercent_; ipp<perccnt; ipp++) cout << mark_;
  
  while (perccnt >= last5percent_) {
    cout<<perccnt<<'%';
    if (mode_==ProgBarM_Time) {
      if ((last5percent_%10==0)||(perccnt==100)) {
	tm_.SplitQ(); 
	cout << " CPU: "<<tm_.PartialCPUTime()<<" Total CPU: "<<
	  tm_.TotalCPUTime()<<"  Elapsed: "<<tm_.TotalElapsedTime()<<" s."<<endl;
      }
    }
    else if ((last5percent_%40==0)||(perccnt==100)) cout<<endl;
    last5percent_+=5;
  }
  lastpercent_=perccnt;
  cout.flush();
  return;
}
  

} // FIN namespace SOPHYA 
