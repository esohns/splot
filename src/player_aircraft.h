#ifndef PLAYERAIRCRAFT_H
#define PLAYERAIRCRAFT_H

#ifdef HAVE_CONFIG_H
#include "splot-config.h"
#endif

//#include "compatibility.h"

#if defined(HAVE_APPLE_OPENGL_FRAMEWORK) || defined(HAVE_OPENGL_GL_H)
#include <OpenGL/gl.h>
#else
#include "GL/gl.h"
#endif

#include "game_element.h"

#include "defines.h"

//class Splot_State;
class Splot_EnemyFleet;
class Splot_PowerUps;

class Splot_PlayerAircraft
 : public Splot_GameElement
{
  //friend class StatusDisplay;

 public:
  Splot_PlayerAircraft ();
  virtual ~Splot_PlayerAircraft ();

  void moveEvent (int, int); // x/y
  void fireGun (bool); // status
  void shootGun ();
  void useItem (int); // index
  void useItem ();
  void nextItem ();

  void doDamage (float); // damage
  void ammoDamage (float, float v[3]); // damage/direction
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
  void newGame ();
  void fullRepair ();

  inline bool isVisible() { return !dontShow_; }

  inline float getSize (int i) { return size_[i]; }

  GLuint heroTex_;
  GLuint bombTex_;

  void loadTextures ();
  void deleteTextures ();

 protected:
  void setAmmoStock (int, float);
  void deathExplosions ();

  float gunFlash0_[NUM_PLAYER_AMMO_TYPES];
  float gunFlash1_[NUM_PLAYER_AMMO_TYPES];
  bool gunActive_[NUM_PLAYER_AMMO_TYPES];

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

  inline void drawQuad (float szx, float szy)
  {
    glBegin (GL_TRIANGLE_STRIP);
    glTexCoord2f (1.0, 0.0); glVertex3f ( szx,  szy, 0.0);
    glTexCoord2f (0.0, 0.0); glVertex3f (-szx,  szy, 0.0);
    glTexCoord2f (1.0, 1.0); glVertex3f ( szx, -szy, 0.0);
    glTexCoord2f (0.0, 1.0); glVertex3f (-szx, -szy, 0.0);
    glEnd ();
  }

  //-- tip variables
  int tipShipPast_;
  int tipSuperShield_;
};

#endif // PLAYERAIRCRAFT_H