#include "stdafx.h"

#include "OpenGL_common.h"

#ifdef HAVE_CONFIG_H
#include "splot-config.h"
#endif

#include "ace/OS_Memory.h"
#include "ace/Log_Msg.h"

#if defined (USE_GLC_TEXT)
#include "TextGLC.h"
#endif
#if defined (USE_FTGL_TEXT)
#include "text_FTGL.h"
#endif

#include "configuration.h"
#include "state.h"
#include "highscore.h"
#include "background.h"
#include "player_bullets.h"
#include "enemy_bullets.h"
#include "player_aircraft.h"
#include "explosion.h"
#include "powerup.h"
#include "audio.h"
#include "text.h"
#include "menu.h"
#include "screen.h"
#include "status_display.h"

// init statics
bool Splot_OpenGLCommon::initialized_ = false;

bool
Splot_OpenGLCommon::init ()
{
  if (!Splot_OpenGLCommon::initialized_)
  {
    if (!Splot_OpenGLCommon::initTextToolkit ())
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to Splot_OpenGLCommon::initTextToolkit(), aborting\n")));

      return false;
    } // end IF

    Splot_OpenGLCommon::initialized_ = true;
  } // end IF

  return Splot_OpenGLCommon::initialized_;
}

void
Splot_OpenGLCommon::fini ()
{
  Splot_OpenGLCommon::finiTextToolkit ();
}

bool
Splot_OpenGLCommon::initScreen ()
{
  const Configuration_t& configuration =
    SPLOT_CONFIGURATION_SINGLETON::instance ()->get ();
  if (configuration.debug)
    ACE_DEBUG ((LM_INFO,
                ACE_TEXT ("init screen\n")));

  Splot_OpenGLCommon::reshape (configuration.screen_width,
                               configuration.screen_height);

  glDisable (GL_DEPTH_TEST);
  glDepthFunc (GL_LEQUAL);

  glEnable (GL_TEXTURE_2D);

  glEnable (GL_BLEND);
  //	glDisable (GL_BLEND);
  glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  if (configuration.blend_enabled)
  {
    //glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    //glEnable (GL_BLEND);
    glDisable (GL_ALPHA_TEST);
  } // end IF
  else
  {
    glDisable (GL_BLEND);

    glAlphaFunc (GL_GREATER, 0.2F);
    glEnable (GL_ALPHA_TEST);
  } // end ELSE

  //	glDisable(GL_CULL_FACE);
  glEnable (GL_CULL_FACE);
  glDisable (GL_NORMALIZE);

  glPointSize (1.0);
  glLineWidth (1.0);
  glClearColor (0.0, 0.0, 0.0, 1.0);

#ifdef USE_GLPNG_IMAGE
  pngSetViewingGamma (configuration.view_gamma);
#endif

  return true;
}

bool
Splot_OpenGLCommon::initTextToolkit ()
{
  State_t& state = SPLOT_STATE_SINGLETON::instance ()->get ();
#if defined (USE_GLC_TEXT) && defined (USE_FTGL_TEXT)
  const Configuration_t& configuration =
      SPLOT_CONFIGURATION_SINGLETON::instance ()->get ();
  if (configuration.text_type == TEXT_GLC)
    ACE_NEW_RETURN (state.text,
                    TextGLC (),
                    false);
  else
    ACE_NEW_RETURN (state.text,
                    TextFTGL (),
                    false);
#elif defined (USE_GLC_TEXT)
  ACE_NEW_RETURN (state.text,
                  TextGLC (),
                  false);
#elif defined (USE_FTGL_TEXT)
  ACE_NEW_RETURN (state.text,
                  Splot_TextFTGL (),
                  false);
#else
#error "USE_GLC_TEXT or USE_FTGL_TEXT must be defined"
#endif

  return true;
}

void
Splot_OpenGLCommon::finiTextToolkit ()
{
  State_t& state = SPLOT_STATE_SINGLETON::instance ()->get ();
  if (state.text)
    delete state.text;
}

void
Splot_OpenGLCommon::draw ()
{
  State_t& state = SPLOT_STATE_SINGLETON::instance ()->get ();
  switch (state.game_mode)
  {
    case GAMEMODE_GAME:
      drawGame ();
      break;
    case GAMEMODE_GAME_OVER:
      drawGameOver ();
      break;
    case GAMEMODE_LEVEL_COMPLETE:
      drawLevelComplete ();
      break;
    case GAMEMODE_MENU:
      state.menu->drawGL ();
      break;
    default:
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("invalid/unknown game mode (was: %d), returning\n"),
                  state.game_mode));
      return;
  } // end SWITCH
}

void
Splot_OpenGLCommon::drawGame ()
{
  const Configuration_t& configuration =
    SPLOT_CONFIGURATION_SINGLETON::instance ()->get ();
  State_t& state = SPLOT_STATE_SINGLETON::instance ()->get ();

  //-- Clear buffers
  glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  //	glClear( GL_COLOR_BUFFER_BIT );

  //-- Place camera
  glLoadIdentity ();
  glTranslatef (0.0, 0.0, configuration.z_transformation);
  //	glTranslatef(0.0, 5.0, -12.0);

  if (!state.game_pause)
  {
    //-- Add items to scene
    Splot_Screen::put ();

    //-- Update scene
    state.enemies->update ();
    state.power_ups->update ();
    state.player_bullets->update ();
    state.enemy_bullets->update ();
    state.player_bullets->checkForHits (state.enemies->getFleet ());
    if (state.game_mode == GAMEMODE_GAME)
    {
      state.enemy_bullets->checkForHits (state.player);
      state.player->checkForCollisions (state.enemies->getFleet ());
      state.player->checkForPowerUps (state.power_ups);
    } // end IF
    state.explosions->update ();
//    state.audio->update ();

    state.player->update ();
    state.game_frame++;
  } // end IF

  glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  //-- Draw background
  state.background->drawGL ();

  //-- Draw actors
  state.enemies->drawGL ();
  state.player->drawGL ();

  if (configuration.graphics_level > 0)
    state.status_display->darkenGL ();

  glBlendFunc (GL_SRC_ALPHA, GL_ONE);

  state.power_ups->drawGL ();

  //-- Draw ammo
  state.player_bullets->drawGL ();
  state.enemy_bullets->drawGL ();

  //-- Draw explosions
  state.explosions->drawGL ();

  //-- Draw stats
  state.status_display->drawGL (state.player);
}

void
Splot_OpenGLCommon::drawGameOver ()
{
  const Configuration_t& configuration =
    SPLOT_CONFIGURATION_SINGLETON::instance ()->get ();
  State_t& state = SPLOT_STATE_SINGLETON::instance ()->get ();

  state.player_death--;

  //-- Clear buffers
  glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  //-- Place camera
  glLoadIdentity ();
  if (state.player_death > 0)
  {
    float z = 1.0F*state.player_death/DEATH_TIME;
    glTranslatef (0.0, 0.0, configuration.z_transformation-z*z);
  } // end IF
  else
    glTranslatef (0.0, 0.0, configuration.z_transformation);

  //-- Add items to scene
  Splot_Screen::put ();

  //-- Update scene
  state.explosions->update ();
  state.power_ups->update ();
  state.enemies->update ();
  state.player_bullets->update ();
  state.enemy_bullets->update ();
  state.player_bullets->checkForHits (state.enemies->getFleet ());
//  state.audio->update ();
  state.player->update ();
  state.game_frame++;

  glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  
  //-- Draw background
  state.background->drawGL ();
  //-- Draw actors
  state.enemies->drawGL ();

  if (configuration.graphics_level > 0)
    state.status_display->darkenGL ();

  glBlendFunc (GL_SRC_ALPHA, GL_ONE);

  state.power_ups->drawGL ();

  //-- Draw ammo
  state.player_bullets->drawGL ();
  state.enemy_bullets->drawGL ();
  //-- Draw explosions
  state.explosions->drawGL ();
  //-- Draw stats
  state.status_display->drawGL (state.player);

  float score = SPLOT_STATE_SINGLETON::instance ()->gameState ().score;
  int rank = state.highscore->check (SPLOT_CONFIGURATION_SINGLETON::instance ()->intSkill (), score);
  char buffer[BUFSIZ];
  ACE_OS::memset (&buffer, 0, sizeof (buffer));
  float scale = 0.15F;
  float pulse = (float)state.player_death;
  if (rank == 1)
    ACE_OS::sprintf (buffer, ACE_TEXT_ALWAYS_CHAR ("new high score!\n\n%d"),
                     (int)score);
  else if (rank > 1)
    ACE_OS::sprintf (buffer, ACE_TEXT_ALWAYS_CHAR ("not bad!\nrank: %d\n%d"),
                     rank, (int)score);
  else
  {
    scale = 0.10F;
    ACE_OS::sprintf (buffer, ACE_TEXT_ALWAYS_CHAR ("G A M E   O V E R\nmissed a rank by: %d\n%d"),
                     (int)(state.highscore->getScore (SPLOT_CONFIGURATION_SINGLETON::instance ()->intSkill (), HI_SCORE_HIST-1)-score), (int)score);
  } // end ELSE
  Splot_OpenGLCommon::drawText (buffer, pulse, scale);
}

void
Splot_OpenGLCommon::drawLevelComplete ()
{
  const Configuration_t& configuration =
    SPLOT_CONFIGURATION_SINGLETON::instance ()->get ();
  State_t& state = SPLOT_STATE_SINGLETON::instance ()->get ();

  state.player_success--;

  if (state.player_success < -500)
  {
    SPLOT_STATE_SINGLETON::instance ()->gotoNextLevel ();
    state.game_mode = GAMEMODE_GAME;
    state.audio->setMusicMode (MUSIC_GAME);
    state.audio->setMusicVolume (configuration.volume_music);
    return;
  } // end IF

  // fade out music
  float fade_factor = -state.player_success/450.0F;
  if (state.player_success < 0)
  {
    float vol =
      configuration.volume_music-(configuration.volume_music*fade_factor);
    if (vol < 0.0) vol = 0.0;
    state.audio->setMusicVolume (vol);
  } // end IF

  //-- Clear buffers
  glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  //-- Place camera
  glLoadIdentity ();
  glTranslatef (0.0, 0.0, configuration.z_transformation);

  //-- Update scene
  state.enemies->update ();
  state.explosions->update ();
  state.player_bullets->update ();
  state.player->update ();
//  state.audio->update ();

  glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  //-- Draw background
  state.background->drawGL ();
  //-- Draw actors
  state.player->drawGL ();

  if (configuration.graphics_level > 0)
    state.status_display->darkenGL ();

  glBlendFunc (GL_SRC_ALPHA, GL_ONE);
  //-- Draw ammo
  state.player_bullets->drawGL ();
  //-- Draw explosions
  state.explosions->drawGL ();
  //-- Draw stats
  state.status_display->drawGL (state.player);

  char buffer[BUFSIZ];
  ACE_OS::sprintf (buffer, ACE_TEXT_ALWAYS_CHAR ("congratulations!\n \nl e v e l\n %d \nc o m p l e t e\n \n"),
                   state.game_level);
  //	if(game->hero->getScore() > game->hiScore[config->intSkill()][0])
  //	{
  //		sprintf(buffer, _("congratulations!\n \nl e v e l\n %d \nc o m p l e t e\n \n"), game->gameLevel);
  //	}
  //	else
  //	{
  //		sprintf(buffer, _("congratulations!\n \nl e v e l\n %d \nc o m p l e t e\n \nn e w   h i g h   s c o r e : \n %g \n"), game->gameLevel, game->hero->getScore());
  //	}
  Splot_OpenGLCommon::drawText (buffer, (float)state.player_success, 0.15F);
}

void
Splot_OpenGLCommon::drawText (const char* string_in,
                              float pulse_in,
                              float scale_in)
{
  State_t& state = SPLOT_STATE_SINGLETON::instance ()->get ();

  //-- alpha
  float tmp = 0.5F+0.5F*::sin (pulse_in*0.02F);
  float aa, ca;
  aa = 0.7F-0.5F*tmp;
  if (pulse_in > -50.0)
    aa *= (-pulse_in/50.0F);
  ca = 1.0F-tmp;

  float width, height;
  height = 1.5F*state.text->LineHeight ();

  char buffer[BUFSIZ];
  ACE_OS::strncpy (buffer, string_in, sizeof (buffer));
  char* index[10] = {NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL};
  index[0] = buffer;
  char* walker   = buffer;
  int lines = 1;
  char* newline = NULL;
  while ((newline = ACE_OS::strchr (walker, '\n')) != NULL)
  {
    walker = newline+1;
    index[lines] = newline+1;
    *newline = '\0';
    lines++;
  } // end WHILE

  float x_sin, y_sin, y, min_y;
  min_y = 0.5F*height*lines;
  for (int l = 0; l < lines; l++)
  {
    y = min_y-height*(l+1);
    if (index[l] && ACE_OS::strlen (index[l]) <= 0)
      continue;

    for (int i = 0; i < 6; i++)
    {
      glColor4f (1.0, ca*ca*0.3F, ca*0.3F, aa*aa);
      x_sin = 1.75F*::sin (i+state.frame*0.06F);
      y_sin = 0.75F*::sin (i+state.frame*0.09F);

      glPushMatrix ();
      glScalef (scale_in, scale_in*0.75F, 1.0);
      width = state.text->Advance (index[l]);
      glTranslatef (-(width/2.0F)-x_sin, y+y_sin, 0.0);
      state.text->Render (index[l]);
      glPopMatrix ();
    } // end FOR
  } // end FOR
}

void
Splot_OpenGLCommon::reshape (int, int)
{
  const Configuration_t& configuration =
    SPLOT_CONFIGURATION_SINGLETON::instance ()->get ();

  glMatrixMode (GL_PROJECTION);
  glLoadIdentity ();
  gluPerspective (configuration.screen_FOV,
                  (float)configuration.screen_width/(float)configuration.screen_height,
                  configuration.screen_z_near,
                  configuration.screen_z_far);
  glMatrixMode (GL_MODELVIEW);
  glViewport (0, 0, configuration.screen_width, configuration.screen_height);
}
