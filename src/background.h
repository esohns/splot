#ifndef BACKGROUND_H
#define BACKGROUND_H

//#ifdef HAVE_CONFIG_H
//#include "splot-config.h"
//#endif

//#include "compatibility.h"
//
#if defined(HAVE_APPLE_OPENGL_FRAMEWORK) || defined(HAVE_OPENGL_GL_H)
#include <OpenGL/gl.h>
#else
#include "GL/gl.h"
#endif

enum BackgroundTextureType_t
{
  BACKGROUND_INVALID = -1,
  BACKGROUND_BASE = 0,
  BACKGROUND_BLIP,
  ///////////////////////////////////////
  MAX_BACKGROUND_TYPES
};

// forward declarations
class GroundSegment;

class Splot_Background
{
  friend class GroundMetalSegment;
  friend class GroundSeaSegment;

 public:
  Splot_Background ();
  virtual ~Splot_Background ();

  virtual void drawGL() = 0;
  virtual void setVariation (int) = 0;
  void nextVariation ();

  virtual void loadTextures() = 0;
  virtual void deleteTextures() = 0;

 protected:
  int    variation_;
  float  position_[3];
  GLuint tex_[MAX_BACKGROUND_TYPES];

  GroundSegment* rootSeg_;
};

#endif // BACKGROUND_H
