#ifndef ENEMYBULLETS_H
#define ENEMYBULLETS_H

//#ifdef HAVE_CONFIG_H
//#include "splot-config.h"
//#endif

//#include "compatibility.h"
//
#if defined(HAVE_APPLE_OPENGL_FRAMEWORK) || defined(HAVE_OPENGL_GL_H)
#include <OpenGL/gl.h>
#else
#include "GL/gl.h"
#endif

#include "defines.h"
#include "types.h"

class Splot_PlayerAircraft;

class Splot_EnemyBullets
{
 public:
  Splot_EnemyBullets ();
  ~Splot_EnemyBullets ();

  static void initialize ();

  void add (int,         // type
            float p[3],  // position
            float v[3]); // velocity
  void update ();

  void checkForHits (Splot_PlayerAircraft*);

  void drawGL ();
  void clear ();

  void loadTextures ();
  void deleteTextures ();

 private:
  void remove (const Bullet_t&);

  Bullets_t bullets_[NUM_ENEMY_AMMO_TYPES];
  static float bullet_size[NUM_ENEMY_AMMO_TYPES][2];
  static float bullet_damage[NUM_ENEMY_AMMO_TYPES];
  //Bullets_t bulletPool_;

  GLuint texBullet_[NUM_ENEMY_AMMO_TYPES];
};

#endif // ENEMYBULLETS_H
