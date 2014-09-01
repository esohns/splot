#include "stdafx.h"

#include "powerup.h"

#include <cmath>

#include <string>

#include "ace/Default_Constants.h"
#include "ace/OS_Memory.h"
#include "ace/OS.h"
#include "ace/Log_Msg.h"

#include "common.h"
#include "state.h"
#include "configuration.h"
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
//  float p[3] = { 0.0, 0.0, 0.0 };
//  float v[3] = { 0.0, 0.0, 0.0 };
//  ACE_NEW (inherited::free_list_,
//           Splot_PowerUp (POWERUP_INVALID,
//                          p, v, 1.0));
//  inherited::size_++;

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
//  inherited::dealloc (1);
  ACE_ASSERT (inherited::size_ == 0);
}

void
Splot_PowerUps::loadTextures ()
{
  std::string path_base = ACE_TEXT_ALWAYS_CHAR (SPLOT_IMAGE_DATA_DIR);
  path_base += ACE_DIRECTORY_SEPARATOR_STR;
  std::string filename = path_base + ACE_TEXT_ALWAYS_CHAR ("powerUpTex.png");
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
  filename = path_base + ACE_TEXT_ALWAYS_CHAR ("powerUpAmmo.png");
  tex_[POWERUP_AMMO_0] = Splot_Image::load (dataLoc (filename.c_str ()));
  if (!tex_[POWERUP_AMMO_0])
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to Splot_Image::load (\"%s\"), continuing\n"),
                ACE_TEXT (filename.c_str ())));
  tex_[POWERUP_AMMO_1] = tex_[POWERUP_AMMO_0];
  tex_[POWERUP_AMMO_2] = tex_[POWERUP_AMMO_0];
  filename = path_base + ACE_TEXT_ALWAYS_CHAR ("powerUpShield.png");
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
  Splot_PowerUp* current;
  do
  {
    current = NULL;
    if (inherited::size_ > 1)
      current = inherited::remove ();
    else
      break;

    ACE_ASSERT (current);
    Splot_Screen::remove (current);
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
Splot_PowerUps::remove (Splot_PowerUp* powerUp_in)
{
  ACE_ASSERT (powerUp_in);

  Splot_PowerUp* prev = inherited::free_list_;
  Splot_PowerUp* current = inherited::free_list_;
  while (current)
  {
    if (current == powerUp_in)
      break;

    prev = current;
    current = current->get_next ();
  } // end WHILE
  if (current == powerUp_in)
    currentPwrUp_ = prev;

  if (!current)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("element not found (was: %@), returning\n"),
                powerUp_in));

    return;
  } // end IF
  prev->set_next (current->get_next ());
  inherited::size_--;

  Splot_Screen::remove (powerUp_in);

//  const Configuration_t& configuration =
//   SPLOT_CONFIGURATION_SINGLETON::instance ()->get ();
//  if (configuration.debug)
//    ACE_DEBUG ((LM_INFO,
//                ACE_TEXT ("active power-ups: %u\n"),
//                inherited::size_));
}

void
Splot_PowerUps::update ()
{
  State_t& state = SPLOT_STATE_SINGLETON::instance ()->get ();
  GameState_t& game_state = SPLOT_STATE_SINGLETON::instance ()->gameState ();
  const Configuration_t& configuration =
   SPLOT_CONFIGURATION_SINGLETON::instance ()->get ();

  Splot_PowerUp* current = inherited::free_list_;
  Splot_PowerUp* delUp = NULL;
  float s = 0.0, score = 0.0F;
  float b = configuration.screen_bound[0]-1.0F;
  while (current)
  {
    current->age_++;
    current->position_[1] += (speed_*state.speed_adj);
    if (current->velocity_[0] || current->velocity_[1])
    {
      s = (1.0F-state.speed_adj)+(state.speed_adj*0.982F);
      current->velocity_[0] *= s;
      current->velocity_[1] *= s;
      current->position_[0] += current->velocity_[0];
      current->position_[1] += current->velocity_[1];
      if (current->velocity_[0] < 0.01) current->velocity_[0] = 0.0;
      if (current->velocity_[1] < 0.01) current->velocity_[1] = 0.0;
    } // end IF
    if (current->position_[0] < -b)
      current->position_[0] = -b;
    else if (current->position_[0] >  b)
      current->position_[0] = b;

    if (current->position_[1] < -12)
    {
      if (state.game_mode == GAMEMODE_GAME)
        switch (current->type_)
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
      delUp = current;
      current = current->get_next ();
      remove (delUp);
    } // end IF
    else
      current = current->get_next ();
  } // end WHILE
  game_state.score += score;
}

void
Splot_PowerUps::drawGL ()
{
  float szp;
  Splot_PowerUp* current = inherited::free_list_;
  while (current &&
         (current->type_ != POWERUP_INVALID))
  {
    szp = pwrUpSize_[current->type_][0]*2.5F;

    glColor4fv (pwrUpColor_[current->type_]);
    glBindTexture (GL_TEXTURE_2D, pwrTex_);
    glPushMatrix ();
    glTranslatef (current->position_[0]+wobble_0_[current->age_%WOBBLE_0],
                  current->position_[1]+wobble_1_[current->age_%WOBBLE_1],
                  current->position_[2]);
    glRotatef ((float)IRAND, 0.0, 0.0, 1.0);
    glBegin (GL_QUADS);
    glTexCoord2f (0.0F, 0.0F); glVertex3f (-szp,  szp, 0.0F );
    glTexCoord2f (0.0F, 1.0F); glVertex3f (-szp, -szp, 0.0F );
    glTexCoord2f (1.0F, 1.0F); glVertex3f ( szp, -szp, 0.0F );
    glTexCoord2f (1.0F, 0.0F); glVertex3f ( szp,  szp, 0.0F );
    glEnd ();
    glPopMatrix ();

    current = current->get_next ();
  } // end WHILE

  glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  current = inherited::free_list_;
  while (current &&
         (current->type_ != POWERUP_INVALID))
  {
    glColor4f (1.0F, 1.0F, 1.0F, 1.0F);
    glBindTexture (GL_TEXTURE_2D, tex_[current->type_]);
    glPushMatrix ();
    glTranslatef(current->position_[0]+wobble_0_[current->age_%WOBBLE_0],
                 current->position_[1]+wobble_1_[current->age_%WOBBLE_1],
                 current->position_[2]);
    glBegin (GL_QUADS);
    glTexCoord2f (0.0F, 0.0F); glVertex3f (-pwrUpSize_[current->type_][0], pwrUpSize_[current->type_][1], 0.0F);
    glTexCoord2f (0.0F, 1.0F); glVertex3f (-pwrUpSize_[current->type_][0],-pwrUpSize_[current->type_][1], 0.0F);
    glTexCoord2f (1.0F, 1.0F); glVertex3f ( pwrUpSize_[current->type_][0],-pwrUpSize_[current->type_][1], 0.0F);
    glTexCoord2f (1.0F, 0.0F); glVertex3f ( pwrUpSize_[current->type_][0], pwrUpSize_[current->type_][1], 0.0F);
    glEnd ();
    glPopMatrix();

    current = current->get_next ();
  } // end WHILE

  glBlendFunc (GL_SRC_ALPHA, GL_ONE);
}

// init statics
unsigned int Splot_PowerUp::count = 0;

Splot_PowerUp::Splot_PowerUp ()
 : inherited ()
 , type_(POWERUP_INVALID)
{
  inherited::type_ = GAMEELEMENT_POWERUP;

  potency_ = 1.0F;

  Splot_PowerUp::count++;
}

Splot_PowerUp::Splot_PowerUp (PowerUpType_t type_in,
                              const float (&position_in)[3],
                              const float (&velocity_in)[3],
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

  Splot_PowerUp::count++;
}

Splot_PowerUp::~Splot_PowerUp ()
{
  Splot_PowerUp::count--;
}
