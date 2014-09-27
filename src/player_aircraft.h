#ifndef PLAYERAIRCRAFT_H
#define PLAYERAIRCRAFT_H

#ifdef HAVE_CONFIG_H
#include "splot-config.h"
#endif

#include <algorithm>

#ifdef USE_SDL
#include "SDL_opengl.h"
#elif defined (HAVE_APPLE_OPENGL_FRAMEWORK) || defined (HAVE_OPENGL_GL_H)
#include <OpenGL/gl.h>
#else
#include "GL/gl.h"
#endif

#include "game_element.h"

#include "defines.h"

class Splot_EnemyFleet;
class Splot_PowerUps;
class Splot_StatusDisplay;

class Splot_PlayerAircraft
 : public Splot_GameElement
{
  friend class Splot_StatusDisplay;

 public:
  Splot_PlayerAircraft ();
  virtual ~Splot_PlayerAircraft ();

  void moveEvent (int, int); // x/y
  void moveEventMouse (int, int); // x/y

  void fireGun (bool); // status
  void shootGun ();

  void useItem (int); // index
  void useItem ();
  void nextItem ();

  void doDamage (float); // damage
  void ammoDamage (float, const float (&)[3]); // damage/direction
  void checkForCollisions (Splot_EnemyFleet*);
  void checkForPowerUps (Splot_PowerUps*);

  void setShips (int);
  void addScore (float);
  void addShip (bool = false); // score
  void loseShip ();

  void startDeath ();

  void update ();
  void drawGL ();
  void reset ();
  void fullRepair ();

  inline bool isVisible() { return !dontShow_; }

  inline void getSize (float (&size_out)[2]) { std::copy (size_, size_+(sizeof (size_)/sizeof (size_[0])), size_out); }

  GLuint texAircraft_;
  GLuint texBomb_;

  void loadTextures ();
  void deleteTextures ();

 protected:
  void setAmmoStock (int, float);
  void deathExplosions ();

  float gunFlash0_[NUM_PLAYER_AMMUNITION_TYPES];
  float gunFlash1_[NUM_PLAYER_AMMUNITION_TYPES];
  bool gunActive_[NUM_PLAYER_AMMUNITION_TYPES];

 private:
  typedef Splot_GameElement inherited;

  //float epos_[3];
  float size_[2];
  float bound_[2][2];

  float deathCircle_[DEATH_SPIKES][2];
  int dontShow_;

  //-- secondary movement
  float move_[2];
  float secondaryMove_[2];

  inline void drawQuad (float szx_in, float szy_in)
  {
    glBegin (GL_TRIANGLE_STRIP);
    glTexCoord2f (1.0, 0.0); glVertex3f ( szx_in,  szy_in, 0.0);
    glTexCoord2f (0.0, 0.0); glVertex3f (-szx_in,  szy_in, 0.0);
    glTexCoord2f (1.0, 1.0); glVertex3f ( szx_in, -szy_in, 0.0);
    glTexCoord2f (0.0, 1.0); glVertex3f (-szx_in, -szy_in, 0.0);
    glEnd ();
  }

  //-- tip variables
  int tipShipPast_;
  int tipSuperShield_;
};

#endif // PLAYERAIRCRAFT_H
