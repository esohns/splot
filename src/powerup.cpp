#include "stdafx.h"

#include "powerup.h"

#include <cmath>
#include <iostream>

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

// init statics
float Splot_PowerUps::size[POWERUP_MAX_TYPES][2];
float Splot_PowerUps::color[POWERUP_MAX_TYPES][4];

void
Splot_PowerUps::initialize ()
{
  for (int i = 0; i < POWERUP_MAX_TYPES; i++)
  {
    Splot_PowerUps::size[i][0] = 0.6F;
    Splot_PowerUps::size[i][1] = 0.6F;
  } // end FOR

  Splot_PowerUps::color[POWERUP_AMMUNITION_0][0] = 1.0F;
  Splot_PowerUps::color[POWERUP_AMMUNITION_0][1] = 0.8F;
  Splot_PowerUps::color[POWERUP_AMMUNITION_0][2] = 0.5F;
  Splot_PowerUps::color[POWERUP_AMMUNITION_0][3] = 0.8F;

  Splot_PowerUps::color[POWERUP_AMMUNITION_1][0] = 0.0F;
  Splot_PowerUps::color[POWERUP_AMMUNITION_1][1] = 1.0F;
  Splot_PowerUps::color[POWERUP_AMMUNITION_1][2] = 0.5F;
  Splot_PowerUps::color[POWERUP_AMMUNITION_1][3] = 0.8F;

  Splot_PowerUps::color[POWERUP_AMMUNITION_2][0] = 0.4F;
  Splot_PowerUps::color[POWERUP_AMMUNITION_2][1] = 0.2F;
  Splot_PowerUps::color[POWERUP_AMMUNITION_2][2] = 1.0F;
  Splot_PowerUps::color[POWERUP_AMMUNITION_2][3] = 1.0F;

  Splot_PowerUps::color[POWERUP_REPAIR][0] = 1.0F;
  Splot_PowerUps::color[POWERUP_REPAIR][1] = 0.1F;
  Splot_PowerUps::color[POWERUP_REPAIR][2] = 0.0F;
  Splot_PowerUps::color[POWERUP_REPAIR][3] = 1.0F;

  Splot_PowerUps::color[POWERUP_SHIELD][0] = 0.5F;
  Splot_PowerUps::color[POWERUP_SHIELD][1] = 0.7F;
  Splot_PowerUps::color[POWERUP_SHIELD][2] = 1.0F;
  Splot_PowerUps::color[POWERUP_SHIELD][3] = 1.0F;

  Splot_PowerUps::color[POWERUP_SHIELD_SUPER][0] = 1.0F;
  Splot_PowerUps::color[POWERUP_SHIELD_SUPER][1] = 0.6F;
  Splot_PowerUps::color[POWERUP_SHIELD_SUPER][2] = 0.0F;
  Splot_PowerUps::color[POWERUP_SHIELD_SUPER][3] = 1.0F;
}

Splot_PowerUps::Splot_PowerUps ()
 : inherited (ACE_PURE_FREE_LIST,
              ACE_DEFAULT_FREE_LIST_PREALLOC,
              ACE_DEFAULT_FREE_LIST_LWM,
              ACE_DEFAULT_FREE_LIST_HWM,
              ACE_DEFAULT_FREE_LIST_INC)
 , current_ (NULL)
 , pwrTex_ (0)
 , speed_ (0.0F)
{
  for (int i = 0; i < POWERUP_MAX_TYPES; i++)
    tex_[i] = 0;

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
  tex_[POWERUP_AMMUNITION_0] = Splot_Image::load (dataLoc (filename.c_str ()));
  if (!tex_[POWERUP_AMMUNITION_0])
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to Splot_Image::load (\"%s\"), continuing\n"),
                ACE_TEXT (filename.c_str ())));
  tex_[POWERUP_AMMUNITION_1] = tex_[POWERUP_AMMUNITION_0];
  tex_[POWERUP_AMMUNITION_2] = tex_[POWERUP_AMMUNITION_0];
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
  Splot_PowerUp* current = inherited::remove ();
  while (current)
  {
    Splot_Screen::remove (current);
    current = inherited::remove ();
  } // end WHILE

  current_ = NULL;
}

Splot_PowerUp*
Splot_PowerUps::getFirst ()
{
  current_ = inherited::free_list_;

  return current_;
}

void
Splot_PowerUps::remove (Splot_PowerUp* powerUp_in)
{
  ACE_ASSERT (powerUp_in);

  Splot_PowerUp* prev = NULL;
  Splot_PowerUp* current = inherited::free_list_;
  while (current)
  {
    if (current == powerUp_in)
      break;

    prev = current;
    current = current->get_next ();
  } // end WHILE
  if (!current)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("element not found (was: %@), returning\n"),
                powerUp_in));

    return;
  } // end IF
  current_ = prev;
  if (prev)
  {
    prev->set_next (current->get_next ());
    inherited::size_--;
  } // end IF
  else
    inherited::remove (); // was head element, now empty

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
  Splot_PowerUp* expired = NULL;
  float s = 0.0, score = 0.0F;
  float b = configuration.screen_bound[0]-1.0F;
  while (current)
  {
    current->age_++;
    current->position_[1] += (speed_*state.speed_adj);
    if (current->translationVector_[0] || current->translationVector_[1])
    {
      s = (1.0F-state.speed_adj)+(state.speed_adj*0.982F);
      current->translationVector_[0] *= s;
      current->translationVector_[1] *= s;
      current->position_[0] += current->translationVector_[0];
      current->position_[1] += current->translationVector_[1];
      if (current->translationVector_[0] < 0.01)
        current->translationVector_[0] = 0.0;
      if (current->translationVector_[1] < 0.01)
        current->translationVector_[1] = 0.0;
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
      expired = current;
      current = current->get_next ();
      remove (expired);
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
  while (current)
  {
    szp = Splot_PowerUps::size[current->type_][0]*2.5F;

    glColor4fv (Splot_PowerUps::color[current->type_]);
    glBindTexture (GL_TEXTURE_2D, pwrTex_);
    glPushMatrix ();
    glTranslatef (current->position_[0]+wobble_0_[current->age_%WOBBLE_0],
                  current->position_[1]+wobble_1_[current->age_%WOBBLE_1],
                  current->position_[2]);
    glRotatef ((float)IRAND, 0.0, 0.0, 1.0);
    glBegin (GL_QUADS);
    glTexCoord2f (0.0F, 0.0F); glVertex3f (-szp,  szp, 0.0F);
    glTexCoord2f (0.0F, 1.0F); glVertex3f (-szp, -szp, 0.0F);
    glTexCoord2f (1.0F, 1.0F); glVertex3f ( szp, -szp, 0.0F);
    glTexCoord2f (1.0F, 0.0F); glVertex3f ( szp,  szp, 0.0F);
    glEnd ();
    glPopMatrix ();

    current = current->get_next ();
  } // end WHILE

  glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  current = inherited::free_list_;
  while (current)
  {
    glColor4f (1.0F, 1.0F, 1.0F, 1.0F);
    glBindTexture (GL_TEXTURE_2D, tex_[current->type_]);
    glPushMatrix ();
    glTranslatef (current->position_[0]+wobble_0_[current->age_%WOBBLE_0],
                  current->position_[1]+wobble_1_[current->age_%WOBBLE_1],
                  current->position_[2]);
    glBegin (GL_QUADS);
    glTexCoord2f (0.0F, 0.0F);
    glVertex3f (-Splot_PowerUps::size[current->type_][0],  Splot_PowerUps::size[current->type_][1], 0.0F);
    glTexCoord2f (0.0F, 1.0F);
    glVertex3f (-Splot_PowerUps::size[current->type_][0], -Splot_PowerUps::size[current->type_][1], 0.0F);
    glTexCoord2f (1.0F, 1.0F);
    glVertex3f ( Splot_PowerUps::size[current->type_][0], -Splot_PowerUps::size[current->type_][1], 0.0F);
    glTexCoord2f (1.0F, 0.0F);
    glVertex3f ( Splot_PowerUps::size[current->type_][0],  Splot_PowerUps::size[current->type_][1], 0.0F);
    glEnd ();
    glPopMatrix ();

    current = current->get_next ();
  } // end WHILE

  glBlendFunc (GL_SRC_ALPHA, GL_ONE);
}

void
Splot_PowerUps::dump ()
{
  std::cerr << ACE_TEXT_ALWAYS_CHAR ("power-ups: ") << inherited::size_ << std::endl;
  Splot_PowerUp* current = inherited::free_list_;
  int i = 0;
  while (current)
  {
    std::cerr << ACE_TEXT_ALWAYS_CHAR ("#")
              << i
              << ACE_TEXT_ALWAYS_CHAR (" @: ")
              << current
              << std::endl
              << ACE_TEXT_ALWAYS_CHAR ("position: [")
              << current->position_[0]
              << ACE_TEXT_ALWAYS_CHAR (",")
              << current->position_[1]
              << ACE_TEXT_ALWAYS_CHAR (",")
              << current->position_[2]
              << ACE_TEXT_ALWAYS_CHAR ("]")
              << std::endl
              << ACE_TEXT_ALWAYS_CHAR ("prev/next: ")
              << current->prev_
              << ACE_TEXT_ALWAYS_CHAR ("/")
              << current->next_
              << std::endl;
    if (current->next_)
      std::cerr << ACE_TEXT_ALWAYS_CHAR (" --> ") << std::endl;

    i++;
    current = current->get_next ();
  } // end WHILE
}

/////////////////////////////////////////

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
                              const float (&translationVector_)[3],
                              float potency_in)
 : inherited ()
 , type_ (type_in)
{
  inherited::type_ = GAMEELEMENT_POWERUP;
  // *NOTE*: assignment doesn't work
  //inherited::position_ = position_in;
  ACE_OS::memcpy (&(inherited::position_), &position_in, sizeof (position_in));
  //inherited::velocity_ = velocity_in;
  ACE_OS::memcpy (&(inherited::translationVector_), &translationVector_, sizeof (translationVector_));

  potency_ = potency_in;

  Splot_PowerUp::count++;
}

Splot_PowerUp::~Splot_PowerUp ()
{
  Splot_PowerUp::count--;
}
