#ifndef AUDIOSDLMIXER_H
#define AUDIOSDLMIXER_H

#ifdef HAVE_CONFIG_H
#include "splot-config.h"
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
  virtual void stopMusic ();
  virtual void pauseMusic (bool); // pause ? : resume
  virtual void setMusicMode (SoundType_t);
  virtual void setMusicVolume (float);
  virtual void setSoundVolume (float);

 protected:
  virtual void init ();

 private:
  typedef Splot_Audio inherited;

  Mix_Chunk* sounds_[MAX_SOUND_TYPES];
};

//#ifdef __cplusplus
//}
//#endif
#endif // USE_SDLMIXER_AUDIO

#endif // AUDIOSDLMIXER_H
