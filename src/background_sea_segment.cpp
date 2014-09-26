#include "stdafx.h"

#include "background_sea_segment.h"

#ifdef HAVE_CONFIG_H
#include "splot-config.h"
#endif

#include <cmath>

#ifdef USE_SDL
#include "SDL_opengl.h"
#elif defined (HAVE_APPLE_OPENGL_FRAMEWORK) || defined (HAVE_OPENGL_GL_H)
#include <OpenGL/gl.h>
#else
#include "GL/gl.h"
#endif

#include "defines.h"
#include "state.h"
#include "background.h"

Splot_BackgroundSeaSegment::Splot_BackgroundSeaSegment (const float (&position_in)[3],
                                                        const float (&size_in)[2],
                                                        Splot_Background* parent_in)
 : inherited (position_in, size_in, parent_in)
{
  float a = 1.0F+(FRAND*0.5F);
  vert_[0][0] =  a*size_in[0]; a = 1.0F+(FRAND*0.5F);
  vert_[1][0] = -a*size_in[0]; a = 1.0F+(FRAND*0.5F);
  vert_[2][0] =  a*size_in[0]; a = 1.0F+(FRAND*0.5F);
  vert_[3][0] = -a*size_in[0]; a = 1.0F+(FRAND*0.5F);
  vert_[0][1] =  a*size_in[1]; a = 1.0F+(FRAND*0.5F);
  vert_[1][1] =  a*size_in[1]; a = 1.0F+(FRAND*0.5F);
  vert_[2][1] = -a*size_in[1]; a = 1.0F+(FRAND*0.5F);
  vert_[3][1] = -a*size_in[1]; a = 1.0F+(FRAND*0.5F);
  vert_[0][2] = 0.0;
  vert_[1][2] = 0.0;
  vert_[2][2] = 0.0;
  vert_[3][2] = 0.0;
}

Splot_BackgroundSeaSegment::~Splot_BackgroundSeaSegment ()
{

}

void
Splot_BackgroundSeaSegment::drawGL ()
{
  glPushMatrix ();
  glTranslatef (inherited::position_[0], inherited::position_[1], inherited::position_[2]);
  glBindTexture (GL_TEXTURE_2D, inherited::parent_->tex_[BACKGROUNDTEXTURE_BASE]);
  glBegin (GL_TRIANGLE_STRIP);
  glTexCoord2f (1.0, 0.0); glVertex3fv (vert_[0]);
  glTexCoord2f (0.0, 0.0); glVertex3fv (vert_[1]);
  glTexCoord2f (1.0, 1.0); glVertex3fv (vert_[2]);
  glTexCoord2f (0.0, 1.0); glVertex3fv (vert_[3]);
  glEnd ();
  glPopMatrix ();
}
