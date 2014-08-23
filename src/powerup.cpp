#include "stdafx.h"

#include "powerup.h"

//#ifdef HAVE_CONFIG_H
//#include <chromium-bsu-config.h>
//#endif
//
//#include "gettext.h"
//
//#include "Splot_PowerUps.h"
//
//#include <cstdio>
//#include <cstdlib>
//#include <cmath>
//
//#include "compatibility.h"

//#if defined(HAVE_APPLE_OPENGL_FRAMEWORK) || defined(HAVE_OPENGL_GL_H)
//#include <OpenGL/gl.h>
//#else
//#include <GL/gl.h>
//#endif

#include <math.h>

#include <string>

#include "ace/Default_Constants.h"
#include "ace/OS_Memory.h"
#include "ace/OS.h"
#include "ace/Log_Msg.h"

#include "common.h"
#include "state.h"
#include "configuration.h"
//#include "Explosions.h"
//#include "EnemyFleet.h"
//#include "EnemyAircraft.h"
#include "player_aircraft.h"
#include "screen.h"
#include "image.h"

Splot_PowerUps::Splot_PowerUps ()
 : inherited (ACE_PURE_FREE_LIST,
              ACE_DEFAULT_FREE_LIST_PREALLOC,
              ACE_DEFAULT_FREE_LIST_LWM,
              ACE_DEFAULT_FREE_LIST_HWM,
              ACE_DEFAULT_FREE_LIST_INC)
 , currentPwrUp_ (NULL)
 , pwrTex_ (0)
 , speed_ (0.0F)
{
  float p[3] = { 0.0, 0.0, 0.0 };
  float v[3] = { 0.0, 0.0, 0.0 };
  ACE_NEW (inherited::free_list_,
           Splot_PowerUp (POWERUP_INVALID,
                          p, v, 1.0));
  inherited::size_++;

  for (int i = 0; i < POWERUP_MAX_TYPES; i++)
  {
    pwrUpSize_[i][0] = 0.6F;
    pwrUpSize_[i][1] = 0.6F;
    tex_[i] = 0;
  } // end FOR

  pwrUpColor_[POWERUP_AMMO_0][0] = 1.0F;
  pwrUpColor_[POWERUP_AMMO_0][1] = 0.8F;
  pwrUpColor_[POWERUP_AMMO_0][2] = 0.5F;
  pwrUpColor_[POWERUP_AMMO_0][3] = 0.8F;

  pwrUpColor_[POWERUP_AMMO_1][0] = 0.0F;
  pwrUpColor_[POWERUP_AMMO_1][1] = 1.0F;
  pwrUpColor_[POWERUP_AMMO_1][2] = 0.5F;
  pwrUpColor_[POWERUP_AMMO_1][3] = 0.8F;

  pwrUpColor_[POWERUP_AMMO_2][0] = 0.4F;
  pwrUpColor_[POWERUP_AMMO_2][1] = 0.2F;
  pwrUpColor_[POWERUP_AMMO_2][2] = 1.0F;
  pwrUpColor_[POWERUP_AMMO_2][3] = 1.0F;

  pwrUpColor_[POWERUP_REPAIR][0] = 1.0F;
  pwrUpColor_[POWERUP_REPAIR][1] = 0.1F;
  pwrUpColor_[POWERUP_REPAIR][2] = 0.0F;
  pwrUpColor_[POWERUP_REPAIR][3] = 1.0F;

  pwrUpColor_[POWERUP_SHIELD][0] = 0.5F;
  pwrUpColor_[POWERUP_SHIELD][1] = 0.7F;
  pwrUpColor_[POWERUP_SHIELD][2] = 1.0F;
  pwrUpColor_[POWERUP_SHIELD][3] = 1.0F;

  pwrUpColor_[POWERUP_SHIELD_SUPER][0] = 1.0F;
  pwrUpColor_[POWERUP_SHIELD_SUPER][1] = 0.6F;
  pwrUpColor_[POWERUP_SHIELD_SUPER][2] = 0.0F;
  pwrUpColor_[POWERUP_SHIELD_SUPER][3] = 1.0F;

  State_t& state = SPLOT_STATE_SINGLETON::instance ()->get ();
  speed_ = state.scroll_speed*0.8F;

  for (int i = 0; i < WOBBLE_0; i++)
    wobble_0_[i] = 0.1F*(float)sin (2.0*M_PI*((double)i/(double)WOBBLE_0));
  for (int i = 0; i < WOBBLE_1; i++)
    wobble_1_[i] = 0.3F*(float)sin (2.0*M_PI*((double)i/(double)WOBBLE_1));

  loadTextures ();
}

Splot_PowerUps::~Splot_PowerUps ()
{
  deleteTextures ();
  clear ();
  inherited::dealloc (1);
  ACE_ASSERT (inherited::size_ == 0);
}

void
Splot_PowerUps::loadTextures ()
{
  std::string path_base = ACE_TEXT_ALWAYS_CHAR (SPLOT_IMAGE_DATA_DIR);
  path_base += ACE_DIRECTORY_SEPARATOR_STR;
  std::string filename = path_base + ACE_TEXT_ALWAYS_CHAR ("PowerUpTex.png");
  pwrTex_ = Splot_Image::load (dataLoc (filename.c_str ()),
                               IMG_NOMIPMAPS,
                               IMG_BLEND3,
                               GL_CLAMP,
                               GL_LINEAR,
                               GL_LINEAR);
  if (!pwrTex_)
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to Splot_Image::load (\"%s\"), continuing\n"),
                ACE_TEXT (filename.c_str ())));
  filename = path_base + ACE_TEXT_ALWAYS_CHAR ("PowerUpAmmo.png");
  tex_[POWERUP_AMMO_0] = Splot_Image::load (dataLoc (filename.c_str ()));
  if (!tex_[POWERUP_AMMO_0])
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to Splot_Image::load (\"%s\"), continuing\n"),
                ACE_TEXT (filename.c_str ())));
  tex_[POWERUP_AMMO_1] = tex_[POWERUP_AMMO_0];
  tex_[POWERUP_AMMO_2] = tex_[POWERUP_AMMO_0];
  filename = path_base + ACE_TEXT_ALWAYS_CHAR ("PowerUpShield.png");
  tex_[POWERUP_SHIELD] = Splot_Image::load (dataLoc (filename.c_str ()));
  if (!tex_[POWERUP_SHIELD])
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to Splot_Image::load (\"%s\"), continuing\n"),
                ACE_TEXT (filename.c_str ())));
  tex_[POWERUP_REPAIR] = tex_[POWERUP_SHIELD];
  tex_[POWERUP_SHIELD_SUPER] = tex_[POWERUP_SHIELD];
}

void
Splot_PowerUps::deleteTextures ()
{
  for (int i = 0; i < POWERUP_MAX_TYPES; i++)
    glDeleteTextures (1, &tex_[i]);
  glDeleteTextures (1, &pwrTex_);
}

void
Splot_PowerUps::clear ()
{
  Splot_PowerUp* cur = NULL;
  Splot_PowerUp* del = NULL;
  State_t& state = SPLOT_STATE_SINGLETON::instance ()->get ();
  do
  {
    if (inherited::size_ > 1)
      cur = inherited::remove ();
    else
      break;

    ACE_ASSERT (cur);
    del = cur;
    Splot_Screen::remove (del);
  } while (true);

  currentPwrUp_ = NULL;
}

Splot_PowerUp*
Splot_PowerUps::getFirst ()
{
  currentPwrUp_ = ((inherited::size_ > 1) ? inherited::free_list_
                                          : NULL);

  return currentPwrUp_;
}

void
Splot_PowerUps::remove (Splot_PowerUp* pwr_in)
{
  Splot_PowerUp* prev = NULL;
  if (pwr_in)
  {
    Splot_PowerUp* tmp = inherited::free_list_;
    do
    {
      prev = tmp;
      tmp = tmp->get_next ();
      if (tmp == pwr_in)
        break;
    } while (tmp);
    if (tmp == pwr_in)
      currentPwrUp_ = prev;
  } // end IF
  if (!prev)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("element not found (was: %@), returning\n"),
                pwr_in));

    return;
  } // end IF
  prev->set_next (pwr_in->get_next ());
  inherited::size_--;

  Splot_Screen::remove (pwr_in);

  const Configuration_t& configuration =
   SPLOT_CONFIGURATION_SINGLETON::instance ()->get ();
  if (configuration.debug)
    ACE_DEBUG ((LM_DEBUG,
                ACE_TEXT ("active power-ups: %u\n"),
                inherited::size_));
}

void
Splot_PowerUps::update ()
{
  State_t& state = SPLOT_STATE_SINGLETON::instance ()->get ();
  GameState_t& game_state = SPLOT_STATE_SINGLETON::instance ()->gameState ();
  const Configuration_t& configuration =
   SPLOT_CONFIGURATION_SINGLETON::instance ()->get ();

  Splot_PowerUp* pwrUp = inherited::free_list_;
  Splot_PowerUp* delUp = NULL;
  float s = 0.0, score = 0.0F;
  float b = configuration.screen_bound[0] - 1.0F;
  while (pwrUp &&
         (pwrUp->type_ != POWERUP_INVALID))
  {
    pwrUp->age_++;
    pwrUp->position_[1] += (speed_*state.speed_adj);
    if (pwrUp->velocity_[0] || pwrUp->velocity_[1])
    {
      s = (1.0F-state.speed_adj)+(state.speed_adj*0.982F);
      pwrUp->velocity_[0] *= s;
      pwrUp->velocity_[1] *= s;
      pwrUp->position_[0] += pwrUp->velocity_[0];
      pwrUp->position_[1] += pwrUp->velocity_[1];
      if (pwrUp->velocity_[0] < 0.01) pwrUp->velocity_[0] = 0.0;
      if (pwrUp->velocity_[1] < 0.01) pwrUp->velocity_[1] = 0.0;
    } // end IF
    if (pwrUp->position_[0] < -b)
      pwrUp->position_[0] = -b;
    else if (pwrUp->position_[0] >  b)
      pwrUp->position_[0] = b;

    if (pwrUp->position_[1] < -12)
    {
      if (state.game_mode == GAMEMODE_GAME)
        switch (pwrUp->type_)
        {
          case POWERUP_SHIELD_SUPER:
            state.player->addShip ();
            score += SCORE_POWERUP_SHIELD_SUPER;
            break;
          case POWERUP_SHIELD:
            score += SCORE_POWERUP_SHIELD;
            break;
          case POWERUP_REPAIR:
            score += SCORE_POWERUP_REPAIR;
            break;
          default:
            score += SCORE_POWERUP_AMMUNITION;
            break;
        } // end SWITCH
      delUp = pwrUp;
      pwrUp = pwrUp->get_next ();
      remove (delUp);
    } // end IF
    else
      pwrUp = pwrUp->get_next ();
  } // end WHILE
  game_state.score += score;
}

void
Splot_PowerUps::drawGL ()
{
  float *sz, szp;
  Splot_PowerUp* pwrUp = inherited::free_list_;
  while (pwrUp &&
         (pwrUp->type_ != POWERUP_INVALID))
  {
    sz  = pwrUpSize_[pwrUp->type_];
    szp = sz[0]*2.5F;

    glColor4fv (pwrUpColor_[pwrUp->type_]);
    glBindTexture (GL_TEXTURE_2D, pwrTex_);
    glPushMatrix ();
    glTranslatef (pwrUp->position_[0] + wobble_0_[pwrUp->age_%WOBBLE_0],
                  pwrUp->position_[1] + wobble_1_[pwrUp->age_%WOBBLE_1],
                  pwrUp->position_[2]);
    glRotatef ((float)IRAND, 0.0, 0.0, 1.0);
    glBegin (GL_QUADS);
    glTexCoord2f (0.0F, 0.0F); glVertex3f (-szp,  szp, 0.0F );
    glTexCoord2f (0.0F, 1.0F); glVertex3f (-szp, -szp, 0.0F );
    glTexCoord2f (1.0F, 1.0F); glVertex3f ( szp, -szp, 0.0F );
    glTexCoord2f (1.0F, 0.0F); glVertex3f ( szp,  szp, 0.0F );
    glEnd ();
    glPopMatrix ();

    pwrUp = pwrUp->get_next ();
  } // end WHILE

  glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  pwrUp = inherited::free_list_;
  while (pwrUp &&
         (pwrUp->type_ != POWERUP_INVALID))
  {
    sz = pwrUpSize_[pwrUp->type_];

    glColor4f (1.0F, 1.0F, 1.0F, 1.0F);
    glBindTexture (GL_TEXTURE_2D, tex_[pwrUp->type_]);
    glPushMatrix ();
    glTranslatef(pwrUp->position_[0] + wobble_0_[pwrUp->age_%WOBBLE_0],
                 pwrUp->position_[1] + wobble_1_[pwrUp->age_%WOBBLE_1],
                 pwrUp->position_[2]);
    glBegin (GL_QUADS);
    glTexCoord2f (0.0F, 0.0F); glVertex3f (-sz[0],  sz[1], 0.0F);
    glTexCoord2f (0.0F, 1.0F); glVertex3f (-sz[0], -sz[1], 0.0F);
    glTexCoord2f (1.0F, 1.0F); glVertex3f ( sz[0], -sz[1], 0.0F);
    glTexCoord2f (1.0F, 0.0F); glVertex3f ( sz[0],  sz[1], 0.0F);
    glEnd ();
    glPopMatrix();

    pwrUp = pwrUp->get_next ();
  } // end WHILE

  glBlendFunc (GL_SRC_ALPHA, GL_ONE);
}

// init statics
unsigned int Splot_PowerUp::count_ = 0;

Splot_PowerUp::Splot_PowerUp ()
 : inherited ()
 , type_(POWERUP_INVALID)
{
  inherited::type_ = GAMEELEMENT_POWERUP;

  potency_ = 1.0F;

  count_++;
}

Splot_PowerUp::Splot_PowerUp (PowerUpType_t type_in,
                              float position_in[3],
                              float velocity_in[3],
                              float potency_in)
 : inherited ()
 , type_ (type_in)
{
  inherited::type_ = GAMEELEMENT_POWERUP;
  // *NOTE*: assignment doesn't work
  //inherited::position_ = position_in;
  ACE_OS::memcpy (&(inherited::position_), &position_in, sizeof (position_in));
  //inherited::velocity_ = velocity_in;
  ACE_OS::memcpy (&(inherited::velocity_), &velocity_in, sizeof (velocity_in));

  potency_ = potency_in;

  count_++;
}

Splot_PowerUp::~Splot_PowerUp ()
{
  count_--;
}
