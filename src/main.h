#ifndef MAIN_H
#define MAIN_H

class Splot_Main
{
 public:
  Splot_Main (int = 0, char** = NULL);
  virtual ~Splot_Main ();

  virtual bool run () = 0;
  virtual bool checkErrors () = 0;

  virtual bool setVideoMode () = 0;

  virtual void grabMouse (bool, bool = true) = 0;
};

#endif // MAIN_H
