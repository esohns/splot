#ifndef BACKGROUND_H
#define BACKGROUND_H

#include <list>

#include "ace/Global_Macros.h"

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

enum BackgroundType_t
{
  BACKGROUND_INVALID = -1,
  BACKGROUND_METAL = 0,
  BACKGROUND_SEA,
  ///////////////////////////////////////
  MAX_BACKGROUND_TYPES
};

enum BackgroundTextureType_t
{
  BACKGROUNDTEXTURE_INVALID = -1,
  BACKGROUNDTEXTURE_BASE = 0,
  BACKGROUNDTEXTURE_BLIP,
  ///////////////////////////////////////
  MAX_BACKGROUNDTEXTURE_TYPES
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
  Splot_Background (BackgroundType_t);
  virtual ~Splot_Background ();

  static bool init ();
  static void fini ();
  static Splot_Background* make (BackgroundType_t);

  virtual void drawGL () = 0;
  virtual void setVariation (int) = 0;
  void nextVariation ();

  virtual void loadTextures () = 0;
  virtual void deleteTextures () = 0;

  BackgroundType_t type_;

 protected:
  int              variation_;
  float            position_[3];
  GLuint           tex_[MAX_BACKGROUNDTEXTURE_TYPES];

  typedef std::list<Splot_BackgroundSegment*> segments_t;
  typedef segments_t::iterator segmentsIterator_t;
  segments_t       segments_;

 private:
  ACE_UNIMPLEMENTED_FUNC (Splot_Background ());
};

#endif // BACKGROUND_H
