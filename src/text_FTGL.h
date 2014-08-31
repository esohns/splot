#ifndef TEXTFTGL_H
#define TEXTFTGL_H

#ifdef USE_FTGL_TEXT
#include "FTGL/ftgl.h"

#include "text.h"

class Splot_TextFTGL
 : public Splot_Text
{
 public:
  Splot_TextFTGL ();
  virtual ~Splot_TextFTGL ();

  virtual void Render (const char*, const int = -1);
  virtual float Advance (const char*, const int = -1);
  virtual float LineHeight(const char*, const int = -1);

 private:
  typedef Splot_Text inherited;

  FTFont* ftFont_;

  char* findFont ();
};
#endif // USE_FTGL_TEXT

#endif // TEXTFTGL_H
