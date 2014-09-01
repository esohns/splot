#ifndef STATUSDISPLAY_H
#define STATUSDISPLAY_H

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

#include "defines.h"

// forward declarations
class Splot_PlayerAircraft;

class Splot_StatusDisplay
{
 public:
  Splot_StatusDisplay ();
  ~Splot_StatusDisplay ();

  void darkenGL ();
  void drawGL (Splot_PlayerAircraft*);

  void enemyWarning (float v) {if (v > enemyWarn_) enemyWarn_ = v;}

  void loadTextures ();
  void deleteTextures ();

  float ammoAlpha_;
  float damageAlpha_;
  float shieldAlpha_;

 private:
  GLuint texStat_;
  GLuint texShld_;
  GLuint texTop_;
  GLuint texPlayerSuper_;
  GLuint texPlayerShield_;
  GLuint texPlayerAmmoFlash_[NUM_PLAYER_AMMO_TYPES];
  GLuint texUseFocus_;
  GLuint texUseItem_[MAX_PLAYER_ITEMS];

  bool  blink_;

  inline void drawQuad (float szx, float szy)
  {
    glBegin (GL_TRIANGLE_STRIP);
    glTexCoord2f (1.0, 0.0); glVertex3f ( szx,  szy, 0.0);
    glTexCoord2f (0.0, 0.0); glVertex3f (-szx,  szy, 0.0);
    glTexCoord2f (1.0, 1.0); glVertex3f ( szx, -szy, 0.0);
    glTexCoord2f (0.0, 1.0); glVertex3f (-szx, -szy, 0.0);
    glEnd ();
  }

  int tipShipShow_;
  int tipSuperShow_;

  float enemyWarn_;

  static float statPosAmmo[3];
  static float statPosShld[3];
  static float statClrWarn[4];
  static float statClrZero[4];
  static float statClrAmmo[NUM_PLAYER_AMMO_TYPES][4];
};

#endif // STATUSDISPLAY_H
