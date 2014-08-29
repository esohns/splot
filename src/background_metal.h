#ifndef GROUNDMETAL_H
#define GROUNDMETAL_H

//#ifdef HAVE_CONFIG_H
//#include "splot-config.h"
//#endif

//#include "compatibility.h"

//#if defined(HAVE_APPLE_OPENGL_FRAMEWORK) || defined(HAVE_OPENGL_GL_H)
//#include <OpenGL/gl.h>
//#else
//#include "GL/gl.h"
//#endif

#include "background.h"

// forward declarations
class Splot_BackgroundSegment;
class GroundMetalSegment;

class Splot_BackgroundMetal
 : public Splot_Background
{
  friend class Splot_BackgroundSegment;
  friend class GroundMetalSegment;

 public:
  Splot_BackgroundMetal ();
  virtual ~Splot_BackgroundMetal ();

  void drawGL ();
  void setVariation (int); // index

  void loadTextures ();
  void deleteTextures ();

  float size_;

 private:
  typedef Splot_Background inherited;
};

#endif // GROUNDMETAL_H
