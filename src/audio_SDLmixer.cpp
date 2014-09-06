#include "stdafx.h"

#include "audio_SDLmixer.h"

#ifdef USE_SDLMIXER_AUDIO
#include "common.h"
#include "configuration.h"

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
  Uint32 SDL_init_flags = SDL_WasInit (0);
  if (!(SDL_init_flags & SDL_INIT_AUDIO))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("SDL audio not initialized, returning\n")));

    SPLOT_CONFIGURATION_SINGLETON::instance ()->setAudio (false);

    return;
  } // end IF

  if (Mix_OpenAudio (AUDIO_FREQUENCY,
                     AUDIO_SDL_FORMAT,
                     AUDIO_CHANNELS,
                     AUDIO_CHUNKSIZE))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to Mix_OpenAudio(): \"%s\", returning\n"),
                ACE_TEXT (Mix_GetError ())));
    
    SPLOT_CONFIGURATION_SINGLETON::instance ()->setAudio (false);

    return;
  } // end IF

  int frequency = 0;
  Uint16 format = 0;
  int channels = 0;
  std::string format_string;
  if (Mix_QuerySpec (&frequency,
                     &format,
                     &channels) == 0)
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to Mix_QuerySpec(): \"%s\", continuing\n"),
                ACE_TEXT (Mix_GetError ())));
  switch (format)
  {
    case AUDIO_U8:
      format_string = ACE_TEXT_ALWAYS_CHAR ("AUDIO_U8"); break;
    case AUDIO_S8:
      format_string = ACE_TEXT_ALWAYS_CHAR ("AUDIO_S8"); break;
    case AUDIO_U16LSB:
      format_string = ACE_TEXT_ALWAYS_CHAR ("AUDIO_U16LSB"); break;
    case AUDIO_S16LSB:
      format_string = ACE_TEXT_ALWAYS_CHAR ("AUDIO_S16LSB"); break;
    case AUDIO_U16MSB:
      format_string = ACE_TEXT_ALWAYS_CHAR ("AUDIO_U16MSB"); break;
    case AUDIO_S16MSB:
      format_string = ACE_TEXT_ALWAYS_CHAR ("AUDIO_S16MSB"); break;
    default:
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("invalid/unknown audio format (was: %u), continuing\n"),
                  format));

      break;
    }
  } // end SWITCH
  char driver[PATH_MAX];
  ACE_OS::memset (driver, 0, sizeof (driver));
  if (!SDL_AudioDriverName (driver,
                            sizeof (driver)))
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to SDL_AudioDriverName(): \"%s\", continuing\n"),
                ACE_TEXT (SDL_GetError ())));

  const Configuration_t& configuration =
    SPLOT_CONFIGURATION_SINGLETON::instance ()->get ();
  if (configuration.debug)
  {
#if defined (_MSC_VER)
    ACE_DEBUG ((LM_INFO,
                ACE_TEXT ("*** audio capabilities (driver: \"%s\") ***\nfrequency:\t%d\nformat:\t\t%s\nchannels:\t%s\nCD:\t\t%s\n"),
                ACE_TEXT (driver),
                frequency,
                ACE_TEXT (format_string.c_str ()),
                ((channels == 2) ? ACE_TEXT ("stereo") : ACE_TEXT ("mono")),
                (configuration.use_cdrom ? ACE_TEXT (SDL_CDName (configuration.cdrom_device)) : ACE_TEXT ("N/A"))));
#else
    ACE_DEBUG ((LM_INFO,
                ACE_TEXT ("*** audio capabilities (driver: \"%s\") ***\nfrequency:\t%d\nformat:\t%s\nchannels:\t%d\nCD:\t%s\n"),
                ACE_TEXT (driver),
                frequency,
                ACE_TEXT (format_string.c_str ()),
                ((channels == 2) ? ACE_TEXT ("stereo") : ACE_TEXT ("mono")),
                (configuration.use_cdrom ? ACE_TEXT (SDL_CDName (configuration.cdrom_device)) : ACE_TEXT ("N/A"))));
#endif
  } // end IF

//  int total = Mix_GetNumChunkDecoders ();
//  ACE_DEBUG ((LM_DEBUG,
//              ACE_TEXT ("*** audio decoders (effects) ***\n")));
//  for (int i = 0; i < total; i++)
//    ACE_DEBUG ((LM_DEBUG,
//                ACE_TEXT ("chunk decoder: [%s]\n"),
//                ACE_TEXT (Mix_GetChunkDecoder (i))));
//  total = Mix_GetNumMusicDecoders ();
//  ACE_DEBUG ((LM_DEBUG,
//              ACE_TEXT ("*** audio decoders (music) ***\n")));
//  for (int i = 0; i < total; i++)
//    ACE_DEBUG ((LM_DEBUG,
//                ACE_TEXT ("music decoder: [%s]\n"),
//                ACE_TEXT (Mix_GetMusicDecoder (i))));

  std::string filename;
  SDL_RWops* rw_ops = NULL;
  for (int i = 0; i < MAX_SOUND_TYPES; i++)
  {
    filename = dataLoc (inherited::soundFilenames_[i]);
    rw_ops = SDL_RWFromFile (filename.c_str (), ACE_TEXT_ALWAYS_CHAR ("rb"));
    if (!rw_ops)
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to SDL_RWFromFile(\"%s\"): \"%s\", continuing\n"),
                  ACE_TEXT (filename.c_str ()),
                  ACE_TEXT (SDL_GetError ())));

      continue;
    } // end IF

    sounds_[i] = Mix_LoadWAV_RW (rw_ops, 1);
    if (!sounds_[i])
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to Mix_LoadWAV_RW(\"%s\"): \"%s\", continuing\n"),
                  ACE_TEXT (filename.c_str ()),
                  ACE_TEXT (Mix_GetError ())));

    //SDL_FreeRW (rw_ops);
  } // end FOR

  int def_num_channels = Mix_AllocateChannels (-1);
  if (Mix_AllocateChannels (MAX_MIXING_CHANNELS) != MAX_MIXING_CHANNELS)
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to Mix_AllocateChannels(%d): \"%s\", continuing\n"),
                MAX_MIXING_CHANNELS,
                ACE_TEXT (Mix_GetError ())));
  else
    if (configuration.debug)
      ACE_DEBUG ((LM_INFO,
                  ACE_TEXT ("available mixing channels: %d (default: %d)\n"),
                  MAX_MIXING_CHANNELS, def_num_channels));

  if (Mix_ReserveChannels (1) != 1) // channel 0 is for music
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to Mix_ReserveChannels(1): \"%s\", continuing\n"),
                ACE_TEXT (Mix_GetError ())));

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
  {
    int num_channel = Mix_PlayChannel (-1, sounds_[type_in], 0);
    if (num_channel < 0)
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to Mix_PlayChannel(%d): \"%s\", continuing\n"),
                  type_in,
                  ACE_TEXT (Mix_GetError ())));
    //else
    //  if (configuration.debug)
    //    ACE_DEBUG ((LM_INFO,
    //                ACE_TEXT ("playing sound %d on channel %d...\n"),
    //                type_in,
    //                num_channel));
  } // end IF
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
