#include "stdafx.h"

#include "enemy_fleet.h"

#include <iostream>
#include <string>

#include "ace/Default_Constants.h"
#include "ace/OS_Memory.h"
#include "ace/OS.h"

//#include "gettext.h"
#include "defines.h"
#include "common.h"
#include "audio.h"
#include "configuration.h"
#include "state.h"
#include "explosion.h"
#include "player_aircraft.h"
#include "screen.h"
#include "status_display.h"
#include "image.h"

Splot_EnemyFleet::Splot_EnemyFleet ()
 : inherited (ACE_PURE_FREE_LIST,
              ACE_DEFAULT_FREE_LIST_PREALLOC,
              ACE_DEFAULT_FREE_LIST_LWM,
              ACE_DEFAULT_FREE_LIST_HWM,
              ACE_DEFAULT_FREE_LIST_INC)
 , currentShip_ (NULL)
{
  loadTextures ();
}

Splot_EnemyFleet::~Splot_EnemyFleet ()
{
  deleteTextures ();
  clear ();
//  inherited::dealloc(1);
  ACE_ASSERT (inherited::size_ == 0);
}

void
Splot_EnemyFleet::loadTextures ()
{
  char filename[PATH_MAX];
  ACE_OS::memset (&filename, 0, sizeof (filename));

  std::string path_base = ACE_TEXT_ALWAYS_CHAR (SPLOT_IMAGE_DATA_DIR);
  path_base += ACE_DIRECTORY_SEPARATOR_STR;
  std::string format_string = path_base;
  format_string += ACE_TEXT_ALWAYS_CHAR ("enemy%02d.png");
  for (int i = 0; i < MAX_ENEMYAIRCRAFT_TYPES; i++)
  {
    ACE_OS::sprintf (filename,
                     format_string.c_str (),
                     i);
    shipTex_[i] = Splot_Image::load (dataLoc (filename));
    if (!shipTex_[i])
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to Splot_Image::load (\"%s\"): \"%m\", continuing\n"),
                  ACE_TEXT (dataLoc (filename).c_str ())));
    extraTex_[i] = 0;
  } // end FOR

  std::string path = path_base + ACE_TEXT_ALWAYS_CHAR ("enemyAmmo00.png");
  extraTex_[ENEMYAIRCRAFT_STRAIGHT] = Splot_Image::load (dataLoc (path));
  if (!extraTex_[ENEMYAIRCRAFT_STRAIGHT])
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to Splot_Image::load (\"%s\"): \"%m\", continuing\n"),
                ACE_TEXT (dataLoc (path).c_str ())));
  path = path_base + ACE_TEXT_ALWAYS_CHAR ("enemy01-extra.png");
  extraTex_[ENEMYAIRCRAFT_OMNI] = Splot_Image::load (dataLoc (path));
  if (!extraTex_[ENEMYAIRCRAFT_OMNI])
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to Splot_Image::load (\"%s\"): \"%m\", continuing\n"),
                ACE_TEXT (dataLoc (path).c_str ())));
  path = path_base + ACE_TEXT_ALWAYS_CHAR("enemy03-extra.png");
  extraTex_[ENEMYAIRCRAFT_TANK] = Splot_Image::load (dataLoc (path));
  if (!extraTex_[ENEMYAIRCRAFT_TANK])
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to Splot_Image::load (\"%s\"): \"%m\", continuing\n"),
                ACE_TEXT (dataLoc (path).c_str ())));
  extraTex_[ENEMYAIRCRAFT_BOSS_0] = extraTex_[ENEMYAIRCRAFT_TANK];
  extraTex_[ENEMYAIRCRAFT_BOSS_1] = extraTex_[ENEMYAIRCRAFT_STRAIGHT];
}

void
Splot_EnemyFleet::deleteTextures ()
{
  glDeleteTextures (MAX_ENEMYAIRCRAFT_TYPES, &(shipTex_[0]));
  glDeleteTextures (MAX_ENEMYAIRCRAFT_TYPES, &(extraTex_[0]));
}

void
Splot_EnemyFleet::clear ()
{
  currentShip_ = NULL;

  Splot_EnemyAircraft* current = inherited::remove ();
  while (current)
  {
    Splot_Screen::remove (current);
    current = inherited::remove ();
  } // end WHILE
}

void
Splot_EnemyFleet::drawGL ()
{
  glColor4f (1.0, 1.0, 1.0, 1.0);

  State_t& state = SPLOT_STATE_SINGLETON::instance ()->get ();
  int num = 0;
  float szx;
  Splot_EnemyAircraft* current = inherited::free_list_;
  while (current)
  {
    num++;

    glBindTexture (GL_TEXTURE_2D, shipTex_[(int)current->type_]);
    glColor4f (1.0, 1.0, 1.0, 1.0);
    glPushMatrix();
    glTranslatef (current->position_[0],
                  current->position_[1],
                  current->position_[2]);
    glBegin (GL_TRIANGLE_STRIP);
    glTexCoord2f (1.0, 0.0); glVertex3f ( current->size_[0],  current->size_[1], 0.0);
    glTexCoord2f (0.0, 0.0); glVertex3f (-current->size_[0],  current->size_[1], 0.0);
    glTexCoord2f (1.0, 1.0); glVertex3f ( current->size_[0], -current->size_[1], 0.0);
    glTexCoord2f (0.0, 1.0); glVertex3f (-current->size_[0], -current->size_[1], 0.0);
    glEnd ();
    glPopMatrix ();
  
    switch (current->type_)
    {
      case ENEMYAIRCRAFT_STRAIGHT:
        if (current->preFire_)
        {
          glBlendFunc (GL_SRC_ALPHA, GL_ONE);
          glBindTexture (GL_TEXTURE_2D, extraTex_[ENEMYAIRCRAFT_STRAIGHT]);
          glColor4f (1.0, 1.0, 1.0, current->preFire_);
          szx = 0.55F*current->preFire_;
          glPushMatrix ();
          glTranslatef (current->position_[0],
                        current->position_[1]-0.9F,
                        current->position_[2]);
          glRotatef ((float)IRAND, 0.0, 0.0, 1.0);
          drawQuad (szx, szx+0.1F);
          glPopMatrix ();
          glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
          glColor4f (1.0, 1.0, 1.0, 1.0);
        } // end IF
        if (!((current->age_-192)%256))
          retarget (ENEMYAIRCRAFT_GNAT, state.player);
        break;
      case ENEMYAIRCRAFT_OMNI:
        glColor4f (1.0, 0.0, 0.0, 1.0);
        glBindTexture (GL_TEXTURE_2D, extraTex_[ENEMYAIRCRAFT_OMNI]);
        glPushMatrix ();
        glTranslatef (current->position_[0],
                      current->position_[1],
                      current->position_[2]);
        glRotatef ((float)-(current->age_*8), 0.0F, 0.0F, 1.0F);
        drawQuad (current->size_[0], current->size_[1]);
        glPopMatrix ();
        glColor4f (1.0F, 1.0F, 1.0F, 1.0F);
        break;
      case ENEMYAIRCRAFT_RAYGUN:
        break;
      case ENEMYAIRCRAFT_TANK:
        if (current->preFire_)
        {
          glBlendFunc (GL_SRC_ALPHA, GL_ONE);
          glBindTexture (GL_TEXTURE_2D, extraTex_[ENEMYAIRCRAFT_TANK]);
          glColor4f (1.0F, 1.0F, 1.0F, current->preFire_);
          glPushMatrix ();
          glTranslatef (current->position_[0],
                        current->position_[1]-0.63F,
                        current->position_[2]); //NOTE: offset is ~szy*0.3
          glRotatef ((float)IRAND, 0.0, 0.0, 1.0);
          szx = 0.4F+0.6F*current->preFire_;
          drawQuad (szx, szx);
          glPopMatrix ();
          glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
          glColor4f (1.0, 1.0, 1.0, 1.0);
        } // end IF
        break;
      case ENEMYAIRCRAFT_GNAT:
        break;
      case ENEMYAIRCRAFT_BOSS_0:
        if (current->preFire_)
        {
          glBlendFunc (GL_SRC_ALPHA, GL_ONE);
          glBindTexture (GL_TEXTURE_2D, extraTex_[ENEMYAIRCRAFT_BOSS_0]);
          glColor4f (1.0F, 1.0F, 1.0F, current->preFire_);
          szx = 0.4F+0.6F*current->preFire_;
          glPushMatrix ();
          glTranslatef (current->position_[0]+1.1F,
                        current->position_[1]-0.4F,
                        current->position_[2]);
          glRotatef ((float)IRAND, 0.0, 0.0, 1.0);
          drawQuad (szx, szx);
          glPopMatrix ();
          glPushMatrix ();
          glTranslatef (current->position_[0]-1.1F,
                        current->position_[1]-0.4F,
                        current->position_[2]);
          glRotatef ((float)IRAND, 0.0, 0.0, 1.0);
          drawQuad (szx, szx);
          glPopMatrix ();
          glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
          glColor4f (1.0, 1.0, 1.0, 1.0);
        } // end IF
        break;
      case ENEMYAIRCRAFT_BOSS_1:
        if (current->preFire_)
        {
          glBlendFunc (GL_SRC_ALPHA, GL_ONE);
          glBindTexture (GL_TEXTURE_2D, extraTex_[ENEMYAIRCRAFT_BOSS_1]);
          glColor4f (1.0F, 1.0F, 1.0F, current->preFire_);
          szx = 0.9F*current->preFire_;
          if (current->shootSwap_)
          {
            glPushMatrix ();
            glTranslatef (current->position_[0]-1.22F,
                          current->position_[1]-1.22F,
                          current->position_[2]);
            glRotatef ((float)IRAND, 0.0, 0.0, 1.0);
            drawQuad (szx, szx);
            drawQuad (szx+0.2F, szx+0.2F);
            glPopMatrix ();
          } // end IF
          else
          {
            glPushMatrix ();
            glTranslatef (current->position_[0]+0.55F,
                          current->position_[1]-1.7F,
                          current->position_[2]);
            glRotatef ((float)IRAND, 0.0, 0.0, 1.0);
            drawQuad (szx, szx);
            drawQuad (szx+0.3F, szx+0.3F);
            glPopMatrix ();
          } // end ELSE
          glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
          glColor4f (1.0, 1.0, 1.0, 1.0);
        } // end IF
        if (!((current->age_-272)%256))
          retarget(ENEMYAIRCRAFT_GNAT, state.player);
        break;
      default:
        ACE_DEBUG ((LM_ERROR,
                    ACE_TEXT ("invalid/unknown enemy aircraft type (was: %d), continuing\n"),
                    current->type_));
        break;
    } // end SWITCH
    current = current->get_next ();
  } // end WHILE

//  const Configuration_t& configuration =
//    SPLOT_CONFIGURATION_SINGLETON::instance ()->get ();
//  if (configuration.debug && num)
//    ACE_DEBUG ((LM_DEBUG,
//                ACE_TEXT ("# enemies on screen: %d\n"),
//                num));
}

void
Splot_EnemyFleet::toFirst ()
{
  currentShip_ = inherited::free_list_;
}

Splot_EnemyAircraft*
Splot_EnemyFleet::getShip ()
{
  Splot_EnemyAircraft* return_value = currentShip_;
  if (currentShip_)
    currentShip_ = currentShip_->get_next ();

  return return_value;
}

void
Splot_EnemyFleet::retarget (EnemyAircraftType_t type_in,
                            Splot_GameElement* target_in)
{
  Splot_EnemyAircraft* current = inherited::free_list_;
  while (current)
  {
    if (current->type_ == type_in)
      current->target_ = target_in;
    current = current->get_next ();
  } // end WHILE
}

void
Splot_EnemyFleet::update ()
{
  State_t& state = SPLOT_STATE_SINGLETON::instance ()->get ();
  GameState_t& game_state = SPLOT_STATE_SINGLETON::instance ()->gameState ();

  //-- maintain fleet enemies
  float tmp_size, size[2], position[3], score = 0.0F;
  int num = 0;
  Splot_EnemyAircraft* prev, *tmp;
  Splot_EnemyAircraft* current = inherited::free_list_;
  while (current)
  {
    num++;
    current->update ();

    position[0] = current->position_[0];
    position[1] = current->position_[1];
    position[2] = current->position_[2];
    // add some damage explosions to the bosses when hit
    if (current->type_ >= ENEMYAIRCRAFT_BOSS_0)
    {
      tmp_size = current->size_[0]*0.7F;
      size[0] = tmp_size;
      size[1] = tmp_size;
      if ((current->damage_ > current->baseDamage_*0.7F) &&
          !(state.game_frame%18))
      {
        position[0] = current->position_[0]+SRAND*size[0];
        position[1] = current->position_[1]+SRAND*size[1];
        //position[2] = current->position_[2];
        state.explosions->add (EXPLOSION_ENEMY_DAMAGED, position, 0, 1.0);
      } // end IF
      if ((current->damage_ > current->baseDamage_*0.5F) &&
          !(state.game_frame%10))
      {
        position[0] = current->position_[0]+SRAND*size[0];
        position[1] = current->position_[1]+SRAND*size[1];
        //position[2] = current->position_[2];
        state.explosions->add (EXPLOSION_ENEMY_DAMAGED, position, 0, 1.0);
      } // end IF
      if ((current->damage_ > current->baseDamage_*0.3F) &&
          !(state.game_frame%4))
      {
        position[0] = current->position_[0]+SRAND*size[0];
        position[1] = current->position_[1]+SRAND*size[1];
        //position[2] = current->position_[2];
        state.explosions->add (EXPLOSION_ENEMY_DAMAGED, position, 0, 1.0);
      } // end IF
    } // end IF

    //-------------- remove enemies that got through

    if ((current->position_[1] < -8.0F) &&
        (current->type_ != ENEMYAIRCRAFT_GNAT))
      state.status_display->enemyWarning (1.0F-((current->position_[1]+14.0F)/6.0F));
    if (current->position_[1] < -14.0F)
    {
      current->damage_ = 1.0F;
      current->age_ = 0;
      state.player->loseShip ();
      state.tip_ship_past++;
    } // end IF

    // if enemies are critically damaged, destroy them
    if (current->damage_ <= 0.0F)
    {
      current = current->get_next ();
      continue;
    } // end IF

    prev = NULL;
    tmp = inherited::free_list_;
    while (tmp)
    {
      if (tmp == current)
        break;

      prev = tmp;
      tmp = tmp->get_next ();
    } // end WHILE
    tmp = current->get_next ();
    if (prev)
      prev->set_next (tmp);
    else
      inherited::free_list_ = tmp;
    inherited::size_--;

    position[0] = current->position_[0];
    position[1] = current->position_[1];
    if (current->age_) // *NOTE*: set age to 0 for quiet deletion...
      switch (current->type_)
      {
        case ENEMYAIRCRAFT_BOSS_0:
        case ENEMYAIRCRAFT_BOSS_1:
          destroyAll ();
          bossExplosion (current);

          if (state.game_mode != GAMEMODE_GAME_OVER)
          {
            //-- trigger end of level
            score += SCORE_END_OF_LEVEL;
            state.game_mode = GAMEMODE_LEVEL_COMPLETE;
            state.player_success = 0;
          } // end IF
          break;
        case ENEMYAIRCRAFT_OMNI:
          score += SCORE_KILL_OMNI;
          state.explosions->add (EXPLOSION_ENEMY_DAMAGED, current->position_);
          state.explosions->add (EXPLOSION_ENEMY_DAMAGED, current->position_, -3, 0.7F);
          state.explosions->add (EXPLOSION_ENEMY_AMMUNITION_4, current->position_);
          state.audio->play (SOUND_EXPLOSION_LIGHT, current->position_);
          break;
        case ENEMYAIRCRAFT_RAYGUN:
          score += SCORE_KILL_RAYGUN;
          state.explosions->add (EXPLOSION_ENEMY_DESTROYED, current->position_);
          position[0] = current->position_[0]+0.55F;
          state.explosions->add (EXPLOSION_ENEMY_DESTROYED, position, -5, 1.5F);
          position[0] = current->position_[0]-0.5F;
          position[1] = current->position_[1]+0.2F;
          state.explosions->add (EXPLOSION_ENEMY_DESTROYED, position, -15);
          position[0] = current->position_[0];
          state.explosions->add (EXPLOSION_ENEMY_DESTROYED, position, -20, 2.0F);
          state.explosions->add (EXPLOSION_ENEMY_DAMAGED, position, -30, 2.0F);
          state.audio->play (SOUND_EXPLOSION_DEFAULT, current->position_);
          state.audio->play (SOUND_EXPLOSION_HEAVY, current->position_);
          break;
        case ENEMYAIRCRAFT_TANK:
          score += SCORE_KILL_TANK;
          state.explosions->add (EXPLOSION_ENEMY_DESTROYED, position, -5, 2.5F);
          position[0] = current->position_[0]-0.9F;
          position[1] = current->position_[1]-1.0F;
          state.explosions->add (EXPLOSION_ENEMY_DESTROYED, position, -0, 1.5F);
          position[0] = current->position_[0]+1.0F;
          position[1] = current->position_[1]-0.8F;
          state.explosions->add (EXPLOSION_ENEMY_DESTROYED, position, -13, 2.0F);
          state.explosions->add (EXPLOSION_ENEMY_DESTROYED, position, -2, 1.0F);
          position[0] = current->position_[0]+0.7F;
          position[1] = current->position_[1]+0.7F;
          state.explosions->add (EXPLOSION_ENEMY_DESTROYED, position, -20, 1.7F);
          position[0] = current->position_[0]-0.7F;
          position[1] = current->position_[1]+0.9F;
          state.explosions->add (EXPLOSION_ENEMY_DESTROYED, position, -8, 1.5F);
          state.audio->play (SOUND_EXPLOSION_DEFAULT, current->position_);
          state.audio->play (SOUND_EXPLOSION_HEAVY, current->position_);
          break;
        case ENEMYAIRCRAFT_GNAT:
          score += SCORE_KILL_GNAT;
          state.explosions->add (EXPLOSION_ENEMY_AMMUNITION_4, current->position_);
          state.audio->play (SOUND_EXPLOSION_LIGHT, current->position_);
          break;
        default: //-- extra damage explosion delayed for bloom effect *TODO*
          score += SCORE_KILL_DEFAULT;
          state.explosions->add (EXPLOSION_ENEMY_DESTROYED, current->position_);
          state.explosions->add (EXPLOSION_ENEMY_DAMAGED, current->position_, -15);
          state.audio->play (SOUND_EXPLOSION_DEFAULT, current->position_);
          break;
      } // end SWITCH

    Splot_Screen::remove (current);
    current = tmp;
  } // end WHILE
  game_state.score += score;
}

void
Splot_EnemyFleet::bossExplosion (Splot_EnemyAircraft* enemy_in)
{
  ACE_ASSERT (enemy_in);

  float a, b;
  float position[3] = {0.0, 0.0, 0.0};
  int tmp = 0;
  State_t& state = SPLOT_STATE_SINGLETON::instance ()->get ();
  for (int i = 4; i > 0; i--)
  {
    a = enemy_in->size_[0]*(i*0.2F);
    b = enemy_in->size_[1]*(i*0.2F);
    position[0] = enemy_in->position_[0]+a*FRAND;
    position[1] = enemy_in->position_[1]+b*FRAND;
    tmp = (int)(-FRAND*8.0F);
    state.explosions->add (EXPLOSION_ENEMY_DESTROYED, position, tmp, 1.5F+FRAND);
    position[0] = enemy_in->position_[0]-a*FRAND;
    position[1] = enemy_in->position_[1]+b*FRAND;
    tmp = (int)(-FRAND*8.0F);
    state.explosions->add (EXPLOSION_ENEMY_DESTROYED, position, tmp, 1.5F+FRAND);
    position[0] = enemy_in->position_[0]+a*FRAND;
    position[1] = enemy_in->position_[1]-b*FRAND;
    tmp = (int)(-FRAND*8.0F);
    state.explosions->add (EXPLOSION_ENEMY_DESTROYED, position, tmp, 1.5F+FRAND);
    position[0] = enemy_in->position_[0]-a*FRAND;
    position[1] = enemy_in->position_[1]-b*FRAND;
    tmp = (int)(-FRAND*8.0F);
    state.explosions->add (EXPLOSION_ENEMY_DESTROYED, position, tmp, 1.5F+FRAND);
  } // end FOR

  float c, d;
  float scaleX, scaleY;
  switch (enemy_in->type_)
  {
    case ENEMYAIRCRAFT_BOSS_0:
      a = 1.55F;
      b = 1.56F;
      c = 1.58F;
      d = 1.50F;
      scaleX = 4.0F;
      scaleY = 2.0F;
      break;
    case ENEMYAIRCRAFT_BOSS_1:
      a = 1.0F;
      b = 1.0F;
      c = 1.75F;
      d = 1.75F;
      scaleX = 3.5F;
      scaleY = 4.2F;
      break;
    default:
      a = b = c = d = 1.0F;
      scaleX = scaleY = 1.0F;
      break;
  } // end SWITCH
  //-- Boss Visual Explosion
  float xsin, ycos, r;
  float ii;
  for (int i = 0; i < 100; i++)
  {
    ii = i*0.5F;
    r = ii/40.0F;
    position[0] = enemy_in->position_[0]+r*scaleX*sin (ii*a);
    position[1] = enemy_in->position_[1]+r*scaleY*cos (ii*b);
    if (!(i%4))
      state.explosions->add (EXPLOSION_ENEMY_DESTROYED, position, (int)(-ii*2.0F), 1.5F+FRAND);
    xsin = r*scaleX*sin (i*c);
    ycos = r*scaleY*cos (i*d);
    position[0] = enemy_in->position_[0]+xsin*1.5F;
    position[1] = enemy_in->position_[1]-ycos*1.5F;
    if (!(i%5))
    {
      state.explosions->add (EXPLOSION_ENEMY_DESTROYED, position, (int)(-40.0F-(ii*3.0F)), 1.5F+FRAND);
      state.explosions->add (EXPLOSION_ENEMY_DAMAGED, position, (int)(-60.0F-(ii*3.0F)), 1.5F+FRAND);
    } // end IF
    position[0] = enemy_in->position_[0]+xsin*1.5F;
    position[1] = enemy_in->position_[1]+ycos*1.0F;
    if (!(i%3))
      state.explosions->add (EXPLOSION_ENEMY_DESTROYED, position, (int)-ii, 1.5F+FRAND);
    position[0] = enemy_in->position_[0]-ycos;
    position[1] = enemy_in->position_[1]+xsin;
    if (!(i%3))
      state.explosions->add (EXPLOSION_ENEMY_DESTROYED, position, (int)-ii, 1.5F+FRAND);
  } // end FOR
  //-- Boss Audio Explosion
  position[0] = -10.0F;
  position[1] = -5.0F;
  state.audio->play (SOUND_EXPLOSION_DEFAULT, position);
  state.audio->play (SOUND_EXPLOSION_DEFAULT, position, -27);
  state.audio->play (SOUND_EXPLOSION_LIGHT, position, -45);
  position[0] = 10.0F;
  state.audio->play (SOUND_EXPLOSION_DEFAULT, position, -3);
  state.audio->play (SOUND_EXPLOSION_DEFAULT, position, -25);
  state.audio->play (SOUND_EXPLOSION_LIGHT, position, -40);
  position[0] = 0.0F;
  state.audio->play (SOUND_EXPLOSION_HEAVY, position, 0);
  state.audio->play (SOUND_EXPLOSION_HEAVY, position, -60);
  state.audio->play (SOUND_EXPLOSION_LIGHT, position, -55);
  state.audio->play (SOUND_EXPLOSION_LIGHT, position, -80);
  state.audio->play (SOUND_EXPLOSION_LIGHT, position, -120);
  state.audio->play (SOUND_EXPLOSION_LIGHT, position, -160);
}

void
Splot_EnemyFleet::destroyAll ()
{
  Splot_EnemyAircraft* current = inherited::free_list_;
  while (current)
  {
    current->damage_ = 1.0F;
    current = current->get_next ();
  } // end WHILE
}

void
Splot_EnemyFleet::add (Splot_EnemyAircraft* enemy_in)
{
  ACE_ASSERT (enemy_in);

  if (enemy_in->over_) // insert this enemy after 'over' aircraft in list
  {
    Splot_EnemyAircraft* over = inherited::free_list_;
    do
    {
      if (!over ||
          (over == enemy_in->over_))
        break;

      over = over->get_next ();
    } while (true);
    if (over)
    {
      enemy_in->set_next (over->get_next ());
      over->set_next (enemy_in);

      return;
    } // end IF

    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("\"over\" element not found (was: %@), continuing\n"),
                enemy_in->over_));
  } // end IF
  else
    inherited::add (enemy_in);
}

void
Splot_EnemyFleet::dump ()
{
  std::cerr << ACE_TEXT_ALWAYS_CHAR ("enemy fleet: ") << inherited::size_ << std::endl;
  Splot_EnemyAircraft* current = inherited::free_list_;
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
