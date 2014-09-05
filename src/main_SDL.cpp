#include "stdafx.h"

#include "main_SDL.h"

#ifdef USE_SDL

#include <string>

#ifdef USE_SDL_IMAGE
#include "SDL_image.h"
#endif

#if defined(HAVE_APPLE_OPENGL_FRAMEWORK) || (defined(HAVE_OPENGL_GL_H) && defined(HAVE_OPENGL_GLU_H))
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#else
#include "GL/gl.h"
#include "GL/glu.h"
#endif

#include "ace/OS.h"
#include "ace/Log_Msg.h"

#include "common.h"
#include "configuration.h"
#include "state.h"
#include "OpenGL_common.h"

Splot_MainSDL::Splot_MainSDL (int argc_in, char** argv_in)
 : inherited (argc_in, argv_in)
#if SDL_VERSION_ATLEAST (2,0,0)
 , window_(NULL)
 //, context_ ()
#endif
 , joystick_ (NULL)
 , fire_ (0)
 , mouseToggle_ (true)
 , joystickToggle_ (true)
 //, last_ ()
 //, joy_ ()
 //, jNow_ ()
 , adjCount_ (0)
 //, key_speed_ ()
{
  State_t& state = SPLOT_STATE_SINGLETON::instance ()->get ();
  mouseToggle_ = state.mouse_active;
  joystickToggle_ = state.joy_active;

  ACE_OS::memset (&last_, 0, sizeof (last_));
  ACE_OS::memset (&mid_, 0, sizeof (mid_));
  ACE_OS::memset (&joy_, 0, sizeof (joy_));
  ACE_OS::memset (&jNow_, 0, sizeof (jNow_));
  ACE_OS::memset (&key_speed_, 0, sizeof (key_speed_));

  Uint32 SDL_init_options = SDL_INIT_VIDEO;
#ifdef WITH_SDL_JOYSTICK
  SDL_init_options |= SDL_INIT_JOYSTICK;
#endif // WITH_SDL_JOYSTICK
  const Configuration_t& configuration =
    SPLOT_CONFIGURATION_SINGLETON::instance ()->get ();
  if (!configuration.debug)
    SDL_init_options |= SDL_INIT_NOPARACHUTE;
#ifdef USE_SDLMIXER_AUDIO
  SDL_init_options |= SDL_INIT_AUDIO;
#endif
#ifdef WITH_SDL_CDROM
  SDL_init_options |= SDL_INIT_CDROM;
#endif // WITH_SDL_CDROM

  if (SDL_Init (SDL_init_options) < 0)
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to SDL_Init(%u): \"%s\", continuing\n"),
                SDL_init_options,
                ACE_TEXT (SDL_GetError ())));
  else if (configuration.debug)
    ACE_DEBUG ((LM_INFO,
                ACE_TEXT ("initialized SDL\n")));

  //// input repeat/delay
  //int repeat_delay, repeat_interval;
  ////SDL_GetKeyRepeat (&repeat_delay, &repeat_interval);
  //repeat_delay = SDL_DEFAULT_REPEAT_INTERVAL;
  //repeat_interval = SDL_DEFAULT_REPEAT_INTERVAL;
  //if (SDL_EnableKeyRepeat (repeat_delay, repeat_interval))
  //  ACE_DEBUG ((LM_ERROR,
  //              ACE_TEXT ("failed to SDL_EnableKeyRepeat(%d,%d): \"%s\", continuing\n"),
  //              repeat_delay, repeat_interval,
  //              ACE_TEXT (SDL_GetError ())));

#ifdef WITH_SDL_JOYSTICK
  int nj = SDL_NumJoysticks ();
  if (nj > 0)
  {
    if (configuration.debug)
      ACE_DEBUG ((LM_INFO,
                  ACE_TEXT ("found %d joystick(s)\n"),
                  nj));

    joystick_ = SDL_JoystickOpen (0);
    if (!joystick_)
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to SDL_JoystickOpen(0): \"%s\", continuing\n"),
                  ACE_TEXT (SDL_GetError ())));
    else
      if (SDL_JoystickEventState (SDL_ENABLE) < 0)
        ACE_DEBUG ((LM_ERROR,
                    ACE_TEXT ("failed to SDL_JoystickEventState(SDL_ENABLE): \"%s\", continuing\n"),
                    ACE_TEXT (SDL_GetError ())));
  } // end IF
  else if (nj == 0)
  {
    if (configuration.debug)
      ACE_DEBUG ((LM_INFO,
                  ACE_TEXT ("no joysticks found\n")));
  } // end ELSE
  else
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to SDL_NumJoysticks(): \"%s\", continuing\n"),
                ACE_TEXT (SDL_GetError ())));
#endif // WITH_SDL_JOYSTICK

  if (!setVideoMode ())
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to setVideoMode(), continuing\n")));

  if (configuration.debug)
  {
    ACE_DEBUG ((LM_INFO,
                ACE_TEXT ("-OpenGL-----------------------------------------------------\n")
                ACE_TEXT ("Vendor     : %s\n")
                ACE_TEXT ("Renderer   : %s\n")
                ACE_TEXT ("Version    : %s\n"),
                glGetString (GL_VENDOR), glGetString (GL_RENDERER), glGetString (GL_VERSION)));
    //printExtensions (stderr,  (const char*)glGetString (GL_EXTENSIONS));
    ACE_DEBUG ((LM_INFO,
                ACE_TEXT ("------------------------------------------------------------\n")));
  } // end IF

  //-- Set the window manager icon
  std::string path_base = ACE_TEXT_ALWAYS_CHAR (SPLOT_IMAGE_DATA_DIR);
  path_base += ACE_DIRECTORY_SEPARATOR_STR;
  std::string filename = path_base + ACE_TEXT_ALWAYS_CHAR (SPLOT_WINDOW_ICON);
  SDL_Surface* window_icon = NULL;
#ifdef USE_SDL_IMAGE
  window_icon = IMG_Load (dataLoc (filename).c_str ());
  if (!window_icon)
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to IMG_Load(\"%s\"): \"%s\", continuing\n"),
                ACE_TEXT (dataLoc (filename).c_str ()),
                ACE_TEXT (IMG_GetError ())));
#endif // USE_SDL_IMAGE
  if (window_icon)
  {
 #if SDL_VERSION_ATLEAST (2,0,0)
    SDL_SetWindowIcon (window, window_icon);
#else
    SDL_WM_SetIcon (window_icon, NULL);
#endif // SDL_VERSION_ATLEAST (2,0,0)
    SDL_FreeSurface (window_icon);
  } // end IF

#if !(SDL_VERSION_ATLEAST (2,0,0))
  //-- Set the window manager title bar
  SDL_WM_SetCaption (ACE_TEXT_ALWAYS_CHAR (SPLOT_WINDOW_TITLE),
                     ACE_TEXT_ALWAYS_CHAR (SPLOT_ICON_CAPTION));
#endif // !(SDL_VERSION_ATLEAST (2,0,0))
}

Splot_MainSDL::~Splot_MainSDL ()
{
  SDL_Quit ();
}

bool
Splot_MainSDL::run ()
{
  //-- Create game
  SPLOT_STATE_SINGLETON::instance ()->initGame ();

  State_t& state = SPLOT_STATE_SINGLETON::instance ()->get ();
  const Configuration_t& configuration =
    SPLOT_CONFIGURATION_SINGLETON::instance ()->get ();
  float target_adj = 1.0;
  Uint32 now_time = 0;
  Uint32 last_time = 0;
	//key_speed_x  = key_speed_y = 0.0;
  bool done = false;
  SDL_Event event;
  //int frames = 0;

  //-- enter main loop...
  while (!done) 
  {
    //-- Draw scene and refresh screen
    Splot_OpenGLCommon::draw ();
#if SDL_VERSION_ATLEAST (2,0,0)
    SDL_GL_SwapWindow(window);
#else
    SDL_GL_SwapBuffers ();
#endif // SDL_VERSION_ATLEAST (2,0,0)
    if (configuration.debug)
      checkErrors ();

    //-- Delay
    SDL_Delay (32-(int)(24.0*state.game_speed));
    //		SDL_Delay( 8 );
    //		SDL_Delay( 16 );
    //		SDL_Delay( 32 );
    //		SDL_Delay( 66 );

    //		//-- cheezy, partially functional record mechanism
    //		bool write = false;
    //		SDL_Event *fileEvent;
    //		if( !write && game->gameMode == Global::Game)
    //		{
    //			while( (fileEvent = getEvent(game->eventFile)) ) 
    //				done = this->process(fileEvent);
    //		}

    // Check whether there are pending events
    while (SDL_PollEvent (&event)) 
    {
    //			if(write)
    //				saveEvent(&event);
      done = this->process (&event);
      if (!done) break;
    } // end WHILE
    //this->joystickMove ();
    this->keyMove ();
    //++frames;

    state.frame++;
    if (!(state.game_frame%10))
    {
      now_time = SDL_GetTicks ();
      if (last_time)
        state.fps = (10.0F/(now_time-last_time))*1000.0F;
      last_time = now_time;

      if (state.game_mode == GAMEMODE_MENU)
        continue;

      if (state.game_frame < 400)
      {
        if (state.fps < 48.0 &&
            state.game_speed < 1.0)
        {
          state.game_speed += 0.02F;
          if (configuration.debug)
            ACE_DEBUG ((LM_INFO,
                        ACE_TEXT ("speed calibration: fps: %3.2f, speed: %f ...\n"),
                        state.fps, state.game_speed));
        } // end IF
        else if (state.game_frame > 20)
        {
          float tmp = 50.0F/state.fps;
          tmp = 0.8F*target_adj + 0.2F*tmp;
          target_adj = floor (100.0F*(tmp + 0.005F)) / 100.0F;
//          if (configuration.debug)
//            ACE_DEBUG ((LM_INFO,
//                        ACE_TEXT ("speed calibration: fps: %3.2f, adj: %f, tmp: %f ...\n"),
//                        state.fps, target_adj, tmp));
        } // end IF
      } // end IF
      else if (configuration.auto_speed &&
               (state.fps > 30.0 && state.fps < 100.0))  // discount any wacky fps from pausing
      {
        //game->speedAdj = targetAdj;
        // Everything was originally based on 50fps - attempt to adjust
        // if outside of a reasonable range
        float tmp = 50.0F/state.fps;
        if (fabs (target_adj-tmp) > 0.1)
        {
          adjCount_++;
          state.speed_adj = tmp;
          if (configuration.debug)
            ACE_DEBUG ((LM_INFO,
                        ACE_TEXT ("speed calibration: fps: %3.2f, adj: %f, state.speed_adj: %f ...\n"),
                        state.fps, target_adj, state.speed_adj));
        } // end IF
        else
          state.speed_adj = target_adj;
      }
      else
        state.speed_adj = target_adj;

    //				if( !(frames%500) )
    //					if( config->debug() ) fprintf(stdout, _("fps = %g speedAdj = %g\n"), game->fps, game->speedAdj);
    } // end IF
  } // end WHILE

  //-- Delete game objects
  SPLOT_STATE_SINGLETON::instance ()->finiGame ();

  if (adjCount_ > 20)
    ACE_DEBUG ((LM_DEBUG,
                ACE_TEXT ("%d speed adjustments required.\n")
                ACE_TEXT ("NOTE: The game was not able to maintain a steady 50 frames per\n")
                ACE_TEXT ("      second. You should consider reducing your screen resolution\n")
                ACE_TEXT ("      or lowering the graphics detail setting.\n")
                ACE_TEXT ("      -OR-\n")
                ACE_TEXT ("      make sure that you aren't running any system monitoring\n")
                ACE_TEXT ("      tools (like \'top\', \'xosview\', etc.) These kinds of tools\n")
                ACE_TEXT ("      can make it difficult to maintain a steady frame rate.\n"),
                adjCount_));

  //-- Destroy our GL context, etc.
  if (configuration.debug)
    ACE_DEBUG ((LM_INFO,
                ACE_TEXT ("left event dispatching loop...")));

  return true;
}

bool
Splot_MainSDL::checkErrors ()
{
  bool result = false;

  GLenum OpenGL_error = GL_NO_ERROR;
  const char* SDL_error = NULL;

  //-- Check for GL errors
  OpenGL_error = glGetError ();
  if (OpenGL_error != GL_NO_ERROR)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("OpenGL error: \"%s\"\n"),
                ACE_TEXT (gluErrorString (OpenGL_error))));
    //while ((OpenGL_error = glGetError ()) != GL_NO_ERROR)
    //  ACE_DEBUG ((LM_ERROR,
    //              ACE_TEXT ("OpenGL error: \"%s\"\n"),
    //              gluErrorString (OpenGL_error)));

    result = true;
  } // end IF

  //-- Check for SDL errors
  SDL_error = SDL_GetError ();
  if (SDL_error[0] != '\0')
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("SDL error: \"%s\"\n"),
                ACE_TEXT (SDL_error)));

    SDL_ClearError ();
    result = true;
  } // end IF

  return result;
}

bool
Splot_MainSDL::setVideoMode ()
{
  //Global	*game = Global::getInstance();
  //Config	*config = Config::instance();
  //int w;
  //int h;
  Uint32 video_flags = 0;
#if !(SDL_VERSION_ATLEAST (2,0,0))
  SDL_Surface* OpenGL_surface = NULL;
#endif // !(SDL_VERSION_ATLEAST (2,0,0))

  //-- Set the flags we want to use for setting the video mode
#if SDL_VERSION_ATLEAST (2,0,0)
#define SDL_OPENGL SDL_WINDOW_OPENGL
#define SDL_FULLSCREEN SDL_WINDOW_FULLSCREEN
#endif // SDL_VERSION_ATLEAST (2,0,0)
  video_flags = SDL_OPENGL;
  const Configuration_t& configuration =
    SPLOT_CONFIGURATION_SINGLETON::instance ()->get ();
  if (configuration.full_screen)
    video_flags |= SDL_FULLSCREEN;

  //w = config->screenW();
  //h = config->screenH();

  int rs, gs, bs, ds;
#if !(SDL_VERSION_ATLEAST (2,0,0))
  int bpp;
#endif // !(SDL_VERSION_ATLEAST (2,0,0))
  if (configuration.true_color)
  {
    //-- 24 bit color
#if !(SDL_VERSION_ATLEAST (2,0,0))
    bpp = 24;
#endif // !(SDL_VERSION_ATLEAST (2,0,0))
    rs = gs = bs = 8;
    ds = 16;
  } // end IF
  else
  {
    //-- 16 bit color
#if !(SDL_VERSION_ATLEAST (2,0,0))
    bpp = 16;
#endif // !(SDL_VERSION_ATLEAST (2,0,0))
    rs = bs = 5;
    gs = 6;
    ds = 16;
  } // end IF

  //-- Initialize display
  if (SDL_GL_SetAttribute (SDL_GL_RED_SIZE,     rs))
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to SDL_GL_SetAttribute(SDL_GL_RED_SIZE, %d): \"%s\", continuing\n"),
                rs,
                ACE_TEXT (SDL_GetError ())));
  if (SDL_GL_SetAttribute (SDL_GL_GREEN_SIZE,   gs))
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, %d): \"%s\", continuing\n"),
                gs,
                ACE_TEXT (SDL_GetError ())));
  if (SDL_GL_SetAttribute (SDL_GL_BLUE_SIZE, bs))
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, %d): \"%s\", continuing\n"),
                bs,
                ACE_TEXT (SDL_GetError ())));
  if (SDL_GL_SetAttribute (SDL_GL_DEPTH_SIZE, ds))
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, %d): \"%s\", continuing\n"),
                ds,
                ACE_TEXT (SDL_GetError ())));
  if (SDL_GL_SetAttribute (SDL_GL_DOUBLEBUFFER, 1))
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1): \"%s\", continuing\n"),
                ACE_TEXT (SDL_GetError ())));

#if SDL_VERSION_ATLEAST (2,0,0)
  window_ = SDL_CreateWindow (ACE_TEXT_ALWAYS_CHAR (SPLOT_WINDOW_TITLE),
                              SDL_WINDOWPOS_CENTERED,
                              SDL_WINDOWPOS_CENTERED,
                              configuration.screen_width, configuration.screen_height,
                              video_flags);
  if (!window)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to SDL_CreateWindow(%d, %d): \"%s\", aborting\n"),
                configuration.screen_width, configuration.screen_height,
                ACE_TEXT (SDL_GetError ())));

    return false;
  } // end IF

  context = SDL_GL_CreateContext (window);
  if (!context)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to SDL_GL_CreateContext(): \"%s\", aborting\n"),
                ACE_TEXT (SDL_GetError ())));

    return false;
  } // end IF
#else
  OpenGL_surface =
    SDL_SetVideoMode (configuration.screen_width, configuration.screen_height,
                      bpp,
                      video_flags);
  if (!OpenGL_surface)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to SDL_SetVideoMode(%d,%d): \"%s\", aborting\n"),
                configuration.screen_width, configuration.screen_height,
                ACE_TEXT (SDL_GetError ())));

    return false;
  } // end IF
  else if (configuration.debug)
    ACE_DEBUG ((LM_INFO,
                ACE_TEXT ("video mode set\n")));
#endif // SDL_VERSION_ATLEAST (2,0,0)

  SDL_GL_GetAttribute (SDL_GL_RED_SIZE,   &rs);
  SDL_GL_GetAttribute (SDL_GL_GREEN_SIZE, &gs);
  SDL_GL_GetAttribute (SDL_GL_BLUE_SIZE,  &bs);
  SDL_GL_GetAttribute (SDL_GL_DEPTH_SIZE, &ds);
#if !(SDL_VERSION_ATLEAST (2,0,0))
  if (configuration.debug)
    ACE_DEBUG ((LM_INFO,
                ACE_TEXT ("(bpp=%d RGB=%d%d%d depth=%d)\n"),
                OpenGL_surface->format->BitsPerPixel, rs, gs, bs, ds));
#endif

  bool result = Splot_OpenGLCommon::init ();
  if (!result)
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to Splot_OpenGLCommon::init(), aborting\n")));

  return result;
}

#endif // USE_SDL
