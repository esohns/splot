#include "stdafx.h"

#include "configuration.h"

#include <sstream>

#include "ace/Get_Opt.h"
#include "ace/Configuration.h"
#include "ace/Configuration_Import_Export.h"

#include "defines.h"
#include "common.h"

Splot_Configuration::Splot_Configuration ()
{
  configuration_.screen_w = CONFIGURATION_DEFAULT_SCREEN_W;
  configuration_.screen_h = CONFIGURATION_DEFAULT_SCREEN_H;
  configuration_.screen_a = CONFIGURATION_DEFAULT_SCREEN_A;
  configuration_.screen_FOV = CONFIGURATION_DEFAULT_SCREEN_FOV;
  configuration_.screen_near = CONFIGURATION_DEFAULT_SCREEN_NEAR;
  configuration_.screen_far = CONFIGURATION_DEFAULT_SCREEN_FAR;
  configuration_.screen_bound[0] = CONFIGURATION_DEFAULT_SCREEN_BOUND_0;
  configuration_.screen_bound[1] = CONFIGURATION_DEFAULT_SCREEN_BOUND_1;
  configuration_.z_trans = CONFIGURATION_DEFAULT_Z_TRANS;

  configuration_.full_screen = CONFIGURATION_DEFAULT_FULLSCREEN;
  configuration_.blend_enabled = CONFIGURATION_DEFAULT_BLEND_ENABLED;
  configuration_.gfx_level = 2;
  configuration_.true_color = CONFIGURATION_DEFAULT_TRUE_COLOR;
  configuration_.tex_border = CONFIGURATION_DEFAULT_TEX_BORDER;
  configuration_.show_fps = CONFIGURATION_DEFAULT_SHOW_FPS;
  configuration_.view_gamma = CONFIGURATION_DEFAULT_VIEW_GAMMA;
  configuration_.text_type = CONFIGURATION_DEFAULT_TEXT_TYPE;

  configuration_.auto_speed = CONFIGURATION_DEFAULT_AUTO_SPEED;
  configuration_.movement_speed = CONFIGURATION_DEFAULT_MOVEMENT_SPEED;

  configuration_.audio_enabled = CONFIGURATION_DEFAULT_AUDIO_ENABLED;
  configuration_.swap_stereo = CONFIGURATION_DEFAULT_SWAP_STEREO;
  configuration_.use_playList = CONFIGURATION_DEFAULT_USE_PLAYLIST;
  configuration_.use_cdrom = CONFIGURATION_DEFAULT_USE_CDROM;
  configuration_.cdrom_device = 0;
  configuration_.cdrom_count = 1;
  configuration_.vol_sound = CONFIGURATION_DEFAULT_VOL_SOUND;
  configuration_.vol_music = CONFIGURATION_DEFAULT_VOL_MUSIC;
  configuration_.audio_type = CONFIGURATION_DEFAULT_AUDIO_TYPE;

  configuration_.debug = CONFIGURATION_DEFAULT_DEBUG;

  configuration_.skill_base = CONFIGURATION_DEFAULT_SKILL_BASE;

  if (!load (Splot_Configuration::getFileName ()))
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to Splot_Configuration::load (\"%s\"), continuing\n"),
                ACE_TEXT (Splot_Configuration::getFileName ().c_str())));
}

Splot_Configuration::~Splot_Configuration()
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
                                   ACE_Get_Opt::OPTION_ARG_MODE::NO_ARG) == -1)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to ACE_Get_Opt::long_option (\"%s\"): \"%m\", aborting\n"),
                ACE_TEXT (OPTIONS_LONG_OPTION_NOAUDIO)));

    return false;
  } // end IF
  if (argument_parser.long_option (ACE_TEXT (OPTIONS_LONG_OPTION_NOBLEND),
                                   'b',
                                   ACE_Get_Opt::OPTION_ARG_MODE::NO_ARG) == -1)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to ACE_Get_Opt::long_option (\"%s\"): \"%m\", aborting\n"),
                ACE_TEXT (OPTIONS_LONG_OPTION_NOBLEND)));

    return false;
  } // end IF
  if (argument_parser.long_option (ACE_TEXT (OPTIONS_LONG_OPTION_FULLSCREEN),
                                   'f',
                                   ACE_Get_Opt::OPTION_ARG_MODE::NO_ARG) == -1)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to ACE_Get_Opt::long_option (\"%s\"): \"%m\", aborting\n"),
                ACE_TEXT (OPTIONS_LONG_OPTION_FULLSCREEN)));

    return false;
  } // end IF
  if (argument_parser.long_option( ACE_TEXT (OPTIONS_LONG_OPTION_VIDEO_MODE),
                                   'm',
                                   ACE_Get_Opt::OPTION_ARG_MODE::ARG_REQUIRED) == -1)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to ACE_Get_Opt::long_option (\"%s\"): \"%m\", aborting\n"),
                ACE_TEXT (OPTIONS_LONG_OPTION_VIDEO_MODE)));

    return false;
  } // end IF
  if (argument_parser.long_option (ACE_TEXT (OPTIONS_LONG_OPTION_NOTEXBORDER),
                                   't',
                                   ACE_Get_Opt::OPTION_ARG_MODE::NO_ARG) == -1)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to ACE_Get_Opt::long_option (\"%s\"): \"%m\", aborting\n"),
                ACE_TEXT (OPTIONS_LONG_OPTION_NOTEXBORDER)));

    return false;
  } // end IF
  if (argument_parser.long_option (ACE_TEXT (OPTIONS_LONG_OPTION_VERSION),
                                   'v',
                                   ACE_Get_Opt::OPTION_ARG_MODE::NO_ARG) == -1)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to ACE_Get_Opt::long_option (\"%s\"): \"%m\", aborting\n"),
                ACE_TEXT (OPTIONS_LONG_OPTION_VERSION)));

    return false;
  } // end IF
  if (argument_parser.long_option (ACE_TEXT (OPTIONS_LONG_OPTION_WINDOWED),
                                   'w',
                                   ACE_Get_Opt::OPTION_ARG_MODE::NO_ARG) == -1)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to ACE_Get_Opt::long_option (\"%s\"): \"%m\", aborting\n"),
                ACE_TEXT (OPTIONS_LONG_OPTION_WINDOWED)));

    return false;
  } // end IF

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
      case 'd':
        configuration_.debug = true;
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
      case 't':
        configuration_.tex_border = false;
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
          if (long_option == ACE_TEXT_ALWAYS_CHAR (OPTIONS_LONG_OPTION_FULLSCREEN))
            configuration_.full_screen = true;
          else if (long_option == ACE_TEXT_ALWAYS_CHAR (OPTIONS_LONG_OPTION_WINDOWED))
            configuration_.full_screen = false;
          else if (long_option == ACE_TEXT_ALWAYS_CHAR(OPTIONS_LONG_OPTION_VIDEO_MODE))
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
          else if (long_option == ACE_TEXT_ALWAYS_CHAR (OPTIONS_LONG_OPTION_NOTEXBORDER))
            configuration_.tex_border = false;
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
                    ACE_TEXT ("\t[-a/--noaudio] mute (defaults to false)\n")
                    ACE_TEXT ("\t[-b/--noblend] disable blending (OpenGL) (defaults to false)\n")
                    ACE_TEXT ("\t[-d/--debug] debug (defaults to false)\n")
                    ACE_TEXT ("\t[-f/--fullscreen] run in full-screen mode (defaults to true)\n")
                    ACE_TEXT ("\t[-m/--vidmode <mode>]\n\t\tmode\t0 =  512 x  384\n\t\t\t1 =  640 x  480\n\t\t\2 =  800 x  600\n\t\t\t3 = 1024 x  768\n\t\t\t4 = 1280 x  960\n")
                    ACE_TEXT ("\t[-t/--notexborder] do not set tex border color (OpenGL) (defaults to false)\n")
                    ACE_TEXT ("\t[-v/--version] show version information (defaults to false)\n")
                    ACE_TEXT ("\t[-w/--window] run in windowed mode (defaults to true)\n")
                    ACE_TEXT ("\n"),
                    argv_in[0]));

        return false;
      }
    } // end SWITCH
  } // end WHILE

  return true;
}

std::string
Splot_Configuration::getFileName ()
{
  char configuration_filename[PATH_MAX];

  char home_dir[PATH_MAX];
  const char* home_dir_p = ACE_OS::getenv ("HOME");
  if (!home_dir_p)
    ACE_OS::getcwd (home_dir, sizeof (home_dir));
  else
    ACE_OS::strcpy (home_dir, home_dir_p);

  sprintf (configuration_filename,
           ACE_TEXT_ALWAYS_CHAR ("%s/%s"),
           home_dir, CONFIG_FILE);
  alterPathForPlatform (configuration_filename);

  return configuration_filename;
}

//std::string
//Splot_Configuration::getOldFileName ()
//{
//  char configuration_filename[PATH_MAX];
//
//  char home_dir[PATH_MAX];
//  const char* home_dir_p = ACE_OS::getenv ("HOME");
//  if (!home_dir_p)
//    ACE_OS::getcwd (home_dir, sizeof (home_dir));
//  else
//    ACE_OS::strcpy (home_dir, home_dir_p);
//
//  sprintf (configuration_filename,
//           ACE_TEXT_ALWAYS_CHAR ("%s/.splot"CONFIG_EXT),
//           home_dir);
//  alterPathForPlatform (configuration_filename);
//
//  return configuration_filename;
//}

//void
//Splot_Configuration::readValues (FILE* file)
//{
//	char configStrings[32][64];
//	int numLines;
//	int tmp;
//	int i;
//#ifdef HAVE_LOCALE_H
//	char* locale = setlocale(LC_NUMERIC,"C");
//#endif
//	i = numLines = 0;
//	while( fgets(configStrings[i], 64, file) )
//		i++;
//	numLines = i;
//	for(i = 0; i < numLines; i++)
//	{
//		if(strncmp(configStrings[i], "screenSi", 8) == 0) { int screenSize; sscanf(configStrings[i], "screenSize %d\n", &screenSize); setScreenSize(screenSize); }
//		if(strncmp(configStrings[i], "screenWi", 8) == 0) { sscanf(configStrings[i], "screenWidth %d\n",   &m_screenW); }
//		if(strncmp(configStrings[i], "screenHe", 8) == 0) { sscanf(configStrings[i], "screenHeight %d\n",  &m_screenH); }
//		if(strncmp(configStrings[i], "mouseSpe", 8) == 0) { sscanf(configStrings[i], "mouseSpeed %f\n",    &m_movementSpeed); }
//		if(strncmp(configStrings[i], "movement", 8) == 0) { sscanf(configStrings[i], "movementSpeed %f\n", &m_movementSpeed); }
//		if(strncmp(configStrings[i], "gameSkil", 8) == 0) { sscanf(configStrings[i], "gameSkillBase %f\n", &m_gameSkillBase); }
//		if(strncmp(configStrings[i], "gfxLevel", 8) == 0) { sscanf(configStrings[i], "gfxLevel %d\n",      &m_gfxLevel);   }
//		if(strncmp(configStrings[i], "volSound", 8) == 0) { sscanf(configStrings[i], "volSound %f\n",      &m_volSound);   }
//		if(strncmp(configStrings[i], "volMusic", 8) == 0) { sscanf(configStrings[i], "volMusic %f\n",      &m_volMusic);   }
//		if(strncmp(configStrings[i], "full_scr", 8) == 0) { sscanf(configStrings[i], "full_screen %d\n",  &tmp);	m_full_screen = (bool)tmp; }
//		if(strncmp(configStrings[i], "true_col", 8) == 0) { sscanf(configStrings[i], "true_color %d\n",   &tmp);	m_true_color  = (bool)tmp; }
//		if(strncmp(configStrings[i], "swap_ste", 8) == 0) { sscanf(configStrings[i], "swap_stereo %d\n",  &tmp);	m_swap_stereo = (bool)tmp;  }
//		if(strncmp(configStrings[i], "auto_spe", 8) == 0) { sscanf(configStrings[i], "auto_speed %d\n",   &tmp);	m_auto_speed  = (bool)tmp;  }
//		if(strncmp(configStrings[i], "show_fps", 8) == 0) { sscanf(configStrings[i], "show_fps %d\n",     &tmp);	m_show_fps    = (bool)tmp;  }
//		if(strncmp(configStrings[i], "use_play", 8) == 0) { sscanf(configStrings[i], "use_playList %d\n", &tmp);	m_use_playList= (bool)tmp;  }
//#ifdef USE_SDL_CDROM
//		if(strncmp(configStrings[i], "use_cdro", 8) == 0) { sscanf(configStrings[i], "use_cdrom %d\n",    &tmp);	m_use_cdrom   = (bool)tmp;  }
//#endif // USE_SDL_CDROM
//		if(strncmp(configStrings[i], "debug",    5) == 0) { sscanf(configStrings[i], "debug %d\n",        &tmp);	m_debug       = (bool)tmp;  }
//		if(strncmp(configStrings[i], "audioTyp", 8) == 0) { sscanf(configStrings[i], "audioType %d\n",    &tmp);	m_audioType = (AudioType)tmp; }
//		if(strncmp(configStrings[i], "textType", 8) == 0) { sscanf(configStrings[i], "textType %d\n",    &tmp);	m_textType = (TextType)tmp; }
//		if(strncmp(configStrings[i], "maxLevel", 8) == 0) { sscanf(configStrings[i], "maxLevel %d\n",      &m_maxLevel);  }
//		if(strncmp(configStrings[i], "viewGamm", 8) == 0) { sscanf(configStrings[i], "viewGamma %f\n",     &m_viewGamma); }
//#ifdef USE_SDL_CDROM
//		if(strncmp(configStrings[i], "cdromCou", 8) == 0) { sscanf(configStrings[i], "cdromCount %d\n",    &m_cdromCount); }
//		if(strncmp(configStrings[i], "cdromDev", 8) == 0) { sscanf(configStrings[i], "cdromDevice %d\n",   &m_cdromDevice); }
//#endif // USE_SDL_CDROM
//	}
//#ifdef HAVE_LOCALE_H
//	setlocale(LC_NUMERIC,locale);
//#endif
//}

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

    // *TODO*: move these strings...
    if (value_name == ACE_TEXT ("screen_w"))
    {
      configuration_.screen_w = ::atoi (value.c_str ());
    } // end IF
    else if (value_name == ACE_TEXT ("screen_h"))
    {
      configuration_.screen_h = ::atoi (value.c_str ());
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
    else if (value_name == ACE_TEXT ("gfx_level"))
    {
      configuration_.gfx_level = ::atoi (value.c_str ());
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
    else if (value_name == ACE_TEXT ("tex_border"))
    {
      converter.str (value.c_str ());
      converter >> configuration_.tex_border;
      if (converter.fail ())
      {
        converter.clear ();
        converter >> std::boolalpha >> configuration_.tex_border;
      } // end IF
    } // end IF
    else if (value_name == ACE_TEXT ("show_fps"))
    {
      converter.str (value.c_str ());
      converter >> configuration_.show_fps;
      if (converter.fail ())
      {
        converter.clear ();
        converter >> std::boolalpha >> configuration_.show_fps;
      } // end IF
    } // end IF
    else if (value_name == ACE_TEXT ("view_gamma"))
    {
      configuration_.view_gamma = (float)::atof (value.c_str ());
    } // end IF
    else if (value_name == ACE_TEXT ("text_type"))
    {
      configuration_.text_type = (TextType_t)::atoi (value.c_str ());
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
    else if (value_name == ACE_TEXT ("use_playList"))
    {
      converter.str (value.c_str ());
      converter >> configuration_.use_playList;
      if (converter.fail ())
      {
        converter.clear ();
        converter >> std::boolalpha >> configuration_.use_playList;
      } // end IF
    } // end IF
    else if (value_name == ACE_TEXT ("use_cdrom"))
    {
      converter.str (value.c_str ());
      converter >> configuration_.use_cdrom;
      if (converter.fail ())
      {
        converter.clear ();
        converter >> std::boolalpha >> configuration_.use_cdrom;
      } // end IF
    } // end IF
    else if (value_name == ACE_TEXT ("vol_sound"))
    {
      configuration_.vol_sound = (float)::atof (value.c_str ());
    } // end IF
    else if (value_name == ACE_TEXT ("vol_music"))
    {
      configuration_.vol_music = (float)::atof (value.c_str ());
    } // end IF
    else if (value_name == ACE_TEXT ("audio_type"))
    {
      configuration_.audio_type = (AudioType_t)::atoi (value.c_str ());
    } // end IF
    else if (value_name == ACE_TEXT ("skill_base"))
    {
      configuration_.skill_base = (float)::atof (value.c_str ());
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
              ACE_TEXT (filename_in.c_str())));

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
  std::string value_name = ACE_TEXT_ALWAYS_CHAR ("screen_w");
  if (configuration_heap.set_integer_value (section_key,
                                            ACE_TEXT (value_name.c_str ()),
                                            (u_int)configuration_.screen_w))
  {
    ACE_ERROR ((LM_ERROR,
                ACE_TEXT ("failed to ACE_Configuration_Heap::set_integer_value (\"%s\"): \"%m\", aborting\n"),
                ACE_TEXT (value_name.c_str ())));

    return false;
  } // end IF
  value_name = ACE_TEXT_ALWAYS_CHAR ("screen_h");
  if (configuration_heap.set_integer_value (section_key,
                                            ACE_TEXT (value_name.c_str ()),
                                            (u_int)configuration_.screen_h))
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
  value_name = ACE_TEXT_ALWAYS_CHAR ("gfx_level");
  if (configuration_heap.set_integer_value (section_key,
                                            ACE_TEXT (value_name.c_str ()),
                                            (u_int)configuration_.gfx_level))
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
  value_name = ACE_TEXT_ALWAYS_CHAR ("tex_border");
  if (configuration_heap.set_integer_value (section_key,
                                            ACE_TEXT (value_name.c_str ()),
                                            (u_int)configuration_.tex_border))
  {
    ACE_ERROR ((LM_ERROR,
                ACE_TEXT ("failed to ACE_Configuration_Heap::set_integer_value (\"%s\"): \"%m\", aborting\n"),
                ACE_TEXT (value_name.c_str ())));

    return false;
  } // end IF
  value_name = ACE_TEXT_ALWAYS_CHAR ("show_fps");
  if (configuration_heap.set_integer_value (section_key,
                                            ACE_TEXT (value_name.c_str()),
                                            (u_int)configuration_.show_fps))
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
  value_name = ACE_TEXT_ALWAYS_CHAR ("text_type");
  if (configuration_heap.set_integer_value (section_key,
                                            ACE_TEXT (value_name.c_str ()),
                                            (u_int)configuration_.text_type))
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
  value_name = ACE_TEXT_ALWAYS_CHAR ("use_playList");
  if (configuration_heap.set_integer_value (section_key,
                                            ACE_TEXT (value_name.c_str ()),
                                            (u_int)configuration_.use_playList))
  {
    ACE_ERROR ((LM_ERROR,
                ACE_TEXT ("failed to ACE_Configuration_Heap::set_integer_value (\"%s\"): \"%m\", aborting\n"),
                ACE_TEXT (value_name.c_str ())));

    return false;
  } // end IF
  value_name = ACE_TEXT_ALWAYS_CHAR ("use_cdrom");
  if (configuration_heap.set_integer_value (section_key,
                                            ACE_TEXT (value_name.c_str ()),
                                            (u_int)configuration_.use_cdrom))
  {
    ACE_ERROR ((LM_ERROR,
                ACE_TEXT ("failed to ACE_Configuration_Heap::set_integer_value (\"%s\"): \"%m\", aborting\n"),
                ACE_TEXT (value_name.c_str ())));

    return false;
  } // end IF
  value_name = ACE_TEXT_ALWAYS_CHAR ("vol_sound");
  if (configuration_heap.set_integer_value (section_key,
                                            ACE_TEXT (value_name.c_str ()),
                                            (u_int)configuration_.vol_sound))
  {
    ACE_ERROR ((LM_ERROR,
                ACE_TEXT ("failed to ACE_Configuration_Heap::set_integer_value (\"%s\"): \"%m\", aborting\n"),
                ACE_TEXT (value_name.c_str ())));

    return false;
  } // end IF
  value_name = ACE_TEXT_ALWAYS_CHAR ("vol_music");
  if (configuration_heap.set_integer_value (section_key,
                                            ACE_TEXT (value_name.c_str ()),
                                            (u_int)configuration_.vol_music))
  {
    ACE_ERROR ((LM_ERROR,
                ACE_TEXT ("failed to ACE_Configuration_Heap::set_integer_value (\"%s\"): \"%m\", aborting\n"),
                ACE_TEXT (value_name.c_str ())));

    return false;
  } // end IF
  value_name = ACE_TEXT_ALWAYS_CHAR ("audio_type");
  if (configuration_heap.set_integer_value (section_key,
                                            ACE_TEXT (value_name.c_str ()),
                                            (u_int)configuration_.audio_type))
  {
    ACE_ERROR ((LM_ERROR,
                ACE_TEXT ("failed to ACE_Configuration_Heap::set_integer_value (\"%s\"): \"%m\", aborting\n"),
                ACE_TEXT (value_name.c_str ())));

    return false;
  } // end IF
  value_name = ACE_TEXT_ALWAYS_CHAR ("skill_base");
  if (configuration_heap.set_integer_value (section_key,
                                            ACE_TEXT (value_name.c_str ()),
                                            (u_int)configuration_.skill_base))
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

//		fprintf(file, "# Chromium B.S.U. configuration file\n");
//		fprintf(file, "# Please read the documentation for more info\n");
//		fprintf(file, "# Only modifications to option values are preserved.\n");
//		fprintf(file, "use_playList %d\n",	(int)m_use_playList);
//#ifdef USE_SDL_CDROM
//		fprintf(file, "use_cdrom %d\n",		(int)m_use_cdrom);
//#endif // USE_SDL_CDROM
//		fprintf(file, "debug %d\n",		(int)m_debug);
//		fprintf(file, "full_screen %d\n", 	(int)m_full_screen);
//		fprintf(file, "true_color %d\n", 	(int)m_true_color);
//		fprintf(file, "swap_stereo %d\n",	(int)m_swap_stereo);
//		fprintf(file, "auto_speed %d\n",	(int)m_auto_speed);
//		fprintf(file, "show_fps %d\n",		(int)m_show_fps);
//		fprintf(file, "screenWidth %d\n",	m_screenW);
//		fprintf(file, "screenHeight %d\n",	m_screenH);
//		fprintf(file, "gfxLevel %d\n",		m_gfxLevel);
//		fprintf(file, "gameSkillBase %g\n",	m_gameSkillBase);
//		fprintf(file, "movementSpeed %g\n",	m_movementSpeed);
//		fprintf(file, "maxLevel %d\n",		m_maxLevel);
//		fprintf(file, "volSound %g\n",		m_volSound);
//		fprintf(file, "volMusic %g\n",		m_volMusic);
//		fprintf(file, "viewGamma %g\n",		m_viewGamma);
//		fprintf(file, "audioType %d\n",		(int)m_audioType);
//		fprintf(file, "textType %d\n",		(int)m_textType);
//#ifdef USE_SDL_CDROM
//		fprintf(file, "cdromCount %d\n",	m_cdromCount);
//		fprintf(file, "cdromDevice %d\n",	m_cdromDevice);
//#endif // USE_SDL_CDROM
//#ifdef HAVE_LOCALE_H
//		setlocale(LC_NUMERIC,locale);
//#endif

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
      configuration_.screen_w = 320;
      configuration_.screen_h = 200;
      break;
    case 1:
      configuration_.screen_w = 512;
      configuration_.screen_h = 384;
      break;
    case 2:
      configuration_.screen_w = 640;
      configuration_.screen_h = 400;
      break;
    case 3:
      configuration_.screen_w = 640;
      configuration_.screen_h = 480;
      break;
    case 4:
      configuration_.screen_w = 800;
      configuration_.screen_h = 600;
      break;
    case 5:
      configuration_.screen_w = 1024;
      configuration_.screen_h = 768;
      break;
    case 6:
      configuration_.screen_w = 1280;
      configuration_.screen_h = 800;
      break;
    case 7:
      configuration_.screen_w = 1280;
      configuration_.screen_h = 960;
      break;
    case 8:
      configuration_.screen_w = 1280;
      configuration_.screen_h = 1024;
      break;
    default:
      configuration_.screen_w = 1024;
      configuration_.screen_h = 768;
      break;
  } // end SWITCH

  configuration_.screen_a = (float)configuration_.screen_w / (float)configuration_.screen_h;
}

int
Splot_Configuration::approxScreenSize ()
{
  if (configuration_.screen_w >= 1280)
  {
    if (configuration_.screen_h >= 1024)
      return 8;
    if (configuration_.screen_h >= 960)
      return 7;
    if (configuration_.screen_h >= 800)
      return 6;
  } // end IF
  if (configuration_.screen_w >= 1024)
    if (configuration_.screen_h >= 768)
      return 5;
  if (configuration_.screen_w >= 800)
    if (configuration_.screen_h >= 600)
      return 4;
  if (configuration_.screen_w >= 640)
  {
    if (configuration_.screen_h >= 480)
      return 3;
    if (configuration_.screen_h >= 400)
      return 2;
  } // end IF
  if (configuration_.screen_w >= 512)
    if (configuration_.screen_h >= 384)
      return 1;

  return 0;
}
