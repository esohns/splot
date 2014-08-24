#ifndef AUDIO_H
#define AUDIO_H

//#ifdef HAVE_CONFIG_H
//#include <chromium-bsu-config.h>
//#endif

#include "ace/OS.h"

#ifdef USE_SDL
#include "SDL.h" // for CDROM
#endif

#include "defines.h"

enum SoundType_t
{
  SOUND_INVALID = -1,
  SOUND_EXPLOSION_DEFAULT = 0,
  SOUND_EXPLOSION_HEAVY,
  SOUND_EXPLOSION_LIGHT,
  SOUND_MUSIC_GAME,
  SOUND_MUSIC_MENU,
  SOUND_PLAYER_AMMUNITION_0,
  SOUND_POWERUP,
  SOUND_SHIP_ADD,
  SOUND_SHIP_LOSE,
  ///////////////////////////////////////
  MAX_SOUND_TYPES
};

struct Sound_t
{
  Sound_t ()
   : type (SOUND_EXPLOSION_DEFAULT)
   //, position (position_in)
   , age (1)
  {
    position[0] = 0.0; position[1] = 0.0; position[2] = 0.0;
  }
  Sound_t (SoundType_t type_in, float position_in[3], int age_in)
   : type (type_in)
   //, position (position_in)
   , age (age_in)
  {
    position[0] = position_in[0]; position[1] = position_in[1]; position[2] = position_in[2];
  }

  SoundType_t type;
  float       position[3];
  int         age;
};

class Splot_Audio
{
 public:
  Splot_Audio ();
  virtual ~Splot_Audio ();

  virtual void update () {};
  virtual void play (SoundType_t, float p[3], int = 0) {};
  virtual void stopMusic ();
  virtual void pauseMusic (bool); // pause ? : resume
  virtual void setMusicMode (SoundType_t);
  virtual void setMusicVolume (float);
  virtual void setSoundVolume (float) {};
  virtual void setMusicIndex (int);
  virtual void nextMusicIndex ();

 protected:
  virtual void init () {};
#ifdef USE_SDL
  virtual void initCDROM ();
#endif // USE_SDL

  char    soundFilenames_[MAX_SOUND_TYPES][PATH_MAX];
  char    musicPlaylistFilenames_[MAX_MUSIC][PATH_MAX];
  int     musicMaxIndex_;
  int     musicCurrentIndex_;
#ifdef USE_SDL
  SDL_CD* CDROM_;
#endif // USE_SDL
};

#endif // AUDIO_H
