#ifndef EXPLOSION_H
#define EXPLOSION_H

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

#include "types.h"

enum ExplosionType_t
{
  EXPLOSION_INVALID = -1,
  EXPLOSION_EFFECT_ELECTRIC = 0,
  EXPLOSION_EFFECT_GLITTER,
  EXPLOSION_ENEMY_AMMUNITION_0,
  EXPLOSION_ENEMY_AMMUNITION_1,
  EXPLOSION_ENEMY_AMMUNITION_2,
  EXPLOSION_ENEMY_AMMUNITION_3,
  EXPLOSION_ENEMY_AMMUNITION_4,
  EXPLOSION_ENEMY_DAMAGED,
  EXPLOSION_ENEMY_DESTROYED,
  EXPLOSION_LIFE_ADD,
  EXPLOSION_LIFE_LOSE,
  EXPLOSION_LIFE_SCORE,
  EXPLOSION_PLAYER_AMMUNITION_0,
  EXPLOSION_PLAYER_AMMUNITION_1,
  EXPLOSION_PLAYER_AMMUNITION_2,
  EXPLOSION_PLAYER_DAMAGED,
  EXPLOSION_PLAYER_DESTROYED,
  EXPLOSION_PLAYER_SHIELD,
  EXPLOSION_POWER_BURST,
  ///////////////////////////////////////
  MAX_EXPLOSION_TYPES
};

// forward declarations
class Splot_State;

class Splot_Explosions
{
 friend class Splot_State;

 public:
  Explosion_t* add (ExplosionType_t t, float p[3], int age = 0, float size = 1.0);
  Explosion_t* addEffect (ExplosionType_t t, float p[3], float v[3], float clr[4], int age = 0, float size = 1.0);

  void update ();
  void drawGL ();
  void clear ();

 private:
  Splot_Explosions ();
  ~Splot_Explosions ();

  Explosions_t explosions_[MAX_EXPLOSION_TYPES];
  //Explosions_t* exploPool_;
  GLuint tex_[MAX_EXPLOSION_TYPES];
  float  exploSize_[MAX_EXPLOSION_TYPES][2];
  float  exploStay_[MAX_EXPLOSION_TYPES];
  /**< # frames to not allow explosions. [0] = count, [1] = base, [2] = flag */
  float  exploPause_[MAX_EXPLOSION_TYPES][3];

  void draw (ExplosionType_t);
  void drawAmmo (ExplosionType_t);
  void drawBurst (ExplosionType_t);
  void drawShields (ExplosionType_t);
  void drawLife (ExplosionType_t);
  void drawElectric (ExplosionType_t);
  void drawGlitter (ExplosionType_t);

  void loadTextures ();
  void deleteTextures ();
};

#endif // EXPLOSION_H
