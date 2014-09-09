#ifndef BACKGROUNDSEA_H
#define BACKGROUNDSEA_H

#ifdef HAVE_CONFIG_H
#include "splot-config.h"
#endif

#if defined(HAVE_APPLE_OPENGL_FRAMEWORK) || defined(HAVE_OPENGL_GL_H)
#include <OpenGL/gl.h>
#else
#include "GL/gl.h"
#endif

#include "background.h"

class Splot_BackgroundSea
 : public Splot_Background
{
 public:
  Splot_BackgroundSea ();
  virtual ~Splot_BackgroundSea ();

  static bool init ();
  static void fini ();

  void drawGL ();
  void setVariation (int); // index

  void loadTextures ();
  void deleteTextures ();

  float size_;

 private:
  typedef Splot_Background inherited;

  static GLuint texBase;
  //static float vert[4][3];

};

#endif // BACKGROUNDSEA_H
