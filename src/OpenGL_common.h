#ifndef OPENGLCOMMON_H
#define OPENGLCOMMON_H

#include "ace/Global_Macros.h"

class Splot_OpenGLCommon
{
 public:
  static bool init ();
  static void fini ();

  static bool initScreen ();

  static void draw ();
  static void drawGame ();
  static void drawGameOver ();
  static void drawLevelComplete ();
  static void drawText (const char*, // string
                        float,       // pulse
                        float);      // scale
  static void reshape (int, int); // width / height

  static const char* findFont ();

 private:
  ACE_UNIMPLEMENTED_FUNC (Splot_OpenGLCommon ());
  ACE_UNIMPLEMENTED_FUNC (~Splot_OpenGLCommon ());

  static bool initTextToolkit ();
  static void finiTextToolkit ();

  static bool initialized_;
};

#endif // OPENGLCOMMON_H
