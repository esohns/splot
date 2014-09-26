#ifndef BACKGROUNDMETAL_H
#define BACKGROUNDMETAL_H

//#ifdef HAVE_CONFIG_H
//#include "splot-config.h"
//#endif

#ifdef USE_SDL
#include "SDL_opengl.h"
#elif defined (HAVE_APPLE_OPENGL_FRAMEWORK) || defined (HAVE_OPENGL_GL_H)
#include <OpenGL/gl.h>
#else
#include "GL/gl.h"
#endif

#include "background.h"

enum MetalBackgroundVariant_t
{
  METALBACKGROUNDVARIANT_INVALID = -1,
  METALBACKGROUNDVARIANT_1 = 0,
  METALBACKGROUNDVARIANT_2,
  METALBACKGROUNDVARIANT_3,
  ///////////////////////////////////////
  MAX_METALBACKGROUNDVARIANT_TYPES
};

class Splot_BackgroundMetal
 : public Splot_Background
{
 public:
  Splot_BackgroundMetal ();
  virtual ~Splot_BackgroundMetal ();

  static bool init ();
  static void fini ();

  void drawGL ();
  void setVariation (int); // index

  void loadTextures ();
  void deleteTextures ();

  float size_;

 private:
  typedef Splot_Background inherited;

  static GLuint texBase[MAX_METALBACKGROUNDVARIANT_TYPES];
  static GLuint texBlip;
};

#endif // BACKGROUNDMETAL_H
