#ifndef AUDIOSDLMIXER_H
#define AUDIOSDLMIXER_H

#ifdef HAVE_CONFIG_H
#include "splot_config.h"
#endif

#ifdef USE_SDLMIXER_AUDIO
#include "SDL_mixer.h"

#include "audio.h"

class Splot_AudioSDLMixer
 : public Splot_Audio
{
 public:
  Splot_AudioSDLMixer ();
  virtual ~Splot_AudioSDLMixer ();

  virtual void play (SoundType_t,        // type
                     const float (&)[3], // position
                     int = 0);           // age
  virtual void setSoundVolume (float); // volume [0.0, 1.0]

  virtual void playMusic (MusicMode_t); // mode
  virtual void stopMusic ();
  virtual void pauseMusic (bool); // pause ? : resume
  virtual void setMusicMode (MusicMode_t); // mode
  virtual void setMusicVolume (float); // volume [0.0, 1.0]

 protected:
  virtual void init ();

 private:
  typedef Splot_Audio inherited;

  Mix_Chunk* sounds_[MAX_SOUND_TYPES];
  Mix_Music* music_[MAX_MUSICFORMAT_TYPES][MAX_MUSIC_MODES];
};

//#ifdef __cplusplus
//}
//#endif
#endif // USE_SDLMIXER_AUDIO

#endif // AUDIOSDLMIXER_H
