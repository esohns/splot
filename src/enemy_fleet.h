#ifndef ENEMYFLEET_H
#define ENEMYFLEET_H

#ifdef HAVE_CONFIG_H
#include "splot-config.h"
#endif
#if defined (HAVE_APPLE_OPENGL_FRAMEWORK) || defined (HAVE_OPENGL_GL_H)
#include <OpenGL/gl.h>
#else
#include "GL/gl.h"
#endif

#include "ace/Free_List.h"
#include "ace/Synch.h"

#include "enemy_aircraft.h"

class Splot_EnemyFleet
 : public ACE_Locked_Free_List<Splot_EnemyAircraft, ACE_SYNCH_NULL_MUTEX>
{
 public:
  Splot_EnemyFleet ();
  ~Splot_EnemyFleet ();

  void drawGL ();
  void clear ();

  void addEnemy (Splot_EnemyAircraft*);
  void killEnemy (Splot_EnemyAircraft*);
  void destroyAll ();
  void retarget (EnemyAircraftType_t, Splot_GameElement*);

  void toFirst ();
  Splot_EnemyAircraft* getShip ();

  void update ();

  void loadTextures ();
  void deleteTextures ();

  private:
   typedef ACE_Locked_Free_List<Splot_EnemyAircraft, ACE_SYNCH_NULL_MUTEX> inherited;

  inline void drawQuad (float szx, float szy)
  {
    glBegin (GL_TRIANGLE_STRIP);
    glTexCoord2f (1.0, 0.0); glVertex3f ( szx,  szy, 0.0);
    glTexCoord2f (0.0, 0.0); glVertex3f (-szx,  szy, 0.0);
    glTexCoord2f (1.0, 1.0); glVertex3f ( szx, -szy, 0.0);
    glTexCoord2f (0.0, 1.0); glVertex3f (-szx, -szy, 0.0);
    glEnd ();
  }

  void bossExplosion (Splot_EnemyAircraft*);

  GLuint               shipTex_[ENEMYAIRCRAFT_MAX_TYPES];
  GLuint               extraTex_[ENEMYAIRCRAFT_MAX_TYPES];

  Splot_EnemyAircraft* currentShip_;
};

#endif // ENEMYFLEET_H
