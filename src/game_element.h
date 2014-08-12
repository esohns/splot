#ifndef GAMEELEMENT_H
#define GAMEELEMENT_H

#include "ace/OS.h"

enum GameElementType_t
{
  GAMEELEMENT_INVALID = -1,
  GAMEELEMENT_BULLET = 0,
  GAMEELEMENT_BONUS,
  GAMEELEMENT_ENEMY,
  GAMEELEMENT_PLAYER,
  GAMEELEMENT_POWERUP,
  ///////////////////////////////////////
  GAMEELEMENT_MAX_TYPES
};

// forward declaration
class Splot_GameElement;

class Splot_GameElementBase
{
 friend class Splot_GameElementBase;

 public:
  virtual ~Splot_GameElementBase ();

  virtual Splot_GameElement* get_next () { return next_; };
  void set_next (Splot_GameElement*);

 protected:
  Splot_GameElementBase ();

 private:
  Splot_GameElement* prev_;
  Splot_GameElement* next_;
};

class Splot_GameElement
 : public Splot_GameElementBase
{
 public:
  virtual ~Splot_GameElement ();

  virtual void reset ()
  {
    ACE_OS::memset (&position_, 0, sizeof (position_));
    ACE_OS::memset (&velocity_, 0, sizeof (velocity_));
    age_ = 0;
  }
  GameElementType_t type () { return type_; }

  float position_[3];
  float velocity_[3];
  int   age_;

  static unsigned int count;

 protected:
  Splot_GameElement ();

  GameElementType_t type_;

 private:
  typedef Splot_GameElementBase inherited;
};

#endif // GAMEELEMENT_H
