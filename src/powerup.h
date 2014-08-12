#ifndef POWERUP_H
#define POWERUP_H

#ifdef HAVE_CONFIG_H
#include "splot-config.h"
#endif
#if defined (HAVE_APPLE_OPENGL_FRAMEWORK) || defined (HAVE_OPENGL_GL_H)
#include "OpenGL/gl.h"
#else
#include "GL/gl.h"
#endif

#include "ace/Free_List.h"
#include "ace/Synch.h"

#include "game_element.h"

#define WOBBLE_0 45
#define WOBBLE_1 75

enum PowerUpType_t
{
  POWERUP_INVALID = -1,
  POWERUP_AMMO_0 = 0,
  POWERUP_AMMO_1,
  POWERUP_AMMO_2,
  POWERUP_REPAIR,
  POWERUP_SHIELD,
  POWERUP_SHIELD_SUPER,
  ///////////////////////////////////////
  POWERUP_MAX_TYPES
};

// forward declarations
class Splot_PowerUps;

class Splot_PowerUp
 : public Splot_GameElement
{
  friend class Splot_PowerUps;

 public:
  Splot_PowerUp (); // *WARNING*: do NOT use !
  Splot_PowerUp (PowerUpType_t, // type
                 float p[3],    // position
                 float v[3],    // velocity
                 float = 1.0);  // potency
  virtual ~Splot_PowerUp ();

  virtual Splot_PowerUp* get_next() { return dynamic_cast<Splot_PowerUp*> (inherited::get_next()); };

  PowerUpType_t type () { return type_; }

  float potency_;

  static unsigned int count_;

 private:
  typedef Splot_GameElement inherited;

  PowerUpType_t type_;
};

class Splot_PowerUps
 : public ACE_Locked_Free_List<Splot_PowerUp, ACE_SYNCH_NULL_MUTEX>
{
 public:
  Splot_PowerUps ();
  virtual ~Splot_PowerUps ();

  Splot_PowerUp* getFirst ();
  //void addPowerUp (Splot_PowerUp*);
  void remove (Splot_PowerUp*);

  void update ();
  void drawGL ();
  void clear ();

  void loadTextures ();
  void deleteTextures ();

 private:
  typedef ACE_Locked_Free_List<Splot_PowerUp, ACE_SYNCH_NULL_MUTEX> inherited;

  //Splot_PowerUp* pwrUpRoot_;
  Splot_PowerUp* currentPwrUp_;
  float pwrUpSize_[POWERUP_MAX_TYPES][2];
  float pwrUpColor_[POWERUP_MAX_TYPES][4];

  GLuint tex_[POWERUP_MAX_TYPES];
  GLuint pwrTex_;

  float speed_;
  float wobble_0_[WOBBLE_0];
  float wobble_1_[WOBBLE_1];
};

#endif // POWERUP_H
