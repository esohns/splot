#include "stdafx.h"

#include "background_metal_segment.h"

//#ifdef HAVE_CONFIG_H
//#include "splot-config.h"
//#endif
//
//#include "Splot_BackgroundMetalSegment.h"
//
#include <cmath>
//
//#include "compatibility.h"
//
#if defined(HAVE_APPLE_OPENGL_FRAMEWORK) || defined(HAVE_OPENGL_GL_H)
#include <OpenGL/gl.h>
#else
#include "GL/gl.h"
#endif
//#define GL_EXT_
//#include <GL/glext.h>

#include "configuration.h"
#include "state.h"
#include "background.h"

// init statics
float Splot_BackgroundMetalSegment::c0_clr[4] = {0.65F, 0.62F, 0.53F, 1.0};
float Splot_BackgroundMetalSegment::c1_clr[4] = {0.79F, 0.82F, 0.69F, 1.0};
float Splot_BackgroundMetalSegment::r0_clr[4] = {0.07F, 0.07F, 0.13F, 1.0};
float Splot_BackgroundMetalSegment::r1_clr[4] = {0.31F, 0.3F , 0.3F , 1.0};
float Splot_BackgroundMetalSegment::r2_clr[4] = {0.31F, 0.3F , 0.3F , 1.0};

Splot_BackgroundMetalSegment::Splot_BackgroundMetalSegment (const float (&position_in)[3],
                                                            const float (&size_in)[2],
                                                            Splot_Background* parent_in)
 : inherited (position_in, size_in, parent_in)
{

}

Splot_BackgroundMetalSegment::~Splot_BackgroundMetalSegment ()
{

}

void
Splot_BackgroundMetalSegment::drawGL ()
{
  age_ += 1.0;

  State_t& state = SPLOT_STATE_SINGLETON::instance ()->get ();
  float clr_sin = 0.5F*sin (state.game_frame*0.001F);
  Splot_BackgroundMetalSegment::r1_clr[0] = 0.15F+clr_sin;
  Splot_BackgroundMetalSegment::r2_clr[0] = 0.15F+clr_sin;
  clr_sin = 0.2F*sin (state.game_frame*0.0005F);
  Splot_BackgroundMetalSegment::c0_clr[0] = 0.28F+clr_sin;
  Splot_BackgroundMetalSegment::c0_clr[1] = 0.25F+clr_sin;
  Splot_BackgroundMetalSegment::c0_clr[2] = 0.16F+clr_sin;
  Splot_BackgroundMetalSegment::c1_clr[0] = 0.42F+clr_sin;
  Splot_BackgroundMetalSegment::c1_clr[1] = 0.45F+clr_sin;
  Splot_BackgroundMetalSegment::c1_clr[2] = 0.34F+clr_sin;

  float S;
  float tmp;
  float rep;
  float tilt;
  bool blipMirrorT = false;
  switch (inherited::parent_->variation_)
  {
    default:
    case 0:
      rep = 0.26F;
      tilt = 0.1F;
      S = -state.frame*0.001F;
      blipMirrorT = false;
      break;
    case 1:
      rep = 0.4F;
      tilt = 0.2F;
      S = -state.frame*0.001F;
      blipMirrorT = true;
      break;
    case 2:
      S = -state.frame*0.005F;
      tmp = sin (S);
      rep = 0.7F+tmp;
      tilt = 0.5F+tmp;
      blipMirrorT = true;
      break;
  } // end SWITCH

#ifdef EXPERIMENTAL
  drawMultiTex (rep, S, tilt, blipMirrorT,
                c0_clr,
                c1_clr,
                r0_clr,
                r1_clr,
                r2_clr);
#else // EXPERIMENTAL
  const Configuration_t& configuration =
    SPLOT_CONFIGURATION_SINGLETON::instance ()->get ();
  if (configuration.gfx_level > 1)
    drawBlip (rep, S, tilt, blipMirrorT);
  drawSurface (Splot_BackgroundMetalSegment::c0_clr,
               Splot_BackgroundMetalSegment::c1_clr,
               Splot_BackgroundMetalSegment::r0_clr,
               Splot_BackgroundMetalSegment::r1_clr,
               Splot_BackgroundMetalSegment::r2_clr);
#endif // EXPERIMENTAL
}

void
Splot_BackgroundMetalSegment::drawBlip (float rep_in,
                                        float S_in,
                                        float tilt_in,
                                        bool blipMirrorT_in)
{
  float repA = 0.0;
  float repB = rep_in;
  glColor4f (1.0, 1.0, 1.0, 1.0);
  glBindTexture (GL_TEXTURE_2D, parent_->tex_[BACKGROUND_BLIP]);
  //	glBegin(GL_QUADS);
  glBegin (GL_TRIANGLES);
  glTexCoord2f (rep_in, repA+S_in+tilt_in);
  glVertex3f (inherited::position_[0],
              inherited::position_[1],
              inherited::position_[2]);
  glTexCoord2f (0.0, repA+S_in);
  glVertex3f (-inherited::size_[0]+inherited::position_[0],
               inherited::position_[1],
               inherited::position_[2]);
  glTexCoord2f (0.0, repB+S_in);
  glVertex3f (-inherited::size_[0]+inherited::position_[0],
              -inherited::size_[1]+inherited::position_[1],
               inherited::position_[2]);

  glTexCoord2f (rep_in, repA+S_in+tilt_in);
  glVertex3f (inherited::position_[0],
              inherited::position_[1],
              inherited::position_[2]);
  glTexCoord2f (0.0, repB+S_in);
  glVertex3f (-inherited::size_[0]+inherited::position_[0],
              -inherited::size_[1]+inherited::position_[1],
               inherited::position_[2]);
  glTexCoord2f (rep_in, repB+S_in+tilt_in);
  glVertex3f ( inherited::position_[0],
              -inherited::size_[1]+inherited::position_[1],
               inherited::position_[2]);
  //--
  glTexCoord2f (0.0, repA+S_in);
  glVertex3f (inherited::size_[0]+inherited::position_[0],
              inherited::position_[1],
              inherited::position_[2]);
  glTexCoord2f (rep_in, repA+S_in+tilt_in);
  glVertex3f (inherited::position_[0],
              inherited::position_[1],
              inherited::position_[2]);
  glTexCoord2f (rep_in, repB+S_in+tilt_in);
  glVertex3f ( inherited::position_[0],
              -inherited::size_[1]+inherited::position_[1],
               inherited::position_[2]);

  glTexCoord2f (0.0, repA+S_in);
  glVertex3f (inherited::size_[0]+inherited::position_[0],
              inherited::position_[1],
              inherited::position_[2]);
  glTexCoord2f (rep_in, repB+S_in+tilt_in);
  glVertex3f  (inherited::position_[0],
              -inherited::size_[1]+inherited::position_[1],
               inherited::position_[2]);
  glTexCoord2f (0.0, repB+S_in);
  glVertex3f ( inherited::size_[0]+inherited::position_[0],
              -inherited::size_[1]+inherited::position_[1],
               inherited::position_[2]);

  if (blipMirrorT_in) { repA = rep_in; repB = 0.0; }

  glTexCoord2f (0.0, repA+S_in);
  glVertex3f (inherited::size_[0]+inherited::position_[0],
              inherited::size_[1]+inherited::position_[1],
              inherited::position_[2]);
  glTexCoord2f (rep_in, repA+S_in+tilt_in);
  glVertex3f (inherited::position_[0],
              inherited::size_[1]+inherited::position_[1],
              inherited::position_[2]);
  glTexCoord2f (rep_in, repB+S_in+tilt_in);
  glVertex3f (inherited::position_[0],
              inherited::position_[1],
              inherited::position_[2]);

  glTexCoord2f (0.0, repA+S_in);
  glVertex3f (inherited::size_[0]+inherited::position_[0],
              inherited::size_[1]+inherited::position_[1],
              inherited::position_[2]);
  glTexCoord2f (rep_in, repB+S_in+tilt_in);
  glVertex3f (inherited::position_[0],
              inherited::position_[1],
              inherited::position_[2]);
  glTexCoord2f (0.0, repB+S_in);
  glVertex3f (inherited::size_[0]+inherited::position_[0],
              inherited::position_[1],
              inherited::position_[2]);
  //--
  glTexCoord2f (rep_in, repA+S_in+tilt_in);
  glVertex3f (inherited::position_[0],
              inherited::size_[1]+inherited::position_[1],
              inherited::position_[2]);
  glTexCoord2f (0.0, repA+S_in);
  glVertex3f (-inherited::size_[0]+inherited::position_[0],
               inherited::size_[1]+inherited::position_[1],
               inherited::position_[2]);
  glTexCoord2f (0.0, repB+S_in);
  glVertex3f (-inherited::size_[0]+inherited::position_[0],
              inherited::position_[1],
              inherited::position_[2]);

  glTexCoord2f (rep_in, repA+S_in+tilt_in);
  glVertex3f (inherited::position_[0],
              inherited::size_[1]+inherited::position_[1],
              inherited::position_[2]);
  glTexCoord2f (0.0, repB+S_in);
  glVertex3f (-inherited::size_[0]+inherited::position_[0],
              inherited::position_[1],
              inherited::position_[2]);
  glTexCoord2f (rep_in, repB+S_in+tilt_in);
  glVertex3f (inherited::position_[0],
              inherited::position_[1],
              inherited::position_[2]);
  glEnd ();
}

void
Splot_BackgroundMetalSegment::drawSurface (float* c0_clr,
                                           float* c1_clr,
                                           float* r0_clr,
                                           float* r1_clr,
                                           float* r2_clr)
{
  const Configuration_t& configuration =
    SPLOT_CONFIGURATION_SINGLETON::instance ()->get ();
  //float rep = 1.0;
  glBindTexture (GL_TEXTURE_2D, parent_->tex_[BACKGROUND_BASE]);
  glBegin (GL_TRIANGLES); //-- use triangles to prevent color popping on Utah
  if (configuration.gfx_level > 0)
  {
    glColor4fv (c0_clr); glTexCoord2f (1.0, 1.0);
    glVertex3f ( inherited::position_[0],
                -inherited::size_[1]+inherited::position_[1],
                 inherited::position_[2]);
    glColor4fv (c1_clr); glTexCoord2f (1.0, 0.0);
    glVertex3f (inherited::position_[0],
                inherited::position_[1],
                inherited::position_[2]);
    glColor4fv (r2_clr); glTexCoord2f (0.0, 0.0);
    glVertex3f (-inherited::size_[0]+inherited::position_[0],
                 inherited::position_[1],
                 inherited::position_[2]);

    glColor4fv (c0_clr); glTexCoord2f (1.0, 1.0);
    glVertex3f ( inherited::position_[0],
                -inherited::size_[1]+inherited::position_[1],
                 inherited::position_[2]);
    glColor4fv (r2_clr); glTexCoord2f (0.0, 0.0);
    glVertex3f (-inherited::size_[0]+inherited::position_[0],
                 inherited::position_[1],
                 inherited::position_[2]);
    glColor4fv (r0_clr); glTexCoord2f (0.0, 1.0);
    glVertex3f (-inherited::size_[0] + inherited::position_[0],
                -inherited::size_[1] + inherited::position_[1],
                 inherited::position_[2]);

    glColor4fv (r1_clr); glTexCoord2f (0.0, 1.0);
    glVertex3f ( inherited::size_[0]+inherited::position_[0],
                -inherited::size_[1]+inherited::position_[1],
                 inherited::position_[2]);
    glColor4fv (r0_clr); glTexCoord2f (0.0, 0.0);
    glVertex3f (inherited::size_[0]+inherited::position_[0],
                inherited::position_[1],
                inherited::position_[2]);
    glColor4fv (c1_clr); glTexCoord2f (1.0, 0.0);
    glVertex3f (inherited::position_[0],
                inherited::position_[1],
                inherited::position_[2]);

    glColor4fv (r1_clr); glTexCoord2f (0.0, 1.0);
    glVertex3f ( inherited::size_[0]+inherited::position_[0],
                -inherited::size_[1]+inherited::position_[1],
                 inherited::position_[2]);
    glColor4fv (c1_clr); glTexCoord2f (1.0, 0.0);
    glVertex3f (inherited::position_[0],
                inherited::position_[1],
                inherited::position_[2]);
    glColor4fv (c0_clr); glTexCoord2f (1.0, 1.0);
    glVertex3f ( inherited::position_[0],
                -inherited::size_[1]+inherited::position_[1],
                 inherited::position_[2]);

    glColor4fv (r0_clr); glTexCoord2f (0.0, 0.0);
    glVertex3f (inherited::size_[0]+inherited::position_[0],
                inherited::position_[1],
                inherited::position_[2]);
    glColor4fv (r1_clr); glTexCoord2f (0.0, 1.0);
    glVertex3f (inherited::size_[0]+inherited::position_[0],
                inherited::size_[1]+inherited::position_[1]+0.1F,
                inherited::position_[2]);
    glColor4fv (c0_clr); glTexCoord2f (1.0, 1.0);
    glVertex3f (inherited::position_[0],
                inherited::size_[1]+inherited::position_[1]+0.1F,
                inherited::position_[2]);

    glColor4fv (r0_clr); glTexCoord2f (0.0, 0.0);
    glVertex3f (inherited::size_[0]+inherited::position_[0],
                inherited::position_[1],
                inherited::position_[2]);
    glColor4fv (c0_clr); glTexCoord2f (1.0, 1.0);
    glVertex3f (inherited::position_[0],
                inherited::size_[1]+inherited::position_[1]+0.1F,
                inherited::position_[2]);
    glColor4fv (c1_clr); glTexCoord2f (1.0, 0.0);
    glVertex3f (inherited::position_[0],
                inherited::position_[1],
                inherited::position_[2]);

    glColor4fv (c1_clr); glTexCoord2f (1.0, 0.0);
    glVertex3f (inherited::position_[0],
                inherited::position_[1],
                inherited::position_[2]);
    glColor4fv (c0_clr); glTexCoord2f (1.0, 1.0);
    glVertex3f (inherited::position_[0],
                inherited::size_[1]+inherited::position_[1]+0.1F,
                inherited::position_[2]);
    glColor4fv (r0_clr); glTexCoord2f (0.0, 1.0);
    glVertex3f (-inherited::size_[0]+inherited::position_[0],
                 inherited::size_[1]+inherited::position_[1]+0.1F,
                 inherited::position_[2]);

    glColor4fv (c1_clr); glTexCoord2f (1.0, 0.0);
    glVertex3f (inherited::position_[0],
                inherited::position_[1],
                inherited::position_[2]);
    glColor4fv (r0_clr); glTexCoord2f (0.0, 1.0);
    glVertex3f (-inherited::size_[0]+inherited::position_[0],
                 inherited::size_[1]+inherited::position_[1]+0.1F,
                 inherited::position_[2]);
    glColor4fv (r2_clr); glTexCoord2f (0.0, 0.0);
    glVertex3f (-inherited::size_[0]+inherited::position_[0],
                 inherited::position_[1],
                 inherited::position_[2]);
  } // end IF
  else
  {
    //float b = -0.1;
    glColor4f (c0_clr[0]+0.1F, c0_clr[1]+0.1F, c0_clr[2]+0.1F, c0_clr[3]);
    glTexCoord2f (1.0, 1.0);
    glVertex3f ( inherited::position_[0],
                -inherited::size_[1]+inherited::position_[1],
                 inherited::position_[2]);
    glTexCoord2f (1.0, 0.0);
    glVertex3f (inherited::position_[0],
                inherited::position_[1],
                inherited::position_[2]);
    glTexCoord2f (0.0, 0.0);
    glVertex3f (-inherited::size_[0]+inherited::position_[0],
                 inherited::position_[1],
                 inherited::position_[2]);

    glTexCoord2f (1.0, 1.0);
    glVertex3f ( inherited::position_[0],
                -inherited::size_[1]+inherited::position_[1],
                 inherited::position_[2]);
    glTexCoord2f (0.0, 0.0);
    glVertex3f (-inherited::size_[0]+inherited::position_[0],
                 inherited::position_[1],
                 inherited::position_[2]);
    glTexCoord2f (0.0, 1.0);
    glVertex3f (-inherited::size_[0]+inherited::position_[0],
                -inherited::size_[1]+inherited::position_[1],
                 inherited::position_[2]);

    glTexCoord2f (0.0, 1.0);
    glVertex3f ( inherited::size_[0]+inherited::position_[0],
                -inherited::size_[1]+inherited::position_[1],
                 inherited::position_[2]);
    glTexCoord2f (0.0, 0.0);
    glVertex3f (inherited::size_[0]+inherited::position_[0],
                inherited::position_[1],
                inherited::position_[2]);
    glTexCoord2f (1.0, 0.0);
    glVertex3f (inherited::position_[0],
                inherited::position_[1],
                inherited::position_[2]);

    glTexCoord2f (0.0, 1.0); 
    glVertex3f ( inherited::size_[0]+inherited::position_[0],
                -inherited::size_[1]+inherited::position_[1],
                 inherited::position_[2]);
    glTexCoord2f (1.0, 0.0);
    glVertex3f (inherited::position_[0],
                inherited::position_[1],
                inherited::position_[2]);
    glTexCoord2f (1.0, 1.0);
    glVertex3f ( inherited::position_[0],
                -inherited::size_[1]+inherited::position_[1],
                 inherited::position_[2]);

    glTexCoord2f (0.0, 0.0);
    glVertex3f (inherited::size_[0]+inherited::position_[0],
                inherited::position_[1],
                inherited::position_[2]);
    glTexCoord2f (0.0, 1.0);
    glVertex3f (inherited::size_[0]+inherited::position_[0],
                inherited::size_[1]+inherited::position_[1],
                inherited::position_[2]);
    glTexCoord2f (1.0, 1.0);
    glVertex3f (inherited::position_[0],
                inherited::size_[1]+inherited::position_[1],
                inherited::position_[2]);

    glTexCoord2f (0.0, 0.0);
    glVertex3f (inherited::size_[0]+inherited::position_[0],
                inherited::position_[1],
                inherited::position_[2]);
    glTexCoord2f (1.0, 1.0);
    glVertex3f (inherited::position_[0],
                inherited::size_[1]+inherited::position_[1],
                inherited::position_[2]);
    glTexCoord2f (1.0, 0.0);
    glVertex3f (inherited::position_[0],
                inherited::position_[1],
                inherited::position_[2]);

    glTexCoord2f (1.0, 0.0);
    glVertex3f (inherited::position_[0],
                inherited::position_[1],
                inherited::position_[2]);
    glTexCoord2f (1.0, 1.0);
    glVertex3f (inherited::position_[0],
                inherited::size_[1]+inherited::position_[1],
                inherited::position_[2]);
    glTexCoord2f (0.0, 1.0);
    glVertex3f (-inherited::size_[0]+inherited::position_[0],
                 inherited::size_[1]+inherited::position_[1],
                 inherited::position_[2]);

    glTexCoord2f (1.0, 0.0);
    glVertex3f (inherited::position_[0],
                inherited::position_[1],
                inherited::position_[2]);
    glTexCoord2f (0.0, 1.0);
    glVertex3f (-inherited::size_[0]+inherited::position_[0],
                 inherited::size_[1]+inherited::position_[1],
                 inherited::position_[2]);
    glTexCoord2f (0.0, 0.0);
    glVertex3f (-inherited::size_[0]+inherited::position_[0],
                 inherited::position_[1],
                 inherited::position_[2]);
  } // end ELSE
  glEnd ();
}

#ifdef EXPERIMENTAL
#define glTexCoord2f_M0 (a, b) glMultiTexCoord2fARB (GL_TEXTURE0_ARB, a, b)
#define glTexCoord2f_M1 (a, b) glMultiTexCoord2fARB (GL_TEXTURE1_ARB, a, b)
void
Splot_BackgroundMetalSegment::drawMultiTex (float rep, float S, float tilt, bool blipMirrorT,
                                            float *c0_clr,
                                            float *c1_clr,
                                            float *r0_clr,
                                            float *r1_clr,
                                            float *r2_clr)
{
  float repA = 0.0;
  float repB = rep;
  glActiveTextureARB (GL_TEXTURE0_ARB);
  glBindTexture (GL_TEXTURE_2D, parent->tex[Ground::Blip]);
  glTexEnvf (GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);

  glActiveTextureARB (GL_TEXTURE1_ARB);
  glBindTexture (GL_TEXTURE_2D, parent->tex[Ground::Base]);
  glTexEnvf (GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);
  glEnable (GL_TEXTURE_2D);

  //	glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);	

  glBegin (GL_TRIANGLES); //-- use triangles to prevent color popping on Utah

  glTexCoord2f_M0 (rep, repA+S+tilt); glColor4fv (c1_clr); glTexCoord2f_M1 (1.0, 0.0); glVertex3f (		  pos[0],		   pos[1], pos[2]);
  glTexCoord2f_M0 (0.0, repA+S);      glColor4fv (r2_clr); glTexCoord2f_M1 (0.0, 0.0); glVertex3f (-size[0]+pos[0],		   pos[1], pos[2]);
  glTexCoord2f_M0 (0.0, repB+S);      glColor4fv (r0_clr); glTexCoord2f_M1 (0.0, 1.0); glVertex3f (-size[0]+pos[0], -size[1]+pos[1], pos[2]);

  glTexCoord2f_M0 (rep, repA+S+tilt); glColor4fv (c1_clr); glTexCoord2f_M1 (1.0, 0.0); glVertex3f (		  pos[0],		   pos[1], pos[2]);
  glTexCoord2f_M0 (0.0, repB+S);      glColor4fv (r0_clr); glTexCoord2f_M1 (0.0, 1.0); glVertex3f (-size[0]+pos[0], -size[1]+pos[1], pos[2]);
  glTexCoord2f_M0 (rep, repB+S+tilt); glColor4fv (c0_clr); glTexCoord2f_M1 (1.0, 1.0); glVertex3f (		  pos[0], -size[1]+pos[1], pos[2]);

  glTexCoord2f_M0 (0.0, repA+S);      glColor4fv (r0_clr); glTexCoord2f_M1 (0.0, 0.0); glVertex3f ( size[0]+pos[0],		   pos[1], pos[2]);
  glTexCoord2f_M0 (rep, repA+S+tilt); glColor4fv (c1_clr); glTexCoord2f_M1 (1.0, 0.0); glVertex3f (		  pos[0],		   pos[1], pos[2]);
  glTexCoord2f_M0 (rep, repB+S+tilt); glColor4fv (c0_clr); glTexCoord2f_M1 (1.0, 1.0); glVertex3f (		  pos[0], -size[1]+pos[1], pos[2]);

  glTexCoord2f_M0 (0.0, repA+S);      glColor4fv (r0_clr); glTexCoord2f_M1 (0.0, 0.0); glVertex3f ( size[0]+pos[0],		   pos[1], pos[2]);
  glTexCoord2f_M0 (rep, repB+S+tilt); glColor4fv (c0_clr); glTexCoord2f_M1 (1.0, 1.0); glVertex3f (		  pos[0], -size[1]+pos[1], pos[2]);
  glTexCoord2f_M0 (0.0, repB+S);      glColor4fv (r1_clr); glTexCoord2f_M1 (0.0, 1.0); glVertex3f ( size[0]+pos[0], -size[1]+pos[1], pos[2]);
 
  if (blipMirrorT) { repA = rep; repB = 0.0; }

  glTexCoord2f_M0 (0.0, repA+S);      glColor4fv (r1_clr); glTexCoord2f_M1 (0.0, 1.0); glVertex3f ( size[0]+pos[0],  size[1]+pos[1], pos[2]);
  glTexCoord2f_M0 (rep, repA+S+tilt); glColor4fv (c0_clr); glTexCoord2f_M1 (1.0, 1.0); glVertex3f (		  pos[0],  size[1]+pos[1], pos[2]);
  glTexCoord2f_M0 (rep, repB+S+tilt); glColor4fv (c1_clr); glTexCoord2f_M1 (1.0, 0.0); glVertex3f (		  pos[0],		   pos[1], pos[2]);

  glTexCoord2f_M0 (0.0, repA+S);      glColor4fv (r1_clr); glTexCoord2f_M1 (0.0, 1.0); glVertex3f ( size[0]+pos[0],  size[1]+pos[1], pos[2]);
  glTexCoord2f_M0 (rep, repB+S+tilt); glColor4fv (c1_clr); glTexCoord2f_M1 (1.0, 0.0); glVertex3f (		  pos[0],		   pos[1], pos[2]);
  glTexCoord2f_M0 (0.0, repB+S);      glColor4fv (r0_clr); glTexCoord2f_M1 (0.0, 0.0); glVertex3f ( size[0]+pos[0],		   pos[1], pos[2]);
  //--
  glTexCoord2f_M0 (rep, repA+S+tilt); glColor4fv (c0_clr); glTexCoord2f_M1 (1.0, 1.0); glVertex3f (		  pos[0],  size[1]+pos[1], pos[2]);
  glTexCoord2f_M0 (0.0, repA+S);      glColor4fv (r0_clr); glTexCoord2f_M1 (0.0, 1.0); glVertex3f (-size[0]+pos[0],  size[1]+pos[1], pos[2]);
  glTexCoord2f_M0 (0.0, repB+S);      glColor4fv (r2_clr); glTexCoord2f_M1 (0.0, 0.0); glVertex3f (-size[0]+pos[0],		   pos[1], pos[2]);

  glTexCoord2f_M0 (rep, repA+S+tilt); glColor4fv (c0_clr); glTexCoord2f_M1 (1.0, 1.0); glVertex3f (		  pos[0],  size[1]+pos[1], pos[2]);
  glTexCoord2f_M0 (0.0, repB+S);      glColor4fv (r2_clr); glTexCoord2f_M1 (0.0, 0.0); glVertex3f (-size[0]+pos[0],		   pos[1], pos[2]);
  glTexCoord2f_M0 (rep, repB+S+tilt); glColor4fv (c1_clr); glTexCoord2f_M1 (1.0, 0.0); glVertex3f (		  pos[0],		   pos[1], pos[2]);

  glEnd ();

  glDisable (GL_TEXTURE_2D);
  glActiveTextureARB (GL_TEXTURE0_ARB);
  glTexEnvf (GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
}
#endif // EXPERIMENTAL
