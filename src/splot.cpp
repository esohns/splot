// splot.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include <iostream>
#include <sstream>

#ifdef ENABLE_NLS
#include <locale.h>
#include <libintl.h>
#endif
#include "gettext.h"

#include "ace/OS.h"
#include "ace/OS_main.h"
#include "ace/ACE.h"
#if defined(ACE_WIN32) || defined(ACE_WIN64)
#include "ace/Init_ACE.h"
#endif
#include "ace/Log_Msg.h"

#include "SDL.h"

#ifdef HAVE_CONFIG_H
#include "splot-config.h"
#endif

#include "defines.h"
#include "common.h"
#include "configuration.h"
#include "highscore.h"
#include "state.h"
#include "OpenGL_common.h"
#include "enemy_aircraft.h"

void
do_printVersion (const std::string& programName_in)
{
  // step1: program version
  //   std::cout << programName_in << ACE_TEXT(" : ") << VERSION << std::endl;
  std::cout << programName_in
            << ACE_TEXT_ALWAYS_CHAR (": ")
#ifdef HAVE_CONFIG_H
            << VERSION
#else
            << ACE_TEXT_ALWAYS_CHAR ("N/A")
#endif
            << std::endl;

  // step2: SDL version
  SDL_version sdl_version;
  ACE_OS::memset (&sdl_version, 0, sizeof (sdl_version));
  SDL_VERSION (&sdl_version);
  std::ostringstream version_number;
  version_number << sdl_version.major;
  version_number << ACE_TEXT_ALWAYS_CHAR (".");
  version_number << sdl_version.minor;
  version_number << ACE_TEXT_ALWAYS_CHAR (".");
  version_number << sdl_version.patch;
  std::cout << ACE_TEXT ("SDL (compiled): ")
            << version_number.str ()
            << std::endl;
  const SDL_version* sdl_version_p = SDL_Linked_Version ();
  if (!sdl_version_p)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to SDL_Linked_Version (): \"%s\", aborting\n"),
                ACE_TEXT (SDL_GetError ())));

    return;
  } // end IF
  version_number.str ("");
  version_number << sdl_version_p->major;
  version_number << ACE_TEXT_ALWAYS_CHAR (".");
  version_number << sdl_version_p->minor;
  version_number << ACE_TEXT_ALWAYS_CHAR (".");
  version_number << sdl_version_p->patch;
  std::cout << ACE_TEXT ("SDL (linked): ")
            << version_number.str ()
            << std::endl;

  // step3: ACE version
  // *NOTE*: cannot use ACE_VERSION, as it doesn't contain the (potential) beta
  // version number (this is needed, as the library soname is compared to this
  // string)
  version_number.str ("");
  version_number << ACE::major_version ();
  version_number << ACE_TEXT_ALWAYS_CHAR (".");
  version_number << ACE::minor_version ();
  version_number << ACE_TEXT_ALWAYS_CHAR (".");
  version_number << ACE::beta_version ();
  std::cout << ACE_TEXT ("ACE: ")
    //             << ACE_VERSION
            << version_number.str ()
            << std::endl;
}

#if defined (ACE_WIN32) || defined (ACE_WIN64)
#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */
int
SDL_main (int argc_in,
          char** argv_in)
#else
int
ACE_TMAIN (int argc_in,
           ACE_TCHAR** argv_in)
#endif
{
  // step0: init ACE
  // *PORTABILITY*: on Windows, ACE needs initialization...
#if defined (ACE_WIN32) || defined (ACE_WIN64)
  if (ACE::init () == -1)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to ACE::init(): \"%m\", aborting\n")));

    return EXIT_FAILURE;
  } // end IF
#endif

  // step1: process commandline options (if any)
  Mode_t mode = MODE_INVALID;
  if (!SPLOT_CONFIGURATION_SINGLETON::instance ()->init (argc_in,
                                                         argv_in,
                                                         mode))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT("failed to initialize configuration, aborting\n")));

    // clean up
    // *PORTABILITY*: on Windows, ACE needs finalization...
#if defined (ACE_WIN32) || defined (ACE_WIN64)
    if (ACE::fini () == -1)
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to ACE::fini(): \"%m\", continuing\n")));
#endif

    return EXIT_FAILURE;
  } // end IF
  if (mode == MODE_SHOW_VERSION)
  {
    do_printVersion (ACE::basename (argv_in[0]));

    // *PORTABILITY*: on Windows, ACE needs finalization...
#if defined (ACE_WIN32) || defined (ACE_WIN64)
    if (ACE::fini () == -1)
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to ACE::fini(): \"%m\", continuing\n")));
#endif

    return EXIT_SUCCESS;
  } // end IF

  // step2: initialize logging and/or tracing
  std::string log_file;
  const Configuration_t& configuration =
    SPLOT_CONFIGURATION_SINGLETON::instance ()->get ();
  if (configuration.debug)
  {
    log_file = getHomeDirectory ();
    log_file += ACE_DIRECTORY_SEPARATOR_STR;
    log_file += ACE_TEXT_ALWAYS_CHAR (CONFIG_LOG_FILE);
  } // end IF
  if (!initLogging (ACE_TEXT_ALWAYS_CHAR (ACE::basename (argv_in[0])),        // program name
                    log_file,                                                 // logfile
                    false,                                                    // log to syslog ?
                    false,                                                    // trace messages ?
                    SPLOT_CONFIGURATION_SINGLETON::instance() ->get ().debug, // debug messages ?
                    NULL))                                                    // logger
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to initLogging(), aborting\n")));

    // *PORTABILITY*: on Windows, need to fini ACE...
#if defined (ACE_WIN32) || defined (ACE_WIN64)
    if (ACE::fini () == -1)
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to ACE::fini(): \"%m\", continuing\n")));
#endif

    return EXIT_FAILURE;
  } // end IF

#ifdef ENABLE_NLS
#ifdef HAVE_LOCALE_H
  setlocale (LC_ALL, "");
#endif
  bindtextdomain (PACKAGE, LOCALEDIR);
  bind_textdomain_codeset (PACKAGE, "UTF-8");
  textdomain (PACKAGE);
#endif

#ifdef __linux__
  // by default, disable SDL's use of DGA mouse. If SDL_VIDEO_X11_DGAMOUSE is
  // set however, use default value.
  ACE_OS::setenv ("SDL_VIDEO_X11_DGAMOUSE", "0", false);
#endif

  if (configuration.debug)
    if (ACE_OS::atexit (Splot_EnemyAircraft::printNumAllocated,
                        NULL))
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to ACE_OS::atexit (): \"%m\", continuing\n")));

  // step3: initialize (game-)state
  ACE_OS::srand ((u_int)ACE_OS::time (NULL));
  GameToolkitType_t toolkit = STATE_DEFAULT_TOOLKIT;
#if defined (USE_SDL)
  toolkit = TOOLKIT_SDL;
#elif defined (USE_GLUT)
  toolkit = TOOLKIT_GLUT;
#else
#error "USE_SDL or USE_GLUT must be defined"
#endif

  Splot_State::initialize ();
  Splot_State* splot_state = SPLOT_STATE_SINGLETON::instance ();
  ACE_ASSERT (splot_state);
  if (!splot_state->initialize (toolkit,
                                argc_in,
                                argv_in))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to Splot_State::initialize (%d), aborting\n"),
                toolkit));

    // *PORTABILITY*: on Windows, need to fini ACE...
#if defined (ACE_WIN32) || defined (ACE_WIN64)
    if (ACE::fini () == -1)
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to ACE::fini(): \"%m\", continuing\n")));
#endif

    return EXIT_FAILURE;
  } // end IF
  if (!Splot_OpenGLCommon::init ())
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to Splot_OpenGLCommon::init(), aborting\n")));

    // *PORTABILITY*: on Windows, need to fini ACE...
#if defined (ACE_WIN32) || defined (ACE_WIN64)
    if (ACE::fini () == -1)
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to ACE::fini(): \"%m\", continuing\n")));
#endif

    return EXIT_FAILURE;
  } // end IF

  //const State_t& state = SPLOT_STATE_SINGLETON::instance ()->get ();
  //if (configuration.debug)
  //  state.highscore->print (SPLOT_CONFIGURATION_SINGLETON::instance ()->intSkill ());

  // step4: run game
  splot_state->run ();

  // step5: clean up
  Splot_OpenGLCommon::fini ();

  // *PORTABILITY*: on Windows, must fini ACE...
#if defined (ACE_WIN32) || defined (ACE_WIN64)
  if (ACE::fini () == -1)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to ACE::fini(): \"%m\", aborting\n")));

    return EXIT_FAILURE;
  } // end IF
#endif

  return EXIT_SUCCESS;
} // end main
#if defined (ACE_WIN32) || defined (ACE_WIN64)
#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif
