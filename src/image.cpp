#include "stdafx.h"

#include "image.h"

#ifdef HAVE_CONFIG_H
#include "splot_config.h"
#endif

//#if defined (HAVE_APPLE_OPENGL_FRAMEWORK) || defined (HAVE_OPENGL_GLU_H)
//#include <OpenGL/glu.h>
//#else
//#include "GL/glu.h"
//#elif defined (USE_GLPNG_IMAGE)
//#if defined(HAVE_APPLE_OPENGL_FRAMEWORK) || defined(HAVE_OPENGL_GL_H)
//#include <OpenGL/gl.h>
//#else
//#include <GL/gl.h>
//#endif
//#if defined(HAVE_APPLE_OPENGL_FRAMEWORK) || defined(HAVE_GLPNG_GLPNG_H)
//#include <glpng/glpng.h>
//#else
//#include <GL/glpng.h>
//#endif
//#endif

#include "ace/OS.h"
#include "ace/Log_Msg.h"

#if defined (USE_SDL)
#include "SDL.h"
#endif
#if defined (USE_SDL_IMAGE)
#include "SDL_image.h"
#endif

GLuint
Splot_Image::load (const std::string& filename_in,
                   ImageMipMap_t mipmap_in,
                   ImageBlend_t transformation_in,
                   GLint wrapst_in,
                   GLint minfilter_in,
                   GLint magfilter_in)
{
  GLuint texture_id = 0;

#if defined (USE_SDL_IMAGE)
  SDL_Surface* image = IMG_Load (filename_in.c_str ());
  if (!image)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to IMG_Load (\"%s\"): \"%s\", aborting\n"),
                ACE_TEXT (filename_in.c_str ()),
                ACE_TEXT (IMG_GetError ())));

    return 0;
  } // end IF

  glGenTextures (1, &texture_id);
  glBindTexture (GL_TEXTURE_2D, texture_id);

  glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrapst_in);
  glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrapst_in);

  glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, magfilter_in);
  glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, minfilter_in);

  // for the blend types, convert to RGBA
  if ((transformation_in != IMG_SOLID) &&
      (transformation_in != IMG_ALPHA))
  {
    SDL_PixelFormat rgba;
    ACE_OS::memset (&rgba, 0, sizeof (rgba));
    rgba.BitsPerPixel = 32; rgba.BytesPerPixel = 4;
#if SDL_BYTEORDER == SDL_BIG_ENDIAN
    rgba.Rmask = 0xFF000000; rgba.Rshift = 0; rgba.Rloss = 0;
    rgba.Gmask = 0x00FF0000; rgba.Gshift = 8; rgba.Gloss = 0;
    rgba.Bmask = 0x0000FF00; rgba.Bshift = 16; rgba.Bloss = 0;
    rgba.Amask = 0x000000FF; rgba.Ashift = 24; rgba.Aloss = 0;
#else
    rgba.Rmask = 0x000000FF; rgba.Rshift = 24; rgba.Rloss = 0;
    rgba.Gmask = 0x0000FF00; rgba.Gshift = 16; rgba.Gloss = 0;
    rgba.Bmask = 0x00FF0000; rgba.Bshift = 8; rgba.Bloss = 0;
    rgba.Amask = 0xFF000000; rgba.Ashift = 0; rgba.Aloss = 0;
#endif

    SDL_Surface* tmp = NULL;
#if SDL_VERSION_ATLEAST (2,0,0)
    tmp = SDL_ConvertSurface (image, &rgba, SDL_SWSURFACE);
#else
    tmp = SDL_ConvertSurface (image, &rgba, SDL_SWSURFACE | SDL_SRCALPHA);
#endif
    if (!tmp)
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to SDL_ConvertSurface (): \"%s\", aborting\n"),
                  ACE_TEXT (SDL_GetError ())));

      // clean up
      SDL_FreeSurface (image);

      return 0;
    } // end IF

    if (SDL_MUSTLOCK (tmp))
      if (SDL_LockSurface (tmp))
        ACE_DEBUG ((LM_ERROR,
                    ACE_TEXT ("failed to SDL_LockSurface (): \"%s\", continuing\n"),
                    ACE_TEXT (SDL_GetError ())));

    // This stuff probably faster than using SDL_GetRGBA and SDL_MapRGBA
    // Since we are using RGBA here, it works fine
    Uint32 a = 0;
    Uint8* p = NULL;
    switch (transformation_in)
    {
      case IMG_BLEND1:
        for (int y = 0; y < tmp->h; y++)
          for (int x = 0; x < tmp->w; x++)
          {
            p = (Uint8*)tmp->pixels + y*tmp->pitch + x*tmp->format->BytesPerPixel;
            // a = r+g+b (clipped to 0xFF)
            a = p[0] + p[1] + p[2];
            if (a > 0xFF) p[3] = 0xFF; else p[3] = a;
          } // end FOR
        break;
      case IMG_BLEND2:
        for (int y = 0; y < tmp->h; y++)
          for (int x = 0; x < tmp->w; x++)
          {
            p = (Uint8*)tmp->pixels + y*tmp->pitch + x*tmp->format->BytesPerPixel;
            // a = (r+g+b)/2 (clipped to 0xFF)
            a = p[0] + p[1] + p[2];
            if (a > 0x1FE) p[3] = 0xFF; else p[3] = a/2;
          } // end FOR
        break;
      case IMG_BLEND3:
        for (int y = 0; y < tmp->h; y++)
          for (int x = 0; x < tmp->w; x++)
          {
            p = (Uint8*)tmp->pixels + y*tmp->pitch + x*tmp->format->BytesPerPixel;
            // a = (r+g+b)/3
            a = p[0] + p[1] + p[2];
            p[3] = a/3;
          } // end FOR
        break;
      default:
        ACE_DEBUG ((LM_ERROR,
                    ACE_TEXT ("invalid / unknown transformation (was: %d), continuing\n"),
                    transformation_in));
        break;
    } // end SWITCH

    if (SDL_MUSTLOCK (tmp))
      SDL_UnlockSurface (tmp);

    SDL_FreeSurface (image);
    image = tmp;
  } // end IF

  GLenum format = GL_LUMINANCE;
  switch (image->format->BytesPerPixel)
  {
    case 4: format = GL_RGBA; break;
    case 3: format = GL_RGB; break;
    case 2: format = GL_LUMINANCE_ALPHA; break;
    default: break;
  } // end SWITCH

  GLint pack, unpack;
  glGetIntegerv (GL_PACK_ALIGNMENT, &pack);
  glGetIntegerv (GL_UNPACK_ALIGNMENT, &unpack);
  glPixelStorei (GL_PACK_ALIGNMENT, 1);
  glPixelStorei (GL_UNPACK_ALIGNMENT, 1);

  if (SDL_MUSTLOCK (image))
    if (SDL_LockSurface (image))
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to SDL_LockSurface (): \"%s\", continuing\n"),
                  ACE_TEXT (SDL_GetError())));

  switch (mipmap_in)
  {
    case IMG_NOMIPMAPS:
      glTexImage2D (GL_TEXTURE_2D,
                    0,
                    image->format->BytesPerPixel,
                    image->w, image->h,
                    0,
                    format,
                    GL_UNSIGNED_BYTE,
                    image->pixels);
      break;
    case IMG_SIMPLEMIPMAPS:
    case IMG_BUILDMIPMAPS:
      gluBuild2DMipmaps (GL_TEXTURE_2D,
                         image->format->BytesPerPixel,
                         image->w, image->h,
                         format,
                         GL_UNSIGNED_BYTE,
                         image->pixels);
      break;
    default:
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("invalid / unknown mipmap option (was: %d), continuing\n"),
                  mipmap_in));
      break;
  } // end SWITCH

  if (SDL_MUSTLOCK (image))
    SDL_UnlockSurface (image);

  glPixelStorei (GL_PACK_ALIGNMENT, pack);
  glPixelStorei (GL_UNPACK_ALIGNMENT, unpack);

  SDL_FreeSurface (image);
#elif defined (USE_GLPNG_IMAGE)
  int glpng_mipmap = PNG_NOMIPMAPS;
  switch (mipmap)
  {
    case IMG_NOMIPMAPS: glpng_mipmap = PNG_NOMIPMAPS; break;
    case IMG_SIMPLEMIPMAPS: glpng_mipmap = PNG_SIMPLEMIPMAPS; break;
    case IMG_BUILDMIPMAPS: glpng_mipmap = PNG_BUILDMIPMAPS; break;
    default:
      ACE_DEBUG  ((LM_ERROR,
                   ACE_TEXT ("invalid / unknown mipmap option (was: %d), continuing\n"),
                   mipmap_in));
      break;
  } // end SWITCH

  int glpng_trans = PNG_ALPHA;
  switch (transformation_in)
  {
    case IMG_SOLID: glpng_trans = PNG_SOLID; break;
    case IMG_ALPHA: glpng_trans = PNG_ALPHA; break;
    case IMG_BLEND1: glpng_trans = PNG_BLEND1; break;
    case IMG_BLEND2: glpng_trans = PNG_BLEND2; break;
    case IMG_BLEND3: glpng_trans = PNG_BLEND3; break;
    default:
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("invalid / unknown transformation (was: %d), continuing\n"),
                  transformation_in));
      break;
  } // end SWITCH

  texture_id = pngBind (filename_in.c_str(),
                        glpng_mipmap,
                        glpng_trans,
                        NULL,
                        wrapst_in,
                        minfilter_in,
                        magfilter_in);
  if (!texture_id)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to pngBind (\"%s\"), aborting\n"),
                ACE_TEXT (filename_in.c_str ())));

    return 0;
  } // end IF
#endif

  // check OpenGL error(s)
  GLenum OpenGL_error = glGetError ();
  //while ((OpenGL_error = glGetError ()) != GL_NO_ERROR)
  if (OpenGL_error != GL_NO_ERROR)
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("OpenGL error: \"%s\", continuing\n"),
                gluErrorString (OpenGL_error)));

  return texture_id;
}
