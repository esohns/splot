#include "stdafx.h"

#include "audio.h"

//#ifdef HAVE_CONFIG_H
//#include <chromium-bsu-config.h>
//#endif
//
//#include "gettext.h"
//
//#include "Splot_Audio.h"
//
//
#include <string>
//#include <cstdlib>
#ifdef USE_SDL
#if !defined (ACE_WIN32) && !defined (ACE_WIN64)
#include <cmath>
#include <sstream>
#endif
#endif

#include "ace/Default_Constants.h"
#include "ace/OS.h"
#include "ace/Log_Msg.h"

#include "configuration.h"
#include "state.h"

//#if !defined (USE_SDL_CDROM) && defined (SDL_CD_STUBS)
//void	SDL_CDPlayTracks(void*, int, int, int, int) {}
//void	SDL_CDPause(void*)	{}
//void	SDL_CDResume(void*)	{}
//void	SDL_CDStop(void*)	{}
//int		SDL_CDStatus(void*)	{ return 0; }
//#endif //USE_SDL_CDROM

Splot_Audio::Splot_Audio ()
  : musicMaxIndex_ (1)
  , musicCurrentIndex_ (0)
#ifdef USE_SDL
  , CDROM_ (NULL)
#endif // USE_SDL
{
  std::string path_base = ACE_TEXT_ALWAYS_CHAR (SPLOT_AUDIO_DATA_DIR);
  path_base += ACE_DIRECTORY_SEPARATOR_STR;
  std::string filename = path_base + ACE_TEXT_ALWAYS_CHAR ("boom.wav");
  ACE_OS::strcpy (soundFilenames_[SOUND_PLAYER_AMMUNITION_0], filename.c_str ());
  filename = path_base + ACE_TEXT_ALWAYS_CHAR ("power.wav");
  ACE_OS::strcpy (soundFilenames_[SOUND_POWERUP], filename.c_str ());
  filename = path_base + ACE_TEXT_ALWAYS_CHAR ("exploStd.wav");
  ACE_OS::strcpy (soundFilenames_[SOUND_EXPLOSION_DEFAULT], filename.c_str ());
  filename = path_base + ACE_TEXT_ALWAYS_CHAR ("exploPop.wav");
  ACE_OS::strcpy (soundFilenames_[SOUND_EXPLOSION_LIGHT], filename.c_str ());
  filename = path_base + ACE_TEXT_ALWAYS_CHAR ("exploBig.wav");
  ACE_OS::strcpy (soundFilenames_[SOUND_EXPLOSION_HEAVY], filename.c_str ());
  filename = path_base + ACE_TEXT_ALWAYS_CHAR ("life_lose.wav");
  ACE_OS::strcpy (soundFilenames_[SOUND_SHIP_LOSE], filename.c_str ());
  filename = path_base + ACE_TEXT_ALWAYS_CHAR ("life_add.wav");
  ACE_OS::strcpy (soundFilenames_[SOUND_SHIP_ADD], filename.c_str ());
  filename = path_base + ACE_TEXT_ALWAYS_CHAR ("music_game.wav");
  ACE_OS::strcpy (soundFilenames_[SOUND_MUSIC_GAME], filename.c_str ());
  filename = path_base + ACE_TEXT_ALWAYS_CHAR ("music_menu.wav");
  ACE_OS::strcpy (soundFilenames_[SOUND_MUSIC_MENU], filename.c_str ());

  const Configuration_t& configuration =
    SPLOT_CONFIGURATION_SINGLETON::instance ()->get ();
  if (configuration.audio_enabled)
  {
#ifdef USE_SDL
    initCDROM ();
#endif // USE_SDL
  } // end IF
}

Splot_Audio::~Splot_Audio()
{
#ifdef USE_SDL
  if (CDROM_)
  {
    if (SDL_CDStop (CDROM_) == -1)
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to SDL_CDStop (): \"%s\", continuing\n"),
                  ACE_TEXT (SDL_GetError ())));

    SDL_CDClose (CDROM_);
  } // end IF
#endif // USE_SDL
}
//
//void
//Splot_Audio::update ()
//{
//
//}
//
//void
//Splot_Audio::update ()
//{
//
//}
//
//void
//Splot_Audio::init ()
//{
//
//}

/**
 * If SDL is being used as the windowing toolkit, check
 * availability of CDROM for music tracks. Does nothing
 * if GLUT is being used.
 */
#ifdef USE_SDL
static
const char*
trackType (int track_in)
{
  static char buffer[BUFSIZ];
  switch (track_in)
  {
    case SDL_AUDIO_TRACK: return ACE_TEXT_ALWAYS_CHAR ("audio");
    case SDL_DATA_TRACK:  return ACE_TEXT_ALWAYS_CHAR ("data");
    default:
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("invalid track type (was: %d), continuing\n"),
                  track_in));

      ACE_OS::sprintf (buffer, ACE_TEXT_ALWAYS_CHAR ("UNKNOWN(type=%d)"), track_in);
      break;
  } // end SWITCH

  return buffer;
}
void
Splot_Audio::initCDROM ()
{
  const Configuration_t& configuration =
    SPLOT_CONFIGURATION_SINGLETON::instance ()->get ();
  if (!configuration.use_cdrom)
    return;

  int num_cd_drives = SDL_CDNumDrives ();
  if (num_cd_drives == -1)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to SDL_CDNumDrives (): \"%s\", returning\n"),
                ACE_TEXT (SDL_GetError ())));

    return;
  } // end IF
  if (configuration.debug)
    ACE_DEBUG ((LM_INFO,
                ACE_TEXT ("%d CDROM drive(s)\n"),
                num_cd_drives));

  State_t& state = SPLOT_STATE_SINGLETON::instance ()->get ();
  state.cdrom_count = num_cd_drives;
  if (state.cdrom_count == 0)
  {
    ACE_DEBUG ((LM_DEBUG,
                ACE_TEXT ("no CDROM drive(s): nothing to do, returning\n")));

    return;
  } // end IF

  if (CDROM_)
  {
    SDL_CDClose (CDROM_);
    CDROM_ = NULL;
  } // end IF
  CDROM_ = SDL_CDOpen (configuration.cdrom_device);
  if (!CDROM_)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to SDL_CDOpen (%d): \"%s\", returning\n"),
                configuration.cdrom_device,
                ACE_TEXT (SDL_GetError ())));

    return;
  } // end IF

  CDstatus cdrom_status = SDL_CDStatus (CDROM_);
  if (cdrom_status == CD_ERROR)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to SDL_CDStatus (%@): \"%s\", returning\n"),
                CDROM_,
                ACE_TEXT (SDL_GetError ())));

    return;
  } // end IF
  else if (!CD_INDRIVE (cdrom_status))
  {
    ACE_DEBUG ((LM_DEBUG,
                ACE_TEXT ("%d: no CD in drive, returning\n"),
                configuration.cdrom_device));

    return;
  } // end IF

  // *NOTE*: this code is a mess...
  musicMaxIndex_ = CDROM_->numtracks;
  if (configuration.debug)
    ACE_DEBUG ((LM_INFO,
                ACE_TEXT ("%d: #track(s): %d\n"),
                configuration.cdrom_device,
                CDROM_->numtracks));

  // sanity check: any audio tracks ?
  int num_audio_tracks = 0;
  for (int i = 0; i < CDROM_->numtracks; i++)
  {
    if (configuration.debug)
      ACE_DEBUG ((LM_INFO,
                  ACE_TEXT ("%d: track #%2d type: \"%s\"\n"),
                  configuration.cdrom_device,
                  i,
                  trackType (CDROM_->track[i].type)));

    // SDL BUG? 'type' should only be SDL_DATA_TRACK(0x04)or SDL_Splot_Audio_TRACK(0x00), but 
    // this is returning SDL_AUDIO_TRACK=0x02 and SDL_DATA_TRACK=0x06 on some copied CDRs!
    // Orignal CDs return correct values, but CDR duplications are off by 2!
    if (CDROM_->track[i].type < SDL_DATA_TRACK)
      num_audio_tracks++;
  } // end FOR
  if (!num_audio_tracks)
  {
    ACE_DEBUG ((LM_DEBUG,
                ACE_TEXT ("%d: data tracks only, CD audio not available, returning\n"),
                configuration.cdrom_device));

    SDL_CDClose (CDROM_);
    CDROM_ = NULL;
  } // end IF

  if (configuration.debug)
    ACE_DEBUG ((LM_INFO,
                ACE_TEXT ("use_cdrom enabled. Set to '0' in the configuration file to disable.\n")
                ACE_TEXT ("Press the \'N\' key to skip to next CD track during a game.\n")));
}
#endif // USE_SDL

void
Splot_Audio::stopMusic ()
{
#ifdef USE_SDL
  if (!CDROM_)
    return; // nothing to do
  if (SDL_CDStop (CDROM_) == -1)
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to SDL_CDStop (%@): \"%s\", continuing\n"),
                CDROM_,
                ACE_TEXT (SDL_GetError ())));
#endif // USE_SDL
}

void
Splot_Audio::pauseMusic (bool status_in)
{
#ifdef USE_SDL
  if (!CDROM_)
    return; // nothing to do

  CDstatus cdrom_status = SDL_CDStatus (CDROM_);
  if (cdrom_status == CD_ERROR)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to SDL_CDStatus (%@): \"%s\", returning\n"),
                CDROM_,
                ACE_TEXT (SDL_GetError ())));

    return;
  } // end IF

  if (status_in && cdrom_status == CD_PLAYING)
  {
    if (SDL_CDPause (CDROM_) == -1)
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to SDL_CDPause (%@): \"%s\", continuing\n"),
                  CDROM_,
                  ACE_TEXT (SDL_GetError ())));
  } // end IF
  else if (!status_in && cdrom_status == CD_PAUSED)
  {
    if (SDL_CDResume (CDROM_) == -1)
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to SDL_CDResume (%@): \"%s\", continuing\n"),
                  CDROM_,
                  ACE_TEXT (SDL_GetError ())));
  } // end ELSE
  //else
  //{
  //  const Configuration_t& configuration =
  //    SPLOT_CONFIGURATION_SINGLETON::instance ()->get ();
  //  ACE_DEBUG ((LM_ERROR,
  //              ACE_TEXT ("%d: invalid CDROM state (drive: %d, status was: %d), continuing\n"),
  //              (int)status_in,
  //              configuration.cdrom_device,
  //              cdrom_status));
  //} // end ELSE
#endif // USE_SDL
}

void
Splot_Audio::setMusicMode (SoundType_t soundType_in)
{
  const Configuration_t& configuration =
    SPLOT_CONFIGURATION_SINGLETON::instance ()->get ();
  if (!configuration.audio_enabled)
    return; // nothing to do

#ifdef USE_SDL
  if (!CDROM_)
    return; // nothing to do

  CDstatus cdrom_status = SDL_CDStatus (CDROM_);
  State_t state = SPLOT_STATE_SINGLETON::instance ()->get ();
  switch (soundType_in)
  {
    case SOUND_MUSIC_GAME:
      switch (cdrom_status)
      {
        case CD_PAUSED:
          // resume playback ?
          if (!state.game_pause)
            if (SDL_CDResume (CDROM_) == -1)
              ACE_DEBUG ((LM_ERROR,
                          ACE_TEXT ("failed to SDL_CDResume (%@): \"%s\", continuing\n"),
                          CDROM_,
                          ACE_TEXT (SDL_GetError ())));
          break;
        case CD_STOPPED:
          // start (paused) playback ?
          if (SDL_CDPlayTracks (CDROM_, musicCurrentIndex_, 0, 1, 0) == -1)
            ACE_DEBUG ((LM_ERROR,
                        ACE_TEXT ("failed to SDL_CDPlayTracks (%d): \"%s\", continuing\n"),
                        musicCurrentIndex_,
                        ACE_TEXT (SDL_GetError ())));
          if (state.game_pause)
            if (SDL_CDPause (CDROM_) == -1)
              ACE_DEBUG ((LM_ERROR,
                          ACE_TEXT ("failed to SDL_CDPause (%@): \"%s\", continuing\n"),
                          CDROM_,
                          ACE_TEXT (SDL_GetError ())));
          break;
        default:
          break; // nothing to do
      } // end SWITCH
      break;
    case SOUND_MUSIC_MENU:
      if (cdrom_status == CD_PLAYING)
        if (SDL_CDPause (CDROM_) == -1)
          ACE_DEBUG ((LM_ERROR,
                      ACE_TEXT ("failed to SDL_CDPause (%@): \"%s\", continuing\n"),
                      CDROM_,
                      ACE_TEXT (SDL_GetError ())));
      break;
    default:
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("invalid/unknown music mode (was: %d), returning\n"),
                  soundType_in));
      return;
  } // end SWITCH
#endif // USE_SDL
}

void
Splot_Audio::setMusicVolume (float value_in)
{
  const Configuration_t& configuration =
    SPLOT_CONFIGURATION_SINGLETON::instance ()->get ();
  if (!configuration.audio_enabled)
    return; // nothing to do

#ifdef USE_SDL
  if (!configuration.use_cdrom)
    return; // nothing to do

#if !defined (ACE_WIN32) && !defined (ACE_WIN64)
  // determine desired volume
  const int max_cdrom_volume = 100; // 100 percent
  int desiredVolume = floor (max_cdrom_volume*value_in);
  std::stringstream volume;
  volume << desiredVolume;

  // set the desired volume
  string command = ACE_TEXT_ALWAYS_CHAR ("amixer set CD ");
  command += volume.str ();
  command += "% unmute";
  int status = ACE_OS::system (command.c_str ());
  if (status == -1 || !(WIFEXITED (status) && WEXITSTATUS (status) == 0))
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to set CD volume, amixer returned status: %d, continuing\n"),
                status));
#endif
#endif // USE_SDL
}

void
Splot_Audio::setMusicIndex (int track_in)
{
  const Configuration_t& configuration =
    SPLOT_CONFIGURATION_SINGLETON::instance ()->get ();
  if (!configuration.audio_enabled)
    return; // nothing to do

  if (musicMaxIndex_)
    musicCurrentIndex_ = track_in%musicMaxIndex_;

#ifdef USE_SDL
  if (!CDROM_)
    return; // nothing to do

  CDstatus cdrom_status = SDL_CDStatus (CDROM_);
  if (cdrom_status == CD_ERROR)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to SDL_CDStatus (%@): \"%s\", returning\n"),
                CDROM_,
                ACE_TEXT (SDL_GetError ())));

    return;
  } // end IF

  //// if it's a data track, increment the index and try again
  //if (CDROM_->track[musicCurrentIndex_].type >= SDL_DATA_TRACK)
  //  return setMusicIndex (musicCurrentIndex_ + 1);

  //SDL_CDStop (CDROM_);
  if (SDL_CDPlayTracks (CDROM_, musicCurrentIndex_, 0, 1, 0) == -1)
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to SDL_CDPlayTracks (%d): \"%s\", continuing\n"),
                musicCurrentIndex_,
                ACE_TEXT (SDL_GetError ())));
  else if (configuration.debug)
    ACE_DEBUG ((LM_INFO,
                ACE_TEXT ("playing back CD track %d...\n"),
                musicCurrentIndex_));

  if (cdrom_status != CD_PLAYING)
    if (SDL_CDPause (CDROM_) == -1)
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to SDL_CDPause (%@): \"%s\", continuing\n"),
                  CDROM_,
                  ACE_TEXT (SDL_GetError ())));
#endif // USE_SDL
}

void
Splot_Audio::nextMusicIndex ()
{
  int i = musicCurrentIndex_ + 1;
  if (i >= musicMaxIndex_)
    i = 0;
  setMusicIndex (i);
}
