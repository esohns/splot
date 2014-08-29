#include "stdafx.h"

#include "audio_SDLmixer.h"

//#ifdef HAVE_CONFIG_H
//#include "splot-config.h"
//#endif

#ifdef USE_SDLMIXER_AUDIO

//#include "gettext.h"
//
//#include "Splot_AudioSDLMixer.h"
//
//#include <cstdio>
//#include <cstdlib>
//#include <cstdarg>
//#include <cstring>
//
//#ifndef _WIN32
//#include <unistd.h>
//#include <cerrno>
//#include <fcntl.h>
//#include <sys/stat.h>
//#endif // _WIN32
//
#include "common.h"
//#include "define.h"
#include "configuration.h"
//#include "Global.h"

Splot_AudioSDLMixer::Splot_AudioSDLMixer ()
 : inherited ()
 //, sounds_ ()
{
  for (int i = 0; i < MAX_SOUND_TYPES; i++)
    sounds_[i] = NULL;

  const Configuration_t& configuration =
    SPLOT_CONFIGURATION_SINGLETON::instance ()->get ();
  //UNCLEAN - if initSound fails, config->audioEnabled() will be set to false
  if (configuration.audio_enabled)
    init ();
}

Splot_AudioSDLMixer::~Splot_AudioSDLMixer ()
{
  for (int i = 0; i < MAX_SOUND_TYPES; i++)
    Mix_FreeChunk (sounds_[i]);

  Mix_CloseAudio ();
}

void
Splot_AudioSDLMixer::init ()
{
  if (Mix_OpenAudio (AUDIO_FREQUENCY,
                     AUDIO_SDL_FORMAT,
                     AUDIO_CHANNELS,
                     AUDIO_CHUNKSIZE) < 0)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to Mix_OpenAudio(): \"%s\", returning\n"),
                ACE_TEXT (Mix_GetError ())));
    
    SPLOT_CONFIGURATION_SINGLETON::instance ()->setAudio (false);

    return;
  } // end IF

  for (int i = 0; i < MAX_SOUND_TYPES; i++)
  {
    sounds_[i] = Mix_LoadWAV (dataLoc (inherited::soundFilenames_[i]).c_str ());
    if (!sounds_[i])
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to Mix_LoadWAV(\"%s\"): \"%s\", continuing\n"),
                  ACE_TEXT (dataLoc (inherited::soundFilenames_[i]).c_str ()),
                  ACE_TEXT (Mix_GetError ())));
  } // end FOR

  if (Mix_ReserveChannels (1) != 1) // channel 0 is for music
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to Mix_ReserveChannels(1): \"%s\", continuing\n"),
                ACE_TEXT (Mix_GetError ())));

  const Configuration_t& configuration =
    SPLOT_CONFIGURATION_SINGLETON::instance ()->get ();
  setSoundVolume (configuration.vol_sound);
  setMusicVolume (configuration.vol_music);
}

void
Splot_AudioSDLMixer::play (SoundType_t type_in,
                           const float (&position_in)[3],
                           int age_in)
{
  const Configuration_t& configuration =
    SPLOT_CONFIGURATION_SINGLETON::instance ()->get ();
  if (configuration.audio_enabled)
    if (Mix_PlayChannel (-1, sounds_[type_in], 0) < 0)
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to Mix_PlayChannel(%d): \"%s\", continuing\n"),
                  type_in,
                  ACE_TEXT (Mix_GetError ())));
}

void
Splot_AudioSDLMixer::pauseMusic (bool pause_in)
{
  const Configuration_t& configuration =
    SPLOT_CONFIGURATION_SINGLETON::instance ()->get ();
  if (!configuration.audio_enabled)
    return; // nothing to do

  if (configuration.use_cdrom)
    return inherited::pauseMusic (pause_in);

  if (pause_in)
    Mix_PauseMusic ();
  else
    Mix_ResumeMusic ();
}

void
Splot_AudioSDLMixer::stopMusic ()
{
  const Configuration_t& configuration =
    SPLOT_CONFIGURATION_SINGLETON::instance ()->get ();
  if (!configuration.audio_enabled)
    return; // nothing to do

  if (configuration.use_cdrom)
    return inherited::stopMusic ();

  if (Mix_HaltChannel (0) < 0)
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to Mix_HaltChannel(0): \"%s\", continuing\n"),
                ACE_TEXT (Mix_GetError ())));
}

void
Splot_AudioSDLMixer::setMusicMode (SoundType_t mode_in)
{
  const Configuration_t& configuration =
    SPLOT_CONFIGURATION_SINGLETON::instance ()->get ();
  if (!configuration.audio_enabled)
    return; // nothing to do

  inherited::setMusicMode (mode_in);
  switch (mode_in)
  {
    default:
    case SOUND_MUSIC_GAME:
      if (configuration.use_cdrom)
      {
        if (Mix_HaltChannel (0) < 0)
          ACE_DEBUG ((LM_ERROR,
                      ACE_TEXT ("failed to Mix_HaltChannel(0): \"%s\", continuing\n"),
                      ACE_TEXT (Mix_GetError ())));
      } // end IF
      else
        if (Mix_PlayChannel (0, sounds_[mode_in], -1) < 0)
          ACE_DEBUG ((LM_ERROR,
                      ACE_TEXT ("failed to Mix_PlayChannel(%d): \"%s\", continuing\n"),
                      mode_in,
                      ACE_TEXT (Mix_GetError ())));

      break;
    case SOUND_MUSIC_MENU:
      if (Mix_PlayChannel (0, sounds_[mode_in], -1) < 0)
        ACE_DEBUG ((LM_ERROR,
                    ACE_TEXT ("failed to Mix_PlayChannel(%d): \"%s\", continuing\n"),
                    mode_in,
                    ACE_TEXT (Mix_GetError ())));
      break;
  } // end SWITCH
}

void
Splot_AudioSDLMixer::setMusicVolume (float value_in)
{
  inherited::setMusicVolume (value_in);

  const Configuration_t& configuration =
    SPLOT_CONFIGURATION_SINGLETON::instance ()->get ();
  if (!configuration.audio_enabled)
    return; // nothing to do

  ACE_ASSERT (value_in >= 0.0 &&
              value_in <= 1.0);
  if (Mix_Volume (0, (int)(MIX_MAX_VOLUME*value_in) ) < 0)
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to Mix_Volume(%f): \"%s\", continuing\n"),
                value_in,
                ACE_TEXT (Mix_GetError ())));

}

void
Splot_AudioSDLMixer::setSoundVolume (float value_in)
{
  const Configuration_t& configuration =
    SPLOT_CONFIGURATION_SINGLETON::instance ()->get ();
  if (!configuration.audio_enabled)
    return; // nothing to do

  ACE_ASSERT (value_in >= 0.0 &&
              value_in <= 1.0);
  for (int i = 1; i < MIX_CHANNELS; i++)
    if (Mix_Volume (i, (int)(MIX_MAX_VOLUME*value_in)) < 0)
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to Mix_Volume(%f): \"%s\", continuing\n"),
                  value_in,
                  ACE_TEXT (Mix_GetError ())));
}
#endif // USE_SDLMIXER_AUDIO
