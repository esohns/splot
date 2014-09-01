#ifndef BACKGROUND_H
#define BACKGROUND_H

#include <list>

#ifdef HAVE_CONFIG_H
#include "splot-config.h"
#endif

#ifdef USE_SDL
#include "SDL_opengl.h"
#elif defined (HAVE_APPLE_OPENGL_FRAMEWORK) || defined (HAVE_OPENGL_GL_H)
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
class Splot_BackgroundSegment;
class Splot_BackgroundMetalSegment;
class Splot_BackgroundSeaSegment;

class Splot_Background
{
  friend class Splot_BackgroundMetalSegment;
  friend class Splot_BackgroundSeaSegment;

 public:
  Splot_Background ();
  virtual ~Splot_Background ();

  virtual void drawGL () = 0;
  virtual void setVariation (int) = 0;
  void nextVariation ();

  virtual void loadTextures () = 0;
  virtual void deleteTextures () = 0;

 protected:
  int        variation_;
  float      position_[3];
  GLuint     tex_[MAX_BACKGROUND_TYPES];

  typedef std::list<Splot_BackgroundSegment*> segments_t;
  typedef segments_t::iterator segmentsIterator_t;
  segments_t segments_;
};

#endif // BACKGROUND_H
