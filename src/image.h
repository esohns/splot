#ifndef IMAGE_H
#define IMAGE_H

#include <string>

#ifdef HAVE_CONFIG_H
#include "splot_config.h"
#endif

#ifdef USE_SDL
#include "SDL_opengl.h"
#elif defined (HAVE_APPLE_OPENGL_FRAMEWORK) || defined (HAVE_OPENGL_GL_H)
#include <OpenGL/gl.h>
#else
#include "GL/gl.h"
#endif

#include "ace/Global_Macros.h"

enum ImageMipMap_t
{
  IMG_NOMIPMAPS,
  IMG_SIMPLEMIPMAPS,
  IMG_BUILDMIPMAPS
};

enum ImageBlend_t
{
  IMG_SOLID,
  IMG_ALPHA,
  IMG_BLEND1,
  IMG_BLEND2,
  IMG_BLEND3
};

class Splot_Image
{
 public:
  static GLuint load (const std::string&,            // filename
                      ImageMipMap_t = IMG_NOMIPMAPS, // mipmap
                      ImageBlend_t = IMG_ALPHA,      // blend / tansformation
                      GLint = GL_CLAMP,              // GL_TEXTURE_WRAP_S / GL_TEXTURE_WRAP_T
                      GLint = GL_LINEAR,             // GL_TEXTURE_MIN_FILTER
                      GLint = GL_LINEAR);            // GL_TEXTURE_MAG_FILTER

 private:
  ACE_UNIMPLEMENTED_FUNC (Splot_Image ());
  ACE_UNIMPLEMENTED_FUNC (~Splot_Image ());
};

#endif // IMAGE_H
