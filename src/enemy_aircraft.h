#ifndef ENEMYAIRCRAFT_H
#define ENEMYAIRCRAFT_H

#ifdef HAVE_CONFIG_H
#include "splot-config.h"
#endif
#if defined (HAVE_APPLE_OPENGL_FRAMEWORK) || defined (HAVE_OPENGL_GL_H)
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
  ENEMYAIRCRAFT_MAX_TYPES
};

class Splot_EnemyFleet;
class EnemyAircraft_Boss01;

class Splot_EnemyAircraft
 : public Splot_GameElement
{
  friend class Splot_EnemyFleet;
  //friend class ScreenItemAdd;
  friend class EnemyAircraft_Boss01;

 public:
  Splot_EnemyAircraft (); // *WARNING*: do NOT use !
  Splot_EnemyAircraft (EnemyAircraftType_t, // type
                       float p[3],          // position
                       float = 1.0);        // randFact
  virtual ~Splot_EnemyAircraft ();

  virtual Splot_EnemyAircraft* get_next () { return dynamic_cast<Splot_EnemyAircraft*> (inherited::get_next ()); };

  void update ();
  void reset (float p[3],   // position
              float = 1.0); // randFact

  EnemyAircraftType_t type () const { return type_; };

  bool checkHit (const bullet_t&); // bullet
  void setTarget (Splot_GameElement* target_in) { target_ = target_in; }

  void drawGL (GLuint tex, GLuint xtraTex);

  float size_[2];
  float damage_;
  float baseDamage_;
  float collisionMove_;
  float secondaryMove_[2];
  float preFire_;

  static void printNumAllocated ();

 protected:
  void calcShootInterval ();
  void move ();

  Splot_EnemyAircraft* over_;

  float shootVec_[3];
  int   shootPause_;
  int   shootInterval_;
  int   shootSwap_;

  float randMoveX_;
  float lastMoveX_;
  float lastMoveY_;

  float speedAdj_;

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
