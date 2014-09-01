#ifndef MAIN_H
#define MAIN_H

#include "ace/OS.h"

class Splot_Main
{
 public:
  Splot_Main (int = 0, ACE_TCHAR** = NULL);
  virtual ~Splot_Main ();

  virtual bool run () = 0;
  virtual bool checkErrors () = 0;

  virtual bool setVideoMode () = 0;

  virtual void grabMouse (bool, bool = true) = 0;
};

#endif // MAIN_H
