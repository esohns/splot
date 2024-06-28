#ifndef AUDIO_H
#define AUDIO_H

#ifdef HAVE_CONFIG_H
#include "splot_config.h"
#endif

#include "ace/OS.h"

#ifdef WITH_SDL_CDROM
#include "SDL.h" // for CDROM
#endif

#include "defines.h"

enum MusicFormat_t
{
  MUSICFORMAT_INVALID = -1,
  MUSICFORMAT_MID = 0,
  MUSICFORMAT_WAV,
  ///////////////////////////////////////
  MAX_MUSICFORMAT_TYPES
};

enum MusicMode_t
{
  MUSIC_INVALID = -1,
  MUSIC_GAME = 0,
  MUSIC_MENU,
  ///////////////////////////////////////
  MAX_MUSIC_MODES
};

enum SoundType_t
{
  SOUND_INVALID = -1,
  SOUND_EXPLOSION_DEFAULT = 0,
  SOUND_EXPLOSION_HEAVY,
  SOUND_EXPLOSION_LIGHT,
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

//  virtual void update ();

  virtual void play (SoundType_t,        // type
                     const float (&)[3], // position
                     int = 0) {};        // age
  virtual void setSoundVolume (float) {}; // volume [0.0, 1.0]

  virtual void playMusic (MusicMode_t); // mode
  virtual void stopMusic ();
  virtual void pauseMusic (bool); // pause ? : resume
  virtual void setMusicMode (MusicMode_t); // mode
  virtual void setMusicVolume (float); // volume [0.0, 1.0]
  virtual void setMusicIndex (int); // track
  virtual void nextMusicIndex ();

 protected:
  virtual void init () {};
#ifdef WITH_SDL_CDROM
  virtual void initCDROM ();
#endif // WITH_SDL_CDROM

  char    soundFilenames_[MAX_SOUND_TYPES][PATH_MAX];
  char    musicFilenames_[MAX_MUSICFORMAT_TYPES][MAX_MUSIC_MODES][PATH_MAX];
  char    musicPlaylistFilenames_[MAX_MUSIC][PATH_MAX];
  int     musicMaxIndex_;
  int     musicCurrentIndex_;
#ifdef WITH_SDL_CDROM
  SDL_CD* CDROM_;
#endif // USE_SDL
};

#endif // AUDIO_H
