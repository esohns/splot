#include "stdafx.h"

#include "configuration.h"

#include <sstream>

#include "ace/OS.h"
#include "ace/Get_Opt.h"
#include "ace/Configuration.h"
#include "ace/Configuration_Import_Export.h"

#ifdef USE_SDLMIXER_AUDIO
#include "SDL_mixer.h"
#endif

#include "defines.h"
#include "common.h"

Splot_Configuration::Splot_Configuration ()
{
  configuration_.screen_width = CONFIGURATION_DEFAULT_SCREEN_WIDTH;
  configuration_.screen_height = CONFIGURATION_DEFAULT_SCREEN_HEIGHT;
  configuration_.screen_FOV = CONFIGURATION_DEFAULT_SCREEN_FOV;
  configuration_.screen_z_near = CONFIGURATION_DEFAULT_SCREEN_ZNEAR;
  configuration_.screen_z_far = CONFIGURATION_DEFAULT_SCREEN_ZFAR;
  configuration_.screen_bound[0] = CONFIGURATION_DEFAULT_SCREEN_BOUND_0;
  configuration_.screen_bound[1] = CONFIGURATION_DEFAULT_SCREEN_BOUND_1;
  configuration_.z_transformation = CONFIGURATION_DEFAULT_Z_TRANSFORMATION;

  configuration_.full_screen = CONFIGURATION_DEFAULT_FULLSCREEN;
  configuration_.blend_enabled = CONFIGURATION_DEFAULT_BLEND_ENABLED;
  configuration_.graphics_level = CONFIGURATION_DEFAULT_GRAPHICS_LEVEL;
  configuration_.true_color = CONFIGURATION_DEFAULT_TRUE_COLOR;
  configuration_.texture_border = CONFIGURATION_DEFAULT_TEXTURE_BORDER;
  configuration_.show_FPS = CONFIGURATION_DEFAULT_SHOW_FPS;
  configuration_.view_gamma = CONFIGURATION_DEFAULT_VIEW_GAMMA;
  configuration_.text_toolkit_type = CONFIGURATION_DEFAULT_TEXTTOOLKITTYPE;

  configuration_.auto_speed = CONFIGURATION_DEFAULT_AUTO_SPEED;
  configuration_.movement_speed = CONFIGURATION_DEFAULT_MOVEMENT_SPEED;

  configuration_.audio_enabled = CONFIGURATION_DEFAULT_AUDIO_ENABLED;
  configuration_.swap_stereo = CONFIGURATION_DEFAULT_SWAP_STEREO;
  configuration_.use_playlist = CONFIGURATION_DEFAULT_USE_PLAYLIST;
  configuration_.use_CD_audio = CONFIGURATION_DEFAULT_USE_CDAUDIO;
  configuration_.use_MIDI_music = CONFIGURATION_DEFAULT_USE_MIDIMUSIC;
  configuration_.CDROM_device = 0;
  configuration_.volume_sound = CONFIGURATION_DEFAULT_VOLUME_SOUND;
  configuration_.volume_music = CONFIGURATION_DEFAULT_VOLUME_MUSIC;
  configuration_.audio_toolkit_type = CONFIGURATION_DEFAULT_AUDIOTOOLKITTYPE;

  configuration_.debug = CONFIGURATION_DEFAULT_DEBUG;

  configuration_.skill_base = CONFIGURATION_DEFAULT_SKILL_BASE;
}

Splot_Configuration::~Splot_Configuration ()
{
  if (!save ())
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to Splot_Configuration::save (), aborting\n")));
}

bool
Splot_Configuration::init (int argc_in,
                           ACE_TCHAR** argv_in,
                           Mode_t& mode_out)
{
  // init return value(s)
  mode_out = MODE_GAME;

  ACE_Get_Opt argument_parser (argc_in,
                               argv_in,
                               ACE_TEXT (""));
  if (argument_parser.long_option (ACE_TEXT (OPTIONS_LONG_OPTION_NOAUDIO),
                                   'a',
                                   ACE_Get_Opt::NO_ARG) == -1)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to ACE_Get_Opt::long_option (\"%s\"): \"%m\", aborting\n"),
                ACE_TEXT (OPTIONS_LONG_OPTION_NOAUDIO)));

    return false;
  } // end IF
  if (argument_parser.long_option (ACE_TEXT (OPTIONS_LONG_OPTION_NOBLEND),
                                   'b',
                                   ACE_Get_Opt::NO_ARG) == -1)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to ACE_Get_Opt::long_option (\"%s\"): \"%m\", aborting\n"),
                ACE_TEXT (OPTIONS_LONG_OPTION_NOBLEND)));

    return false;
  } // end IF
  if (argument_parser.long_option (ACE_TEXT (OPTIONS_LONG_OPTION_CDMUSIC),
                                   'c',
                                   ACE_Get_Opt::NO_ARG) == -1)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to ACE_Get_Opt::long_option (\"%s\"): \"%m\", aborting\n"),
                ACE_TEXT (OPTIONS_LONG_OPTION_CDMUSIC)));

    return false;
  } // end IF
  if (argument_parser.long_option (ACE_TEXT (OPTIONS_LONG_OPTION_DEBUG),
                                   'd',
                                   ACE_Get_Opt::NO_ARG) == -1)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to ACE_Get_Opt::long_option (\"%s\"): \"%m\", aborting\n"),
                ACE_TEXT (OPTIONS_LONG_OPTION_DEBUG)));

    return false;
  } // end IF
  if (argument_parser.long_option (ACE_TEXT (OPTIONS_LONG_OPTION_INIFILE),
                                   'i',
                                   ACE_Get_Opt::ARG_REQUIRED) == -1)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to ACE_Get_Opt::long_option (\"%s\"): \"%m\", aborting\n"),
                ACE_TEXT (OPTIONS_LONG_OPTION_INIFILE)));

    return false;
  } // end IF
  if (argument_parser.long_option (ACE_TEXT (OPTIONS_LONG_OPTION_FULLSCREEN),
                                   'f',
                                   ACE_Get_Opt::NO_ARG) == -1)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to ACE_Get_Opt::long_option (\"%s\"): \"%m\", aborting\n"),
                ACE_TEXT (OPTIONS_LONG_OPTION_FULLSCREEN)));

    return false;
  } // end IF
  if (argument_parser.long_option( ACE_TEXT (OPTIONS_LONG_OPTION_VIDEOMODE),
                                   'm',
                                   ACE_Get_Opt::ARG_REQUIRED) == -1)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to ACE_Get_Opt::long_option (\"%s\"): \"%m\", aborting\n"),
                ACE_TEXT (OPTIONS_LONG_OPTION_VIDEOMODE)));

    return false;
  } // end IF
  if (argument_parser.long_option( ACE_TEXT (OPTIONS_LONG_OPTION_MIDIMUSIC),
                                   'M',
                                   ACE_Get_Opt::NO_ARG) == -1)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to ACE_Get_Opt::long_option (\"%s\"): \"%m\", aborting\n"),
                ACE_TEXT (OPTIONS_LONG_OPTION_MIDIMUSIC)));

    return false;
  } // end IF
  if (argument_parser.long_option (ACE_TEXT (OPTIONS_LONG_OPTION_SHOWFPS),
                                   's',
                                   ACE_Get_Opt::NO_ARG) == -1)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to ACE_Get_Opt::long_option (\"%s\"): \"%m\", aborting\n"),
                ACE_TEXT (OPTIONS_LONG_OPTION_SHOWFPS)));

    return false;
  } // end IF
  if (argument_parser.long_option (ACE_TEXT (OPTIONS_LONG_OPTION_NOTEXTUREBORDER),
                                   't',
                                   ACE_Get_Opt::NO_ARG) == -1)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to ACE_Get_Opt::long_option (\"%s\"): \"%m\", aborting\n"),
                ACE_TEXT (OPTIONS_LONG_OPTION_NOTEXTUREBORDER)));

    return false;
  } // end IF
  if (argument_parser.long_option (ACE_TEXT (OPTIONS_LONG_OPTION_VERSION),
                                   'v',
                                   ACE_Get_Opt::NO_ARG) == -1)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to ACE_Get_Opt::long_option (\"%s\"): \"%m\", aborting\n"),
                ACE_TEXT (OPTIONS_LONG_OPTION_VERSION)));

    return false;
  } // end IF
  if (argument_parser.long_option (ACE_TEXT (OPTIONS_LONG_OPTION_WINDOWED),
                                   'w',
                                   ACE_Get_Opt::NO_ARG) == -1)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to ACE_Get_Opt::long_option (\"%s\"): \"%m\", aborting\n"),
                ACE_TEXT (OPTIONS_LONG_OPTION_WINDOWED)));

    return false;
  } // end IF

  std::string ini_file = Splot_Configuration::getFileName ();
  ACE_stat stat;
  int result = ACE_OS::stat (ACE_TEXT (ini_file.c_str ()), &stat);
  if ((result == -1)                        ||
      ((stat.st_mode & S_IFMT) != S_IFREG)  || // regular file ?
      ((stat.st_mode & S_IRUSR) != S_IRUSR))   // readable ?
    ini_file.clear ();
  if (!ini_file.empty ())
    if (!load (ini_file))
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to Splot_Configuration::load (\"%s\"), aborting\n"),
                  ACE_TEXT (ini_file.c_str ())));

      return false;
    } // end IF
  ini_file.clear ();

  int option = 0;
  std::stringstream converter;
  while ((option = argument_parser ()) != EOF)
  {
    switch (option)
    {
      case 'a':
        configuration_.audio_enabled = false;
        break;
      case 'b':
        configuration_.blend_enabled = false;
        break;
      case 'c':
        configuration_.use_CD_audio = true;
        break;
      case 'd':
        configuration_.debug = true;
        break;
      case 'i':
        ini_file = argument_parser.opt_arg ();
        break;
      case 'f':
        configuration_.full_screen = true;
        break;
      case 'm':
      {
        int video_mode = -1;
        converter.clear ();
        converter.str (ACE_TEXT_ALWAYS_CHAR (""));
        converter << ACE_TEXT_ALWAYS_CHAR (argument_parser.opt_arg ());
        converter >> video_mode;
        if ((video_mode < 0) || (video_mode > MAX_SCREEN_SIZE))
         goto usage;
        setScreenSize (video_mode);

        break;
      }
      case 'M':
        configuration_.use_MIDI_music = true;
        break;
      case 's':
        configuration_.show_FPS = true;
        break;
      case 't':
        configuration_.texture_border = false;
        break;
      case 'v':
        mode_out = MODE_SHOW_VERSION;
        break;
      case 'w':
        configuration_.full_screen = false;
        break;
      // long options
      case 0:
      {
        const ACE_TCHAR* long_option_p = argument_parser.long_option ();
        if (long_option_p)
        {
          bool is_valid = true;
          std::string long_option = long_option_p;
          if (long_option == ACE_TEXT_ALWAYS_CHAR (OPTIONS_LONG_OPTION_INIFILE))
            ini_file = argument_parser.opt_arg ();
          if (long_option == ACE_TEXT_ALWAYS_CHAR (OPTIONS_LONG_OPTION_FULLSCREEN))
            configuration_.full_screen = true;
          else if (long_option == ACE_TEXT_ALWAYS_CHAR (OPTIONS_LONG_OPTION_WINDOWED))
            configuration_.full_screen = false;
          else if (long_option == ACE_TEXT_ALWAYS_CHAR(OPTIONS_LONG_OPTION_VIDEOMODE))
          {
            int video_mode = -1;
            converter.clear ();
            converter.str (ACE_TEXT_ALWAYS_CHAR (""));
            converter << ACE_TEXT_ALWAYS_CHAR (argument_parser.opt_arg ());
            converter >> video_mode;
            if ((video_mode < 0) || (video_mode > MAX_SCREEN_SIZE))
              goto usage;
            setScreenSize (video_mode);
          } // end IF
          else if (long_option == ACE_TEXT_ALWAYS_CHAR (OPTIONS_LONG_OPTION_NOAUDIO))
            configuration_.audio_enabled = false;
          else if (long_option == ACE_TEXT_ALWAYS_CHAR (OPTIONS_LONG_OPTION_NOBLEND))
            configuration_.blend_enabled = false;
          else if (long_option == ACE_TEXT_ALWAYS_CHAR (OPTIONS_LONG_OPTION_CDMUSIC))
            configuration_.use_CD_audio = true;
          else if (long_option == ACE_TEXT_ALWAYS_CHAR (OPTIONS_LONG_OPTION_MIDIMUSIC))
            configuration_.use_MIDI_music = true;
          else if (long_option == ACE_TEXT_ALWAYS_CHAR (OPTIONS_LONG_OPTION_SHOWFPS))
            configuration_.show_FPS = true;
          else if (long_option == ACE_TEXT_ALWAYS_CHAR (OPTIONS_LONG_OPTION_DEBUG))
            configuration_.debug = true;
          else if (long_option == ACE_TEXT_ALWAYS_CHAR (OPTIONS_LONG_OPTION_NOTEXTUREBORDER))
            configuration_.texture_border = false;
          else if (long_option == ACE_TEXT_ALWAYS_CHAR (OPTIONS_LONG_OPTION_VERSION))
            mode_out = MODE_SHOW_VERSION;
          else
            is_valid = false;

          if (is_valid)
            break;
        } // end IF

        // fall through
      }
      // non-options
      case 1:
      // error handling
      case '?':
      case ':':
      {
        if (option == ':')
          ACE_DEBUG ((LM_ERROR,
                      ACE_TEXT ("option \"%c\" requires an argument, aborting\n"),
                      ACE_TEXT (argument_parser.opt_opt ())));

        // fall through
      }
      default:
      {
        if ((option != ':'))
          ACE_DEBUG ((LM_ERROR,
                      ACE_TEXT ("unrecognized option \"%c\", aborting\n"),
                      option));

usage:
        ACE_DEBUG ((LM_ERROR,
                    ACE_TEXT ("\nusage: %s \n\n")
                    ACE_TEXT ("\t[-a/--%s] mute (defaults to false)\n")
                    ACE_TEXT ("\t[-b/--%s] disable blending (OpenGL) (defaults to false)\n")
                    ACE_TEXT ("\t[-c/--%s] CD music (defaults to false)\n")
                    ACE_TEXT ("\t[-d/--%s] debug (defaults to false)\n")
                    ACE_TEXT ("\t[-f/--%s] run in full-screen mode (defaults to true)\n")
                    ACE_TEXT ("\t[-i/--%s] configuration file\n")
                    ACE_TEXT ("\t[-m/--%s <mode>]\n\t\tmode\t0 = 320 x  200\n\t\t\t1 =  512 x  384\n\t\t\t2 =  640 x  400\n\t\t\t3 =  640 x  480\n\t\t\t4 =  800 x  600\n\t\t\t5 = 1024 x  768\n\t\t\t6 = 1280 x  800\n\t\t\t7 = 1280 x  960\n\t\t\t8 = 1280 x  1024\n")
                    ACE_TEXT ("\t[-M/--%s] MIDI music (defaults to false)\n")
                    ACE_TEXT ("\t[-s/--%s] show FPS (defaults to false)\n")
                    ACE_TEXT ("\t[-t/--%s] do not set tex border color (OpenGL) (defaults to false)\n")
                    ACE_TEXT ("\t[-v/--%s] show version information (defaults to false)\n")
                    ACE_TEXT ("\t[-w/--%s] run in windowed mode (defaults to true)\n")
                    ACE_TEXT ("\n"),
                    argv_in[0],
                    ACE_TEXT (OPTIONS_LONG_OPTION_NOAUDIO),
                    ACE_TEXT (OPTIONS_LONG_OPTION_NOBLEND),
                    ACE_TEXT (OPTIONS_LONG_OPTION_CDMUSIC),
                    ACE_TEXT (OPTIONS_LONG_OPTION_DEBUG),
                    ACE_TEXT (OPTIONS_LONG_OPTION_FULLSCREEN),
                    ACE_TEXT (OPTIONS_LONG_OPTION_INIFILE),
                    ACE_TEXT (OPTIONS_LONG_OPTION_VIDEOMODE),
                    ACE_TEXT (OPTIONS_LONG_OPTION_MIDIMUSIC),
                    ACE_TEXT (OPTIONS_LONG_OPTION_SHOWFPS),
                    ACE_TEXT (OPTIONS_LONG_OPTION_NOTEXTUREBORDER),
                    ACE_TEXT (OPTIONS_LONG_OPTION_VERSION),
                    ACE_TEXT (OPTIONS_LONG_OPTION_WINDOWED)));

        return false;
      }
    } // end SWITCH
  } // end WHILE

  if (!ini_file.empty ())
  {
    result = ACE_OS::stat (ACE_TEXT (ini_file.c_str ()), &stat);
    if ((result == -1)                        ||
        ((stat.st_mode & S_IFMT) != S_IFREG)  || // regular file ?
        ((stat.st_mode & S_IRUSR) != S_IRUSR))   // readable ?
      ini_file.clear ();
    if (!ini_file.empty ())
      if (!load (ini_file))
      {
        ACE_DEBUG ((LM_ERROR,
                    ACE_TEXT ("failed to Splot_Configuration::load (\"%s\"), aborting\n"),
                    ACE_TEXT (ini_file.c_str ())));

        return false;
      } // end IF
  } // end IF

  return true;
}

std::string
Splot_Configuration::getFileName ()
{
  char configuration_filename[PATH_MAX];
  ACE_OS::memset (&configuration_filename, 0, sizeof (configuration_filename));

  std::string home_directory = getHomeDirectory ();
  ACE_OS::sprintf (configuration_filename,
                   ACE_TEXT_ALWAYS_CHAR ("%s%s%s"),
                   home_directory.c_str (), ACE_DIRECTORY_SEPARATOR_STR, CONFIG_FILE);
  alterPathForPlatform (configuration_filename);

  return configuration_filename;
}

bool
Splot_Configuration::load (const std::string& filename_in)
{
  ACE_Configuration_Heap configuration_heap;
  if (configuration_heap.open ())
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to ACE_Configuration_Heap::open (): \"%m\", aborting\n")));

    return false;
  } // end IF

  ACE_Ini_ImpExp import (configuration_heap);
  if (import.import_config (ACE_TEXT (filename_in.c_str ())))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to ACE_Ini_ImpExp::import_config (\"%s\"): \"%m\", aborting\n"),
                ACE_TEXT (filename_in.c_str ())));

    return false;
  } // end IF

  // find/open "splot" section...
  ACE_Configuration_Section_Key section_key;
  if (configuration_heap.open_section (configuration_heap.root_section (),
                                       ACE_TEXT (CONFIGURATION_INI_FILE_ROOT_SECTION),
                                       0, // don't create, MUST exist !
                                       section_key))
  {
    ACE_ERROR ((LM_ERROR,
                ACE_TEXT ("failed to ACE_Configuration_Heap::open_section (\"%s\"): \"%m\", aborting\n"),
                ACE_TEXT (CONFIGURATION_INI_FILE_ROOT_SECTION)));

    return false;
  } // end IF

  // import values...
  int value_index = 0;
  ACE_TString value_name, value;
  ACE_Configuration::VALUETYPE value_type;
  std::istringstream converter;
  int factor = 1;
  while (configuration_heap.enumerate_values (section_key,
                                              value_index,
                                              value_name,
                                              value_type) == 0)
  {
    if (configuration_heap.get_string_value (section_key,
                                             ACE_TEXT (value_name.c_str ()),
                                             value))
    {
      ACE_ERROR ((LM_ERROR,
                  ACE_TEXT ("failed to ACE_Configuration_Heap::get_string_value (\"%s\"): \"%m\", aborting\n"),
                  ACE_TEXT (value_name.c_str ())));

      return false;
    } // end IF

    //     ACE_DEBUG ((LM_DEBUG,
    //                 ACE_TEXT ("enumerated \"%s\", type %d\n"),
    //                 ACE_TEXT (value_name.c_str()),
    //                 value_type));

    converter.clear ();
    // *TODO*: move these strings...
    if (value_name == ACE_TEXT ("screen_width"))
    {
      converter.str (value.c_str ());
      converter >> std::hex >> configuration_.screen_width;
    } // end IF
    else if (value_name == ACE_TEXT ("screen_height"))
    {
      converter.str (value.c_str ());
      converter >> std::hex >> configuration_.screen_height;
    } // end IF
    else if (value_name == ACE_TEXT ("full_screen"))
    {
      converter.str (value.c_str ());
      converter >> configuration_.full_screen;
      if (converter.fail ())
      {
        converter.clear ();
        converter >> std::boolalpha >> configuration_.full_screen;
      } // end IF
    } // end IF
    else if (value_name == ACE_TEXT ("blend_enabled"))
    {
      converter.str (value.c_str ());
      converter >> configuration_.blend_enabled;
      if (converter.fail ())
      {
        converter.clear ();
        converter >> std::boolalpha >> configuration_.blend_enabled;
      } // end IF
    } // end IF
    else if (value_name == ACE_TEXT ("graphics_level"))
    {
      converter.str (value.c_str ());
      converter >> std::hex >> configuration_.graphics_level;
    } // end IF
    else if (value_name == ACE_TEXT ("true_color"))
    {
      converter.str (value.c_str ());
      converter >> configuration_.true_color;
      if (converter.fail ())
      {
        converter.clear ();
        converter >> std::boolalpha >> configuration_.true_color;
      } // end IF
    } // end IF
    else if (value_name == ACE_TEXT ("texture_border"))
    {
      converter.str (value.c_str ());
      converter >> configuration_.texture_border;
      if (converter.fail ())
      {
        converter.clear ();
        converter >> std::boolalpha >> configuration_.texture_border;
      } // end IF
    } // end IF
    else if (value_name == ACE_TEXT ("show_FPS"))
    {
      converter.str (value.c_str ());
      converter >> configuration_.show_FPS;
      if (converter.fail ())
      {
        converter.clear ();
        converter >> std::boolalpha >> configuration_.show_FPS;
      } // end IF
    } // end IF
    else if (value_name == ACE_TEXT ("view_gamma"))
    {
      configuration_.view_gamma = (float)::atof (value.c_str ());
    } // end IF
    else if (value_name == ACE_TEXT ("text_toolkit_type"))
    {
      int tmp = -1;
      converter.str (value.c_str ());
      converter >> std::hex >> tmp;
      configuration_.text_toolkit_type = (TextToolkitType_t)tmp;
    } // end IF
    else if (value_name == ACE_TEXT ("auto_speed"))
    {
      converter.str (value.c_str ());
      converter >> configuration_.auto_speed;
      if (converter.fail ())
      {
        converter.clear ();
        converter >> std::boolalpha >> configuration_.auto_speed;
      } // end IF
    } // end IF
    else if (value_name == ACE_TEXT ("audio_enabled"))
    {
      converter.str (value.c_str ());
      converter >> configuration_.audio_enabled;
      if (converter.fail ())
      {
        converter.clear ();
        converter >> std::boolalpha >> configuration_.audio_enabled;
      } // end IF
    } // end IF
    else if (value_name == ACE_TEXT ("swap_stereo"))
    {
      converter.str (value.c_str ());
      converter >> configuration_.swap_stereo;
      if (converter.fail ())
      {
        converter.clear ();
        converter >> std::boolalpha >> configuration_.swap_stereo;
      } // end IF
    } // end IF
    else if (value_name == ACE_TEXT ("use_playlist"))
    {
      converter.str (value.c_str ());
      converter >> configuration_.use_playlist;
      if (converter.fail ())
      {
        converter.clear ();
        converter >> std::boolalpha >> configuration_.use_playlist;
      } // end IF
    } // end IF
    else if (value_name == ACE_TEXT ("use_CD_audio"))
    {
      converter.str (value.c_str ());
      converter >> configuration_.use_CD_audio;
      if (converter.fail ())
      {
        converter.clear ();
        converter >> std::boolalpha >> configuration_.use_CD_audio;
      } // end IF
    } // end IF
    else if (value_name == ACE_TEXT ("use_MIDI_music"))
    {
      converter.str (value.c_str ());
      converter >> configuration_.use_MIDI_music;
      if (converter.fail ())
      {
        converter.clear ();
        converter >> std::boolalpha >> configuration_.use_MIDI_music;
      } // end IF
    } // end IF
    else if (value_name == ACE_TEXT ("skill"))
    {
      int tmp = -1;
      converter.str (value.c_str ());
      converter >> std::hex >> tmp;
      configuration_.skill_base = (float)tmp/(float)10;
    } // end IF
    else if (value_name == ACE_TEXT ("volume_sound"))
    {
      int tmp = -1;
      converter.str (value.c_str ());
      converter >> std::hex >> tmp;
#ifdef USE_SDLMIXER_AUDIO
      factor = MIX_MAX_VOLUME;
#else
#error "sorry, OPENAL not fully supported yet"
#endif
     configuration_.volume_sound = (float)tmp/(float)factor;
    } // end IF
    else if (value_name == ACE_TEXT ("volume_music"))
    {
      int tmp = -1;
      converter.str (value.c_str ());
      converter >> std::hex >> tmp;
#ifdef USE_SDLMIXER_AUDIO
      factor = MIX_MAX_VOLUME;
#else
#error "sorry, OPENAL not fully supported yet"
#endif
      configuration_.volume_music = (float)tmp/(float)factor;
    } // end IF
    else if (value_name == ACE_TEXT ("audio_toolkit_type"))
    {
      int tmp = -1;
      converter.str (value.c_str ());
      converter >> std::hex >> tmp;
      configuration_.audio_toolkit_type = (AudioToolkitType_t)tmp;
    } // end IF
    else
    {
      ACE_ERROR ((LM_ERROR,
                  ACE_TEXT ("unexpected key \"%s\", continuing\n"),
                  ACE_TEXT (value_name.c_str ())));
    } // end ELSE

    ++value_index;
  } // end WHILE

  //// find/open "audio" section...
  //if (configuration_heap.open_section (configuration_heap.root_section (),
  //                                     ACE_TEXT_ALWAYS_CHAR (CONFIGURATION_INI_FILE_ROOT_SECTION),
  //                                     0, // don't create, MUST exist !
  //                                     section_key))
  //{
  //  ACE_ERROR ((LM_ERROR,
  //              ACE_TEXT ("failed to ACE_Configuration_Heap::open_section (\"%s\"): \"%m\", aborting\n"),
  //              ACE_TEXT (CONFIGURATION_INI_FILE_ROOT_SECTION)));

  //  return false;
  //} // end IF

  //// import values...
  //value_index = 0;
  //while (configuration_heap.enumerate_values (section_key,
  //                                            value_index,
  //                                            value_name,
  //                                            value_type) == 0)
  //{
  //  if (configuration_heap.get_string_value (section_key,
  //                                           ACE_TEXT (value_name.c_str()),
  //                                           value))
  //  {
  //    ACE_ERROR ((LM_ERROR,
  //                ACE_TEXT ("failed to ACE_Configuration_Heap::get_string_value(\"%s\"): \"%m\", aborting\n"),
  //                ACE_TEXT (value_name.c_str ())));

  //    return false;
  //  } // end IF

  //  //     ACE_DEBUG ((LM_DEBUG,
  //  //                 ACE_TEXT ("enumerated \"%s\", type %d\n"),
  //  //                 ACE_TEXT (value_name.c_str ()),
  //  //                 value_type));

  //  // *TODO*: move these strings...
  //  if (value_name == ACE_TEXT ("server"))
  //  {
  //    config_out.network_configuration.server = val_value.c_str();
  //  }
  //  else if (val_name == ACE_TEXT("port"))
  //  {
  //    config_out.network_configuration.port = ::atoi(val_value.c_str());
  //  }
  //  else
  //  {
  //    ACE_ERROR ((LM_ERROR,
  //                ACE_TEXT ("unexpected key \"%s\", continuing\n"),
  //                ACE_TEXT (value_name.c_str ())));
  //  } // end ELSE

  //  ++value_index;
  //} // end WHILE

  ACE_DEBUG ((LM_DEBUG,
              ACE_TEXT ("imported \"%s\"...\n"),
              ACE_TEXT (filename_in.c_str ())));

  return true;
}

bool
Splot_Configuration::save ()
{
  ACE_Configuration_Heap configuration_heap;
  if (configuration_heap.open ())
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to ACE_Configuration_Heap::open (): \"%m\", aborting\n")));

    return false;
  } // end IF

  // create "splot" section...
  ACE_Configuration_Section_Key section_key;
  if (configuration_heap.open_section (configuration_heap.root_section (),
                                       ACE_TEXT (CONFIGURATION_INI_FILE_ROOT_SECTION),
                                       1, // create
                                       section_key))
  {
    ACE_ERROR ((LM_ERROR,
                ACE_TEXT("failed to ACE_Configuration_Heap::open_section (\"%s\"): \"%m\", aborting\n"),
                ACE_TEXT (CONFIGURATION_INI_FILE_ROOT_SECTION)));

    return false;
  } // end IF

  // export values...
  std::string value_name = ACE_TEXT_ALWAYS_CHAR ("screen_width");
  if (configuration_heap.set_integer_value (section_key,
                                            ACE_TEXT (value_name.c_str ()),
                                            (u_int)configuration_.screen_width))
  {
    ACE_ERROR ((LM_ERROR,
                ACE_TEXT ("failed to ACE_Configuration_Heap::set_integer_value (\"%s\"): \"%m\", aborting\n"),
                ACE_TEXT (value_name.c_str ())));

    return false;
  } // end IF
  value_name = ACE_TEXT_ALWAYS_CHAR ("screen_height");
  if (configuration_heap.set_integer_value (section_key,
                                            ACE_TEXT (value_name.c_str ()),
                                            (u_int)configuration_.screen_height))
  {
    ACE_ERROR ((LM_ERROR,
                ACE_TEXT ("failed to ACE_Configuration_Heap::set_integer_value (\"%s\"): \"%m\", aborting\n"),
                ACE_TEXT (value_name.c_str ())));

    return false;
  } // end IF
  value_name = ACE_TEXT_ALWAYS_CHAR ("full_screen");
  if (configuration_heap.set_integer_value (section_key,
                                            ACE_TEXT (value_name.c_str ()),
                                            (u_int)configuration_.full_screen))
  {
    ACE_ERROR ((LM_ERROR,
                ACE_TEXT ("failed to ACE_Configuration_Heap::set_integer_value (\"%s\"): \"%m\", aborting\n"),
                ACE_TEXT (value_name.c_str ())));

    return false;
  } // end IF
  value_name = ACE_TEXT_ALWAYS_CHAR ("blend_enabled");
  if (configuration_heap.set_integer_value (section_key,
                                            ACE_TEXT (value_name.c_str ()),
                                            (u_int)configuration_.blend_enabled))
  {
    ACE_ERROR ((LM_ERROR,
                ACE_TEXT ("failed to ACE_Configuration_Heap::set_integer_value (\"%s\"): \"%m\", aborting\n"),
                ACE_TEXT (value_name.c_str ())));

    return false;
  } // end IF
  value_name = ACE_TEXT_ALWAYS_CHAR ("graphics_level");
  if (configuration_heap.set_integer_value (section_key,
                                            ACE_TEXT (value_name.c_str ()),
                                            (u_int)configuration_.graphics_level))
  {
    ACE_ERROR((LM_ERROR,
               ACE_TEXT ("failed to ACE_Configuration_Heap::set_integer_value (\"%s\"): \"%m\", aborting\n"),
               ACE_TEXT (value_name.c_str ())));

    return false;
  } // end IF
  value_name = ACE_TEXT_ALWAYS_CHAR ("true_color");
  if (configuration_heap.set_integer_value (section_key,
                                            ACE_TEXT (value_name.c_str ()),
                                            (u_int)configuration_.true_color))
  {
    ACE_ERROR ((LM_ERROR,
                ACE_TEXT ("failed to ACE_Configuration_Heap::set_integer_value (\"%s\"): \"%m\", aborting\n"),
                ACE_TEXT (value_name.c_str ())));

    return false;
  } // end IF
  value_name = ACE_TEXT_ALWAYS_CHAR ("texture_border");
  if (configuration_heap.set_integer_value (section_key,
                                            ACE_TEXT (value_name.c_str ()),
                                            (u_int)configuration_.texture_border))
  {
    ACE_ERROR ((LM_ERROR,
                ACE_TEXT ("failed to ACE_Configuration_Heap::set_integer_value (\"%s\"): \"%m\", aborting\n"),
                ACE_TEXT (value_name.c_str ())));

    return false;
  } // end IF
  value_name = ACE_TEXT_ALWAYS_CHAR ("show_FPS");
  if (configuration_heap.set_integer_value (section_key,
                                            ACE_TEXT (value_name.c_str()),
                                            (u_int)configuration_.show_FPS))
  {
    ACE_ERROR ((LM_ERROR,
                ACE_TEXT ("failed to ACE_Configuration_Heap::set_integer_value (\"%s\"): \"%m\", aborting\n"),
                ACE_TEXT (value_name.c_str ())));

    return false;
  } // end IF
  value_name = ACE_TEXT_ALWAYS_CHAR ("view_gamma");
  if (configuration_heap.set_integer_value (section_key,
                                            ACE_TEXT (value_name.c_str ()),
                                            (u_int)configuration_.view_gamma))
  {
    ACE_ERROR ((LM_ERROR,
                ACE_TEXT ("failed to ACE_Configuration_Heap::set_integer_value (\"%s\"): \"%m\", aborting\n"),
                ACE_TEXT (value_name.c_str ())));

    return false;
  } // end IF
  value_name = ACE_TEXT_ALWAYS_CHAR ("text_toolkit_type");
  if (configuration_heap.set_integer_value (section_key,
                                            ACE_TEXT (value_name.c_str ()),
                                            (u_int)configuration_.text_toolkit_type))
  {
    ACE_ERROR ((LM_ERROR,
                ACE_TEXT ("failed to ACE_Configuration_Heap::set_integer_value (\"%s\"): \"%m\", aborting\n"),
                ACE_TEXT (value_name.c_str ())));

    return false;
  } // end IF
  value_name = ACE_TEXT_ALWAYS_CHAR ("auto_speed");
  if (configuration_heap.set_integer_value (section_key,
                                            ACE_TEXT (value_name.c_str ()),
                                            (u_int)configuration_.auto_speed))
  {
    ACE_ERROR ((LM_ERROR,
                ACE_TEXT ("failed to ACE_Configuration_Heap::set_integer_value (\"%s\"): \"%m\", aborting\n"),
                ACE_TEXT (value_name.c_str ())));

    return false;
  } // end IF
  value_name = ACE_TEXT_ALWAYS_CHAR ("audio_enabled");
  if (configuration_heap.set_integer_value (section_key,
                                            ACE_TEXT (value_name.c_str ()),
                                            (u_int)configuration_.audio_enabled))
  {
    ACE_ERROR ((LM_ERROR,
                ACE_TEXT ("failed to ACE_Configuration_Heap::set_integer_value (\"%s\"): \"%m\", aborting\n"),
                ACE_TEXT (value_name.c_str ())));

    return false;
  } // end IF
  value_name = ACE_TEXT_ALWAYS_CHAR ("swap_stereo");
  if (configuration_heap.set_integer_value (section_key,
                                            ACE_TEXT (value_name.c_str ()),
                                            (u_int)configuration_.swap_stereo))
  {
    ACE_ERROR ((LM_ERROR,
                ACE_TEXT ("failed to ACE_Configuration_Heap::set_integer_value (\"%s\"): \"%m\", aborting\n"),
                ACE_TEXT (value_name.c_str ())));

    return false;
  } // end IF
  value_name = ACE_TEXT_ALWAYS_CHAR ("use_playlist");
  if (configuration_heap.set_integer_value (section_key,
                                            ACE_TEXT (value_name.c_str ()),
                                            (u_int)configuration_.use_playlist))
  {
    ACE_ERROR ((LM_ERROR,
                ACE_TEXT ("failed to ACE_Configuration_Heap::set_integer_value (\"%s\"): \"%m\", aborting\n"),
                ACE_TEXT (value_name.c_str ())));

    return false;
  } // end IF
  value_name = ACE_TEXT_ALWAYS_CHAR ("use_CD_audio");
  if (configuration_heap.set_integer_value (section_key,
                                            ACE_TEXT (value_name.c_str ()),
                                            (u_int)configuration_.use_CD_audio))
  {
    ACE_ERROR ((LM_ERROR,
                ACE_TEXT ("failed to ACE_Configuration_Heap::set_integer_value (\"%s\"): \"%m\", aborting\n"),
                ACE_TEXT (value_name.c_str ())));

    return false;
  } // end IF
  value_name = ACE_TEXT_ALWAYS_CHAR ("use_MIDI_music");
  if (configuration_heap.set_integer_value (section_key,
                                            ACE_TEXT (value_name.c_str ()),
                                            (u_int)configuration_.use_MIDI_music))
  {
    ACE_ERROR ((LM_ERROR,
                ACE_TEXT ("failed to ACE_Configuration_Heap::set_integer_value (\"%s\"): \"%m\", aborting\n"),
                ACE_TEXT (value_name.c_str ())));

    return false;
  } // end IF
  value_name = ACE_TEXT_ALWAYS_CHAR ("skill");
  u_int factor = 10;
  if (configuration_heap.set_integer_value (section_key,
                                            ACE_TEXT (value_name.c_str ()),
                                            (u_int)(configuration_.skill_base*factor)))
  {
    ACE_ERROR ((LM_ERROR,
                ACE_TEXT ("failed to ACE_Configuration_Heap::set_integer_value (\"%s\"): \"%m\", aborting\n"),
                ACE_TEXT (value_name.c_str ())));

    return false;
  } // end IF
  value_name = ACE_TEXT_ALWAYS_CHAR ("volume_sound");
  factor = 1;
#ifdef USE_SDLMIXER_AUDIO
  factor = MIX_MAX_VOLUME;
#else
#error "sorry, OPENAL not fully supported yet"
#endif
  if (configuration_heap.set_integer_value (section_key,
                                            ACE_TEXT (value_name.c_str ()),
                                            (u_int)(configuration_.volume_sound*factor)))
  {
    ACE_ERROR ((LM_ERROR,
                ACE_TEXT ("failed to ACE_Configuration_Heap::set_integer_value (\"%s\"): \"%m\", aborting\n"),
                ACE_TEXT (value_name.c_str ())));

    return false;
  } // end IF
  value_name = ACE_TEXT_ALWAYS_CHAR ("volume_music");
  if (configuration_heap.set_integer_value (section_key,
                                            ACE_TEXT (value_name.c_str ()),
                                            (u_int)(configuration_.volume_music*factor)))
  {
    ACE_ERROR ((LM_ERROR,
                ACE_TEXT ("failed to ACE_Configuration_Heap::set_integer_value (\"%s\"): \"%m\", aborting\n"),
                ACE_TEXT (value_name.c_str ())));

    return false;
  } // end IF
  value_name = ACE_TEXT_ALWAYS_CHAR ("audio_toolkit_type");
  if (configuration_heap.set_integer_value (section_key,
                                            ACE_TEXT (value_name.c_str ()),
                                            (u_int)configuration_.audio_toolkit_type))
  {
    ACE_ERROR ((LM_ERROR,
                ACE_TEXT ("failed to ACE_Configuration_Heap::set_integer_value (\"%s\"): \"%m\", aborting\n"),
                ACE_TEXT (value_name.c_str ())));

    return false;
  } // end IF

  ACE_Ini_ImpExp ini_export (configuration_heap);
  if (ini_export.export_config (ACE_TEXT (Splot_Configuration::getFileName ().c_str ())))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to ACE_Ini_ImpExp::export_config (\"%s\"): \"%m\", aborting\n"),
                ACE_TEXT (Splot_Configuration::getFileName ().c_str ())));

    return false;
  } // end IF

  return true;
}

const Configuration_t&
Splot_Configuration::get () const
{
  return configuration_;
}

void
Splot_Configuration::setScreenSize (int mode_in)
{
  int mode = mode_in;
  if (mode > MAX_SCREEN_SIZE)
    mode = MAX_SCREEN_SIZE;
  if (mode < MIN_SCREEN_SIZE)
    mode = MIN_SCREEN_SIZE;

  switch (mode)
  {
    case 0:
      configuration_.screen_width = 320;
      configuration_.screen_height = 200;
      break;
    case 1:
      configuration_.screen_width = 512;
      configuration_.screen_height = 384;
      break;
    case 2:
      configuration_.screen_width = 640;
      configuration_.screen_height = 400;
      break;
    case 3:
      configuration_.screen_width = 640;
      configuration_.screen_height = 480;
      break;
    case 4:
      configuration_.screen_width = 800;
      configuration_.screen_height = 600;
      break;
    case 5:
      configuration_.screen_width = 1024;
      configuration_.screen_height = 768;
      break;
    case 6:
      configuration_.screen_width = 1280;
      configuration_.screen_height = 800;
      break;
    case 7:
      configuration_.screen_width = 1280;
      configuration_.screen_height = 960;
      break;
    case 8:
      configuration_.screen_width = 1280;
      configuration_.screen_height = 1024;
      break;
    default:
      configuration_.screen_width = 1024;
      configuration_.screen_height = 768;
      break;
  } // end SWITCH
}

int
Splot_Configuration::approxScreenSize ()
{
  if (configuration_.screen_width >= 1280)
  {
    if (configuration_.screen_height >= 1024)
      return 8;
    if (configuration_.screen_height >= 960)
      return 7;
    if (configuration_.screen_height >= 800)
      return 6;
  } // end IF
  if (configuration_.screen_width >= 1024)
    if (configuration_.screen_height >= 768)
      return 5;
  if (configuration_.screen_width >= 800)
    if (configuration_.screen_height >= 600)
      return 4;
  if (configuration_.screen_width >= 640)
  {
    if (configuration_.screen_height >= 480)
      return 3;
    if (configuration_.screen_height >= 400)
      return 2;
  } // end IF
  if (configuration_.screen_width >= 512)
    if (configuration_.screen_height >= 384)
      return 1;

  return 0;
}
