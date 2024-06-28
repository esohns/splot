#ifndef ENEMYAIRCRAFT_H
#define ENEMYAIRCRAFT_H

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

#include "types.h"
#include "game_element.h"

enum EnemyAircraftType_t
{
  ENEMYAIRCRAFT_INVALID = -1,
  ENEMYAIRCRAFT_STRAIGHT = 0,
  ENEMYAIRCRAFT_OMNI,
  ENEMYAIRCRAFT_RAYGUN,
  ENEMYAIRCRAFT_TANK,
  ENEMYAIRCRAFT_GNAT,
  ENEMYAIRCRAFT_BOSS_0,
  ENEMYAIRCRAFT_BOSS_1,
  ///////////////////////////////////////
  MAX_ENEMYAIRCRAFT_TYPES
};

class Splot_EnemyFleet;

class Splot_EnemyAircraft
 : public Splot_GameElement
{
  friend class Splot_EnemyFleet;

 public:
  Splot_EnemyAircraft (); // *WARNING*: do NOT use !
  virtual ~Splot_EnemyAircraft ();

  virtual Splot_EnemyAircraft* get_next () { return (Splot_EnemyAircraft*)inherited::get_next (); };

  void update ();
  void reset (const float (&)[3], // position
              float = 1.0);       // random movement factor

  EnemyAircraftType_t type () const { return type_; };

  bool checkHit (const Bullet_t&); // bullet
  void setTarget (Splot_GameElement* target_in) { target_ = target_in; }

  void drawGL (GLuint tex, GLuint xtraTex);

  float size_[2];

  float damage_;
  float baseDamage_;

  float randomMoveX_;
  float collisionMove_;
  float secondaryMove_[2];

  float preFire_;

  static Splot_EnemyAircraft* make (EnemyAircraftType_t, // type
                                    const float (&)[3],  // position
                                    float = 1.0);        // random movement factor
  static void printNumAllocated ();

 protected:
  Splot_EnemyAircraft (EnemyAircraftType_t, // type
                       const float (&)[3],  // position
                       float = 1.0);        // random movement factor

  void calcShootInterval ();
  void move ();

  Splot_EnemyAircraft* over_;

  float shootVec_[3];
  int   shootPause_;
  int   shootInterval_;
  int   shootSwap_;

  float lastMove_[2];

  float speedAdjustment_;

  Splot_GameElement* target_;

  inline void drawQuad (float szx, float szy)
  {
    glBegin (GL_TRIANGLE_STRIP);
    glTexCoord2f (1.0, 0.0); glVertex3f ( szx,  szy, 0.0);
    glTexCoord2f (0.0, 0.0); glVertex3f (-szx,  szy, 0.0);
    glTexCoord2f (1.0, 1.0); glVertex3f ( szx, -szy, 0.0);
    glTexCoord2f (0.0, 1.0); glVertex3f (-szx, -szy, 0.0);
    glEnd ();
  }

 private:
  typedef Splot_GameElement inherited;

  EnemyAircraftType_t type_;

  static unsigned int count;
};

#endif // ENEMYAIRCRAFT_H
