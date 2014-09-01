#include "stdafx.h"

#include "main_SDL.h"

//#ifdef HAVE_CONFIG_H
//#include <chromium-bsu-config.h>
//#endif
//
//#include "gettext.h"
//
//#include "Splot_MainSDL.h"

#ifdef USE_SDL

//#include <cstdlib>
//#include <cstdio>
//#include <cstring>
//

#include "ace/Log_Msg.h"

//
#include "state.h"
#include "configuration.h"
#include "player_aircraft.h"
#include "menu.h"
#include "audio.h"
//#include "Ground.h"

#if SDL_VERSION_ATLEAST (2,0,0)
#define SDLK_KP1 SDLK_KP_1
#define SDLK_KP2 SDLK_KP_2
#define SDLK_KP3 SDLK_KP_3
#define SDLK_KP4 SDLK_KP_4
#define SDLK_KP5 SDLK_KP_5
#define SDLK_KP6 SDLK_KP_6
#define SDLK_KP7 SDLK_KP_7
#define SDLK_KP8 SDLK_KP_8
#define SDLK_KP9 SDLK_KP_9
#endif

bool
Splot_MainSDL::process (SDL_Event* event_in)
{
  State_t& state = SPLOT_STATE_SINGLETON::instance ()->get ();
#if 0
	static int cnt = 0;
	cnt++;
	switch( event->type ) 
	{
	    case SDL_ACTIVEEVENT:		fprintf(stderr, _("%-5d SDL_ACTIVEEVENT  \n")		, cnt); break;
	    case SDL_KEYDOWN:			fprintf(stderr, _("%-5d SDL_KEYDOWN  \n")			, cnt); break;
	    case SDL_KEYUP: 			fprintf(stderr, _("%-5d SDL_KEYUP  \n")			, cnt); break;
	    case SDL_MOUSEMOTION:		fprintf(stderr, _("%-5d SDL_MOUSEMOTION  \n")		, cnt); break;
	    case SDL_MOUSEBUTTONDOWN:	fprintf(stderr, _("%-5d SDL_MOUSEBUTTONDOWN  \n")	, cnt); break;
	    case SDL_MOUSEBUTTONUP: 	fprintf(stderr, _("%-5d SDL_MOUSEBUTTONUP  \n")	, cnt); break;
		case SDL_JOYBUTTONDOWN: 	fprintf(stderr, _("%-5d SDL_JOYBUTTONDOWN  \n")	, cnt); break;
		case SDL_JOYBUTTONUP:		fprintf(stderr, _("%-5d SDL_JOYBUTTONUP  \n")		, cnt); break;
		default:
			break;
	}
#endif
  bool shown, mouse, input, gain = false;
  switch (event_in->type) 
  {
#if SDL_VERSION_ATLEAST (2,0,0)
    case SDL_WINDOWEVENT:
      switch (event->window.event) {
        case SDL_WINDOWEVENT_RESTORED: shown = true; gain = true; break;
        case SDL_WINDOWEVENT_MINIMIZED: shown = true; gain = false; break;
        case SDL_WINDOWEVENT_ENTER: mouse = true; gain = true; break;
        case SDL_WINDOWEVENT_LEAVE: mouse = true; gain = false; break;
        case SDL_WINDOWEVENT_FOCUS_GAINED: input = true; gain = true; break;
        case SDL_WINDOWEVENT_FOCUS_LOST: input = true; gain = false; break;
      }
      activation(shown, mouse, input, gain);
      break;
#else
    case SDL_ACTIVEEVENT:
      shown = event_in->active.state & SDL_APPACTIVE ? true : false;
      mouse = event_in->active.state & SDL_APPMOUSEFOCUS ? true : false;
      input = event_in->active.state & SDL_APPINPUTFOCUS ? true : false;
      gain = event_in->active.gain ? true : false;
      activation (shown, mouse, input, gain);
      break;
#endif
    case SDL_KEYDOWN:
      keyDown (event_in);
      break;
    case SDL_KEYUP:
      keyUp (event_in);
      break;
    case SDL_MOUSEMOTION:
      mouseMotion (event_in);
      break;
    case SDL_MOUSEBUTTONDOWN:
      mouseButtonDown (event_in);
      break;
    case SDL_MOUSEBUTTONUP:
      mouseButtonUp (event_in);
      break;
    case SDL_JOYAXISMOTION:
      joystickMotion (event_in);
      break;
    case SDL_JOYBUTTONDOWN:
      joystickButtonDown (event_in);
      break;
    case SDL_JOYBUTTONUP:
      joystickButtonUp (event_in);
      break;
    case SDL_QUIT:
      state.game_quit = true;
      break;
    default:
      break;
  } // end SWITCH

  return state.game_quit;
}

//void
//Splot_MainSDL::saveEvent (SDL_Event* event_in)
//{
//  Global	*game = Global::getInstance();
//
//  if(game->eventFile && game->gameMode == Global::Game)
//  {
//  #if SDL_VERSION_ATLEAST(2,0,0)
//	  SDL_WindowEvent 		*evW = 0;
//  #else
//	  SDL_ActiveEvent 		*evA = 0;
//  #endif
//	  SDL_KeyboardEvent 		*evK = 0;
//	  SDL_MouseMotionEvent 	*evM = 0;
//	  SDL_MouseButtonEvent 	*evB = 0;
//	  switch(event->type) 
//	  {
//  //	    	case SDL_ACTIVEEVENT:
//  //				evA = (SDL_ActiveEvent*)event;
//  //				fprintf(game->eventFile, "%9d :%s: %5d %5d %5d %5d %5d %5d\n", game->gameFrame, "A",
//  //					(int)evA->type, (int)evA->gain, (int)evA->state, 0, 0, 0);
//  //				break;
//  //	    	case SDL_KEYDOWN:
//  //	    	case SDL_KEYUP:
//  //				evK = (SDL_KeyboardEvent*)event;
//  //				fprintf(game->eventFile, "%9d :%s: %5d %5d %5d %5d %5d %5d\n", game->gameFrame, "K",
//  //					(int)evK->type, (int)evK->state, 
//  //					(int)evK->keysym.scancode, (int)evK->keysym.sym, (int)evK->keysym.mod, (int)evK->keysym.unicode);
//  //				break;
//  //	    	case SDL_MOUSEMOTION:
//  //				evM = (SDL_MouseMotionEvent*)event;
//  //				fprintf(game->eventFile, "%9d :%s: %5d %5d %5d %5d %5d %5d\n", game->gameFrame, "M",
//  //					(int)evM->type, (int)evM->state, (int)evM->x, (int)evM->y, (int)evM->xrel, (int)evM->yrel);
//  //				break;
//  //	    	case SDL_MOUSEBUTTONDOWN:
//  //	    	case SDL_MOUSEBUTTONUP:
//  //				evB = (SDL_MouseButtonEvent*)event;
//  //				fprintf(game->eventFile, "%9d :%s: %5d %5d %5d %5d %5d %5d\n", game->gameFrame, "B",
//  //					(int)evB->type, (int)evB->button, (int)evB->state, (int)evB->x, (int)evB->y, 0);
//  //				break;
//  //			default:
//  //				break;
//  #if SDL_VERSION_ATLEAST(2,0,0)
//	  case SDL_WINDOWEVENT:
//			  evW = (SDL_WindowEvent*)event;
//			  fprintf(game->eventFile, "%9d :%s: %5d %5d %5d %5d %5d %5d\n", game->gameFrame, "W",
//				  evW->type, evW->windowID, evW->event, evW->data1, evW->data2, 0);
//			  break;
//	      case SDL_KEYDOWN:
//	      case SDL_KEYUP:
//			  evK = (SDL_KeyboardEvent*)event;
//			  fprintf(game->eventFile, "%9d :%s: %5d %5d %5d %5d %5d\n", game->gameFrame, "K",
//				  evK->type, evK->state, 
//				  evK->keysym.scancode, evK->keysym.sym, evK->keysym.mod);
//			  break;
//  #else
//	      case SDL_ACTIVEEVENT:
//			  evA = (SDL_ActiveEvent*)event;
//			  fprintf(game->eventFile, "%9d :%s: %5d %5d %5d %5d %5d %5d\n", game->gameFrame, "A",
//				  evA->type, evA->gain, evA->state, 0, 0, 0);
//			  break;
//	      case SDL_KEYDOWN:
//	      case SDL_KEYUP:
//			  evK = (SDL_KeyboardEvent*)event;
//			  fprintf(game->eventFile, "%9d :%s: %5d %5d %5d %5d %5d %5d\n", game->gameFrame, "K",
//				  evK->type, evK->state, 
//				  evK->keysym.scancode, evK->keysym.sym, evK->keysym.mod, evK->keysym.unicode);
//			  break;
//  #endif
//	      case SDL_MOUSEMOTION:
//			  evM = (SDL_MouseMotionEvent*)event;
//			  fprintf(game->eventFile, "%9d :%s: %5d %5d %5d %5d %5d %5d\n", game->gameFrame, "M",
//				  evM->type, evM->state, evM->x, evM->y, evM->xrel, evM->yrel);
//			  break;
//	      case SDL_MOUSEBUTTONDOWN:
//	      case SDL_MOUSEBUTTONUP:
//			  evB = (SDL_MouseButtonEvent*)event;
//			  fprintf(game->eventFile, "%9d :%s: %5d %5d %5d %5d %5d %5d\n", game->gameFrame, "B",
//				  evB->type, evB->button, evB->state, evB->x, evB->y, 0);
//			  break;
//		  default:
//			  break;
//	  }
//  }
//}

//SDL_Event*
//Splot_MainSDL::getEvent(FILE *infile)
//{
//	Global	*game = Global::getInstance();
//	static int count = 0;
//	static	SDL_Event ev;
//	static	char buffer[256] = { "-1 :K: 2 0 0 0 0 0"};
//	SDL_Event	*retVal = 0;
//	
//	if(infile)
//	{
//#if SDL_VERSION_ATLEAST(2,0,0)
//		SDL_WindowEvent 		*evW = (SDL_WindowEvent*)&ev;
//#else
//		SDL_ActiveEvent 		*evA = (SDL_ActiveEvent*)&ev;
//#endif
//		SDL_KeyboardEvent 		*evK = (SDL_KeyboardEvent*)&ev;
//		SDL_MouseMotionEvent 	*evM = (SDL_MouseMotionEvent*)&ev;
//		SDL_MouseButtonEvent 	*evB = (SDL_MouseButtonEvent*)&ev;
//		int		frame;
//		int		a,b,c,d,e,f;
//		char	t;
//		
//		sscanf(buffer, "%d :%c: %d %d %d %d %d %d", &frame, &t, &a, &b, &c, &d, &e, &f);
//		if(frame == game->gameFrame)
//		{
//			switch(t)
//			{
//#if SDL_VERSION_ATLEAST(2,0,0)
//				case 'W':
//					evW->type	= (Uint32)a;
//					evW->windowID	= (Uint32)b;
//					evW->event	= (Uint8)c;
//					evW->data1	= d;
//					evW->data2	= e;
//					retVal = &ev;
//					break;
//#else
//				case 'A':
//					evA->type	= (Uint8)a;
//					evA->gain	= (Uint8)b;
//					evA->state	= (Uint8)c;
//					retVal = &ev;
//					break;
//#endif
//				case 'K':
//					evK->type	= (Uint8)a;
//					evK->state	= (Uint8)b;
//#if SDL_VERSION_ATLEAST(2,0,0)
//					evK->keysym.scancode = (SDL_Scancode)c;
//					evK->keysym.sym		 = (SDL_Keycode)d;
//					evK->keysym.mod		 = (SDL_Keymod)e;
//#else
//					evK->keysym.scancode = (Uint8)c;
//					evK->keysym.sym		 = (SDLKey)d;
//					evK->keysym.mod		 = (SDLMod)e;
//					evK->keysym.unicode	 = (Uint16)f;
//#endif
//					retVal = &ev;	
//					break;
//				case 'M':
//					evM->type	= (Uint8)a;
//					evM->state	= (Uint8)b;
//					evM->x		= (Uint16)c;
//					evM->y		= (Uint16)d;
//					evM->xrel	= (Sint16)e;
//					evM->yrel	= (Sint16)f;
//					retVal = &ev;
//					break;
//				case 'B':
//					evB->type	= (Uint8)a;
//					evB->button	= (Uint8)b;
//					evB->state	= (Uint8)c;
//					evB->x		= (Uint16)d;
//					evB->y		= (Uint16)e;
//					retVal = &ev;
//					break;
//				default:
//					fprintf(stderr, _("Splot_MainSDL::getEvent HUH?\n"));
//					break;
//			}
//			count++;
//		}
//		bool ok = true;
//		if(frame <= game->gameFrame)
//			ok = fgets(buffer, 256, infile);
//		if(!ok)
//		{
//			fprintf(stderr, _("buffer = \"%s\n\""), buffer);
//			exit(1);
//		}
//	}
//	return retVal;
//}

void
Splot_MainSDL::activation (bool shown_in,
                           bool mouse_in,
                           bool input_in,
                           bool gain_in)
{
  //Global *game = Global::getInstance();
  const Configuration_t& configuration =
    SPLOT_CONFIGURATION_SINGLETON::instance ()->get ();
  if (configuration.debug)
    ACE_DEBUG ((LM_INFO,
                ACE_TEXT ("app %s\n"),
                (gain_in ? ACE_TEXT ("gained") : ACE_TEXT ("lost"))));
  State_t& state = SPLOT_STATE_SINGLETON::instance ()->get ();
  bool grab_mouse =((state.game_mode == GAMEMODE_GAME) &&
                    !state.game_pause                  &&
                    gain_in);
  if (shown_in)
  {
    grabMouse (grab_mouse, grab_mouse);
    if (configuration.debug)
      ACE_DEBUG ((LM_INFO,
                  ACE_TEXT ("app active\n")));
  } // end IF
  else if (mouse_in)
  {
    if (configuration.debug)
      ACE_DEBUG ((LM_INFO,
                  ACE_TEXT ("mouse active\n")));
    Uint8 button_state = SDL_GetMouseState (&last_[0], &last_[1]);
    ACE_UNUSED_ARG (button_state);
  } // end IF
  else if (input_in)
  {
    grabMouse (grab_mouse, grab_mouse);
    ACE_DEBUG ((LM_INFO,
                ACE_TEXT ("input active\n")));
  } // end IF
  if (configuration.debug)
    ACE_DEBUG ((LM_INFO,
                ACE_TEXT ("focus\n")));
}

void
Splot_MainSDL::keyUp (SDL_Event* event_in)
{
  State_t& state = SPLOT_STATE_SINGLETON::instance ()->get ();
  switch (event_in->key.keysym.sym)
  {
    case SDLK_SPACE:
      state.player->fireGun (false);
      break;
    default:
      break;
  } // end SWITCH
}

void
Splot_MainSDL::keyDown (SDL_Event* event_in)
{
  State_t& state = SPLOT_STATE_SINGLETON::instance ()->get ();
  switch (event_in->key.keysym.sym)
  {
    case SDL_QUIT:
    case SDLK_q:
      state.game_quit = true;
      break;
    case SDLK_g:
      grabMouse (!mouseToggle_);
      break;
    case SDLK_0:
      state.player->useItem (0);
      break;
    case SDLK_9:
      state.player->useItem (1);
      break;
    case SDLK_ESCAPE:
      if (state.game_mode == GAMEMODE_MENU)
      {
        state.game_mode = GAMEMODE_GAME;
        state.audio->setMusicMode (SOUND_MUSIC_GAME);
        grabMouse (!state.game_pause);
      } // end IF
      else
      {
        if (state.game_mode != GAMEMODE_GAME)
          SPLOT_STATE_SINGLETON::instance ()->newGame ();
        state.game_mode = GAMEMODE_MENU;
        state.menu->startMenu ();
        state.audio->setMusicMode (SOUND_MUSIC_MENU);
        grabMouse (false);
      } // end ELSE
      break;
    default:
      switch (state.game_mode)
      {
        case GAMEMODE_GAME:
          keyDownGame (event_in);
          break;
        case GAMEMODE_LEVEL_COMPLETE:
          SPLOT_STATE_SINGLETON::instance ()->gotoNextLevel ();
          state.game_mode = GAMEMODE_GAME;
          state.audio->setMusicMode (SOUND_MUSIC_GAME);
          break;
        case GAMEMODE_GAME_OVER:
          state.game_mode = GAMEMODE_GAME;
          SPLOT_STATE_SINGLETON::instance ()->newGame ();
          state.toolkit->grabMouse (true);
          state.audio->setMusicMode (SOUND_MUSIC_GAME);
          break;
        default:
          Key_t key;
          switch (event_in->key.keysym.sym)
          {
            case SDLK_KP9:
            case SDLK_KP7:
            case SDLK_KP8:
            case SDLK_UP:
              key = KEY_UP; break;
            case SDLK_KP1:
            case SDLK_KP3:
            case SDLK_KP2:
            case SDLK_DOWN:
              key = KEY_DOWN; break;
            case SDLK_KP4:
            case SDLK_LEFT:
              key = KEY_LEFT; break;
            case SDLK_KP6:
            case SDLK_RIGHT:
              key = KEY_RIGHT; break;
            case SDLK_KP_ENTER:
            case SDLK_RETURN:
              key = KEY_ENTER; break;
            case SDLK_KP5:
            default:
              return;
          } // end SWITCH
          state.menu->keyHit (key);
          break;
      } // end SWITCH
      break;
  } // end SWITCH
}

void
Splot_MainSDL::keyDownGame (SDL_Event* event_in)
{
  State_t& state = SPLOT_STATE_SINGLETON::instance ()->get ();
  switch (event_in->key.keysym.sym)
  {
    case SDLK_KP_PLUS:
    case SDLK_QUOTE:
      state.player->nextItem ();
      break;
    case SDLK_KP_ENTER:
    case SDLK_RETURN:
      state.player->useItem ();
      break;
    case SDLK_PAUSE:
    case SDLK_p:
      grabMouse (state.game_pause);
      state.game_pause = !state.game_pause;
      state.audio->pauseMusic (state.game_pause);
      break;
    case SDLK_n:
      state.audio->nextMusicIndex ();
      break;
    case SDLK_KP7:
      key_speed_[0] -= 5.0;
      key_speed_[1] -= 5.0;
      break;
    case SDLK_KP9:
      key_speed_[0] += 5.0;
      key_speed_[1] -= 5.0;
      break;
    case SDLK_KP3:
      key_speed_[0] += 5.0;
      key_speed_[1] += 5.0;
      break;
    case SDLK_KP1:
      key_speed_[0] -= 5.0;
      key_speed_[1] += 5.0;
      break;
    case SDLK_KP4:
    case SDLK_LEFT:
      key_speed_[0] -= 5.0;
      break;
    case SDLK_KP6:
    case SDLK_RIGHT:
      key_speed_[0] += 5.0;
      break;
    case SDLK_KP8:
    case SDLK_UP:
      key_speed_[1] -= 5.0;
      break;
    case SDLK_KP2:
    case SDLK_DOWN:
      key_speed_[1] += 5.0;
      break;
    case SDLK_SPACE:
      state.player->fireGun (true);
      break;
    default:
      const Configuration_t& configuration =
        SPLOT_CONFIGURATION_SINGLETON::instance ()->get ();
      if (configuration.debug)
        ACE_DEBUG ((LM_INFO,
                    ACE_TEXT ("key '%s' pressed\n"),
                    ACE_TEXT (SDL_GetKeyName (event_in->key.keysym.sym))));
      ACE_DEBUG ((LM_DEBUG,
                  ACE_TEXT ("state.game_frame = %d\n"),
                  state.game_frame));
      break;
  } // end SWITCH
}

void
Splot_MainSDL::keyMove ()
{
  State_t& state = SPLOT_STATE_SINGLETON::instance ()->get ();
  if (state.game_mode != GAMEMODE_GAME)
    return; // nothing to do

  const Uint8* keystate = NULL;
#if SDL_VERSION_ATLEAST (2,0,0)
  keystate = SDL_GetKeyboardState (NULL);
#define LEFT SDL_SCANCODE_LEFT
#define RIGHT SDL_SCANCODE_RIGHT
#define UP SDL_SCANCODE_UP
#define DOWN SDL_SCANCODE_DOWN
#define KP1 SDL_SCANCODE_KP_1
#define KP2 SDL_SCANCODE_KP_2
#define KP3 SDL_SCANCODE_KP_3
#define KP4 SDL_SCANCODE_KP_4
#define KP5 SDL_SCANCODE_KP_5
#define KP6 SDL_SCANCODE_KP_6
#define KP7 SDL_SCANCODE_KP_7
#define KP8 SDL_SCANCODE_KP_8
#define KP9 SDL_SCANCODE_KP_9
 #else
  keystate = SDL_GetKeyState (NULL);
#define LEFT SDLK_LEFT
#define RIGHT SDLK_RIGHT
#define UP SDLK_UP
#define DOWN SDLK_DOWN
#define KP1 SDLK_KP1
#define KP2 SDLK_KP2
#define KP3 SDLK_KP3
#define KP4 SDLK_KP4
#define KP5 SDLK_KP5
#define KP6 SDLK_KP6
#define KP7 SDLK_KP7
#define KP8 SDLK_KP8
#define KP9 SDLK_KP9
#endif
  if (keystate[LEFT]  || keystate[KP4]) key_speed_[0] -= 2.0F+abs (key_speed_[0])*0.4F;
  if (keystate[RIGHT] || keystate[KP6]) key_speed_[0] += 2.0F+abs (key_speed_[0])*0.4F;
  if (keystate[UP]    || keystate[KP8]) key_speed_[1] -= 2.0F+abs (key_speed_[1])*0.4F;
  if (keystate[DOWN]  || keystate[KP2]) key_speed_[1] += 2.0F+abs (key_speed_[1])*0.4F;
  if (keystate[KP7]) {key_speed_[0] -= 2.0F+abs (key_speed_[0])*0.4F; key_speed_[1] -= 2.0F+abs (key_speed_[1])*0.4F;}
  if (keystate[KP9]) {key_speed_[0] += 2.0F+abs (key_speed_[0])*0.4F; key_speed_[1] -= 2.0F+abs (key_speed_[1])*0.4F;}
  if (keystate[KP3]) {key_speed_[0] += 2.0F+abs (key_speed_[0])*0.4F; key_speed_[1] += 2.0F+abs (key_speed_[1])*0.4F;}
  if (keystate[KP1]) {key_speed_[0] -= 2.0F+abs (key_speed_[0])*0.4F; key_speed_[1] += 2.0F+abs (key_speed_[1])*0.4F;}
  //float s = (1.0-game->speedAdj)+(game->speedAdj*0.7);
  float s = 0.7F;
  key_speed_[0] *= s;
  key_speed_[1] *= s;
  state.player->moveEvent ((int)key_speed_[0], (int)key_speed_[1]);
}

void
Splot_MainSDL::mouseMotion (SDL_Event* event_in)
{
  if (!mouseToggle_)
    return; // nothing to do

  if (event_in->motion.x == mid_[0] &&
      event_in->motion.y == mid_[1])
  {
    last_[0] = event_in->motion.x;
    last_[1] = event_in->motion.y;

    return;
  } // end IF

  int xDiff, yDiff;
  xDiff = event_in->motion.x-last_[0];
  yDiff = event_in->motion.y-last_[1];
  State_t& state = SPLOT_STATE_SINGLETON::instance ()->get ();
  if (xDiff || yDiff)
  {
    state.player->moveEvent (xDiff, yDiff);
#if SDL_VERSION_ATLEAST(2,0,0)
    SDL_WarpMouseInWindow (window_, mid_[0], mid_[1]);
#else
    SDL_WarpMouse (mid_[0], mid_[1]);
#endif
  } // end IF

  last_[0] = event_in->motion.x;
  last_[1] = event_in->motion.y;
}

void
Splot_MainSDL::mouseButtonDown (SDL_Event* event_in)
{
  State_t& state = SPLOT_STATE_SINGLETON::instance ()->get ();
  switch (state.game_mode)
  {
    case GAMEMODE_GAME:
    {
      SDL_MouseButtonEvent* mEv = (SDL_MouseButtonEvent*)event_in;
      switch (mEv->button)
      {
        case SDL_BUTTON_LEFT:
          //				game->hero->fireGun(++fire);
          state.player->fireGun (true);
          break;
        case SDL_BUTTON_MIDDLE:
          state.player->nextItem ();
          break;
        case SDL_BUTTON_RIGHT:
          state.player->useItem ();
          break;
        default:
          break;
      } // end SWITCH
      break;
    }
    case GAMEMODE_GAME_OVER:
      state.menu->keyHit (KEY_ENTER);
    default:
      break;
  } // end SWITCH
}

void
Splot_MainSDL::mouseButtonUp (SDL_Event* event_in)
{
  State_t& state = SPLOT_STATE_SINGLETON::instance ()->get ();
  SDL_MouseButtonEvent* mEv = (SDL_MouseButtonEvent*)event_in;
  switch (state.game_mode)
  {
    case GAMEMODE_MENU:
      switch (mEv->button)
      {
        case SDL_BUTTON_LEFT:
          state.menu->mousePress (BUTTON_LEFT, mEv->x, mEv->y);
          break;
        case SDL_BUTTON_MIDDLE:
          state.menu->mousePress (BUTTON_MIDDLE, mEv->x, mEv->y);
          break;
        case SDL_BUTTON_RIGHT:
          state.menu->mousePress (BUTTON_RIGHT, mEv->x, mEv->y);
          break;
        default:
          break;
      } // end SWITCH
    default:
      switch (mEv->button)
      {
        case  SDL_BUTTON_LEFT:
      //				game->hero->fireGun(--fire);
      //				game->hero->fireGun(SDL_GetMouseState(NULL, NULL)&SDL_BUTTON(1));
          state.player->fireGun (false);
          break;
        default:
          break;
      } // end SWITCH
      break;
  } // end SWITCH
}

void
Splot_MainSDL::grabMouse (bool status_in, bool warpmouse_in)
{
  const Configuration_t& configuration =
    SPLOT_CONFIGURATION_SINGLETON::instance ()->get ();
  if (configuration.debug)
    ACE_DEBUG ((LM_INFO,
                ACE_TEXT ("grabMouse(%d)\n"),
                (int)status_in));

  mouseToggle_ = status_in;
  int status_before = SDL_ShowCursor ((int)!status_in); // toggle ?
  ACE_UNUSED_ARG (status_before);

  if (!warpmouse_in)
    return;

  mid_[0] = configuration.screen_width/2;
  mid_[1] = configuration.screen_height/2;
#if SDL_VERSION_ATLEAST (2,0,0)
  SDL_WarpMouseInWindow(window_, mid_[0], mid_[1]);
#else
  SDL_WarpMouse (mid_[0], mid_[1]);
#endif
  last_[0] = mid_[0];
  last_[1] = mid_[1];
}

void
Splot_MainSDL::joystickMotion (SDL_Event* event_in)
{
  static int c = 0;
  const Configuration_t& configuration =
    SPLOT_CONFIGURATION_SINGLETON::instance ()->get ();
  if (configuration.debug)
    ACE_DEBUG ((LM_INFO,
                ACE_TEXT ("joy %05d : axis(%d), value(%d)\n"),
                c++,
                event_in->jaxis.axis, event_in->jaxis.value));

  //if (!joystickToggle_)
  //  return; // nothing to do

  if (event_in->motion.x == mid_[0] &&
      event_in->motion.y == mid_[1])
  {
    last_[0] = event_in->motion.x;
    last_[1] = event_in->motion.y;

    return;
  } // end IF

  State_t& state = SPLOT_STATE_SINGLETON::instance ()->get ();
  int xDiff = event_in->motion.x-last_[0];
  int yDiff = event_in->motion.y-last_[1];
  if (xDiff || yDiff)
  {
    state.player->moveEvent (xDiff, yDiff);
    SDL_WarpMouse (mid_[0], mid_[1]);
  } // end IF

  last_[0] = event_in->motion.x;
  last_[1] = event_in->motion.y;
}

void
Splot_MainSDL::joystickButtonDown (SDL_Event* event_in)
{
  State_t& state = SPLOT_STATE_SINGLETON::instance ()->get ();
  state.player->fireGun (++fire_);
}

void
Splot_MainSDL::joystickButtonUp (SDL_Event* event_in)
{
  State_t& state = SPLOT_STATE_SINGLETON::instance ()->get ();
  state.player->fireGun (--fire_);
}

void
Splot_MainSDL::joystickMove ()
{
#ifdef WITH_SDL_JOYSTICK
  if (!joystick_)
    return; // nothing to do

  State_t& state = SPLOT_STATE_SINGLETON::instance ()->get ();
  static int div = 32768/16;
  joy_[0] = (float)SDL_JoystickGetAxis (joystick_, 0)/div;
  joy_[1] = (float)SDL_JoystickGetAxis (joystick_, 1) / div;
  jNow_[0] = 0.8F*jNow_[0] + 0.2F*joy_[0];
  jNow_[1] = 0.8F*jNow_[1] + 0.2F*joy_[1];
  state.player->moveEvent ((int)jNow_[0], (int)jNow_[1]);
#endif
}

#endif // USE_SDL
