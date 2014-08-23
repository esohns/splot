#ifndef PLAYERBULLETS_H
#define PLAYERBULLETS_H

#ifdef HAVE_CONFIG_H
#include "splot-config.h"
#endif
#if defined(HAVE_APPLE_OPENGL_FRAMEWORK) || defined(HAVE_OPENGL_GL_H)
#include <OpenGL/gl.h>
#else
#include "GL/gl.h"
#endif

#include "defines.h"
#include "types.h"

class Splot_EnemyFleet;

class Splot_PlayerBullets
{
 public:
  Splot_PlayerBullets ();
  virtual ~Splot_PlayerBullets ();

  static void initialize ();

  void addBullet (int, float p[3]); // type / position
  //void killBullet (const bullet_t&);
  void checkForHits (Splot_EnemyFleet*);

  void update ();
  void drawGL ();
  void clear ();

  void print (int); // type

  void loadTextures ();
  void deleteTextures ();

 private:
  //static bullet_t* make ();

  Bullets_t bullets_[NUM_PLAYER_AMMO_TYPES];
  static float bullet_size[NUM_PLAYER_AMMO_TYPES][2];
  static float bullet_damage[NUM_PLAYER_AMMO_TYPES];
  //Bullets_t bulletPool_;

  GLuint bulletTex_[NUM_PLAYER_AMMO_TYPES];
};

#endif // PLAYERBULLETS_H
