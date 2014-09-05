#ifndef POWERUP_H
#define POWERUP_H

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

#include "ace/Free_List.h"
#include "ace/Synch.h"

#include "defines.h"
#include "game_element.h"

enum PowerUpCategory_t
{
  POWERUPCATEGORY_INVALID = -1,
  POWERUPCATEGORY_AMMUNITION = 0,
  POWERUPCATEGORY_REPAIR_SHIELD,
  POWERUPCATEGORY_SPECIAL,
  ///////////////////////////////////////
  POWERUPCATEGORY_MAX_TYPES
};

enum PowerUpType_t
{
  POWERUP_INVALID = -1,
  POWERUP_AMMUNITION_0 = 0,
  POWERUP_AMMUNITION_1,
  POWERUP_AMMUNITION_2,
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
                 const float (&)[3], // position
                 const float (&)[3], // translation vector
                 float = 1.0);       // potency
  virtual ~Splot_PowerUp ();

//  virtual Splot_PowerUp* get_next () { return dynamic_cast<Splot_PowerUp*> (inherited::get_next ()); };
  virtual Splot_PowerUp* get_next () { return (Splot_PowerUp*)inherited::get_next (); };

  PowerUpType_t type_;
  float         potency_;

  static unsigned int count;

 private:
  typedef Splot_GameElement inherited;
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

  void dump ();

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
