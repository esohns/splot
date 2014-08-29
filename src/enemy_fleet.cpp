#include "stdafx.h"

#include "enemy_fleet.h"

#include <string>

#include "ace/Default_Constants.h"
#include "ace/OS_Memory.h"
#include "ace/OS.h"

//#include <cstdio>
//#include <cmath>
//
//#ifdef HAVE_CONFIG_H
//#include "splot-config.h"
//#endif
//
//#include "compatibility.h"
//
//#if defined(HAVE_APPLE_OPENGL_FRAMEWORK) || defined(HAVE_OPENGL_GL_H)
//#include <OpenGL/gl.h>
//#else
//#include <GL/gl.h>
//#endif
//
//#include "gettext.h"
#include "defines.h"
#include "common.h"
//#include "Ammo.h"
#include "Audio.h"
#include "configuration.h"
#include "state.h"
#include "explosion.h"
//#include "EnemyAmmo.h"
#include "player_aircraft.h"
#include "screen.h"
#include "StatusDisplay.h"
#include "image.h"

Splot_EnemyFleet::Splot_EnemyFleet ()
 : inherited (ACE_PURE_FREE_LIST,
              ACE_DEFAULT_FREE_LIST_PREALLOC,
              ACE_DEFAULT_FREE_LIST_LWM,
              ACE_DEFAULT_FREE_LIST_HWM,
              ACE_DEFAULT_FREE_LIST_INC)
 , currentShip_ (NULL)
{
  float p[3] = { 0.0, 0.0, 0.0 };
  ACE_NEW (inherited::free_list_,
           Splot_EnemyAircraft (ENEMYAIRCRAFT_STRAIGHT,
                                p));
  inherited::size_++;

  loadTextures ();
}

Splot_EnemyFleet::~Splot_EnemyFleet ()
{
  deleteTextures ();
  clear ();
  inherited::dealloc(1);
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
    sprintf (filename,
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
  for (int i = 0; i < MAX_ENEMYAIRCRAFT_TYPES; i++)
  {
    glDeleteTextures (1, &shipTex_[i]);
    glDeleteTextures (1, &extraTex_[i]);
  } // end FOR
}

void
Splot_EnemyFleet::clear ()
{
  currentShip_ = NULL;

  Splot_EnemyAircraft* cur = inherited::remove ();
  Splot_EnemyAircraft* del = NULL;
  while (cur &&
         (cur->type_ != ENEMYAIRCRAFT_INVALID))
  {
    del = cur;
    cur = inherited::remove ();
    Splot_Screen::remove (cur);
  } // end WHILE
}

void
Splot_EnemyFleet::drawGL ()
{
  State_t& state = SPLOT_STATE_SINGLETON::instance ()->get ();
  float szx, szy;

  glColor4f (1.0, 1.0, 1.0, 1.0);

  int num = 0;
  Splot_EnemyAircraft* thisEnemy = inherited::free_list_;
  while (thisEnemy &&
         (thisEnemy->type_ != ENEMYAIRCRAFT_INVALID))
  {
    num++;
    szx = thisEnemy->size_[0];
    szy = thisEnemy->size_[1];

    glBindTexture (GL_TEXTURE_2D, shipTex_[(int)thisEnemy->type_]);
    glColor4f (1.0, 1.0, 1.0, 1.0);
    glPushMatrix();
    glTranslatef (thisEnemy->position_[0],
                  thisEnemy->position_[1],
                  thisEnemy->position_[2]);
    glBegin (GL_TRIANGLE_STRIP);
    glTexCoord2f (1.0, 0.0); glVertex3f ( szx,  szy, 0.0);
    glTexCoord2f (0.0, 0.0); glVertex3f (-szx,  szy, 0.0);
    glTexCoord2f (1.0, 1.0); glVertex3f ( szx, -szy, 0.0);
    glTexCoord2f (0.0, 1.0); glVertex3f (-szx, -szy, 0.0);
    glEnd ();
    glPopMatrix ();
  
    switch (thisEnemy->type_)
    {
      case ENEMYAIRCRAFT_STRAIGHT:
        if (thisEnemy->preFire_)
        {
          glBlendFunc (GL_SRC_ALPHA, GL_ONE);
          glBindTexture (GL_TEXTURE_2D, extraTex_[ENEMYAIRCRAFT_STRAIGHT]);
          glColor4f (1.0, 1.0, 1.0, thisEnemy->preFire_);
          szx = 0.55F*thisEnemy->preFire_;
          glPushMatrix ();
          glTranslatef (thisEnemy->position_[0],
                        thisEnemy->position_[1]-0.9F,
                        thisEnemy->position_[2]);
          glRotatef (IRAND, 0.0, 0.0, 1.0);
          drawQuad (szx, szx+0.1F);
          glPopMatrix ();
          glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
          glColor4f (1.0, 1.0, 1.0, 1.0);
        } // end IF
        if (!((thisEnemy->age_-192)%256))
          retarget (ENEMYAIRCRAFT_GNAT, state.player);
        break;
      case ENEMYAIRCRAFT_OMNI:
        glColor4f (1.0, 0.0, 0.0, 1.0);
        glBindTexture (GL_TEXTURE_2D, extraTex_[ENEMYAIRCRAFT_OMNI]);
        glPushMatrix ();
        glTranslatef (thisEnemy->position_[0],
                      thisEnemy->position_[1],
                      thisEnemy->position_[2]);
        glRotatef ((float)-(thisEnemy->age_*8), 0.0F, 0.0F, 1.0F);
        drawQuad (szx, szy);
        glPopMatrix ();
        glColor4f (1.0F, 1.0F, 1.0F, 1.0F);
        break;
      case ENEMYAIRCRAFT_TANK:
        if (thisEnemy->preFire_)
        {
          glBlendFunc (GL_SRC_ALPHA, GL_ONE);
          glBindTexture (GL_TEXTURE_2D, extraTex_[ENEMYAIRCRAFT_TANK]);
          glColor4f (1.0F, 1.0F, 1.0F, thisEnemy->preFire_);
          glPushMatrix ();
          glTranslatef (thisEnemy->position_[0],
                        thisEnemy->position_[1]-0.63F,
                        thisEnemy->position_[2]);//NOTE: offset is ~szy*0.3
          glRotatef (IRAND, 0.0F, 0.0F, 1.0F);
          szx = 0.4F+0.6F*thisEnemy->preFire_;
          drawQuad (szx, szx);
          glPopMatrix ();
          glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
          glColor4f (1.0F, 1.0F, 1.0F, 1.0F);
        } // end IF
        break;
      case ENEMYAIRCRAFT_BOSS_0:
        if (thisEnemy->preFire_)
        {
          glBlendFunc (GL_SRC_ALPHA, GL_ONE);
          glBindTexture (GL_TEXTURE_2D, extraTex_[ENEMYAIRCRAFT_BOSS_0]);
          glColor4f (1.0F, 1.0F, 1.0F, thisEnemy->preFire_);
          szx = 0.4F+0.6F*thisEnemy->preFire_;
          glPushMatrix ();
          glTranslatef (thisEnemy->position_[0]+1.1F,
                        thisEnemy->position_[1]-0.4F,
                        thisEnemy->position_[2]);
          glRotatef (IRAND, 0.0F, 0.0F, 1.0F);
          drawQuad (szx, szx);
          glPopMatrix ();
          glPushMatrix ();
          glTranslatef (thisEnemy->position_[0]-1.1F,
                        thisEnemy->position_[1]-0.4F,
                        thisEnemy->position_[2]);
          glRotatef (IRAND, 0.0F, 0.0F, 1.0F);
          drawQuad (szx, szx);
          glPopMatrix ();
          glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
          glColor4f (1.0F, 1.0F, 1.0F, 1.0F);
        } // end IF
        break;
      case ENEMYAIRCRAFT_BOSS_1:
        if (thisEnemy->preFire_)
        {
          glBlendFunc (GL_SRC_ALPHA, GL_ONE);
          glBindTexture (GL_TEXTURE_2D, extraTex_[ENEMYAIRCRAFT_BOSS_1]);
          glColor4f (1.0F, 1.0F, 1.0F, thisEnemy->preFire_);
          szx = 0.9F*thisEnemy->preFire_;
          if (thisEnemy->shootSwap_)
          {
            glPushMatrix ();
            glTranslatef (thisEnemy->position_[0]-1.22F,
                          thisEnemy->position_[1]-1.22F,
                          thisEnemy->position_[2]);
            glRotatef (IRAND, 0.0F, 0.0F, 1.0F);
            drawQuad (szx, szx);
            drawQuad (szx+0.2F, szx+0.2F);
            glPopMatrix ();
          } // end IF
          else
          {
            glPushMatrix ();
            glTranslatef (thisEnemy->position_[0]+0.55F,
                          thisEnemy->position_[1]-1.7F,
                          thisEnemy->position_[2]);
            glRotatef (IRAND, 0.0F, 0.0F, 1.0F);
            drawQuad (szx, szx);
            drawQuad (szx+0.3F, szx+0.3F);
            glPopMatrix ();
          } // end ELSE
          glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
          glColor4f (1.0F, 1.0F, 1.0F, 1.0F);
        } // end IF
        if (!((thisEnemy->age_-272)%256))
          retarget(ENEMYAIRCRAFT_GNAT, state.player);
        break;
      default:
        ACE_DEBUG ((LM_ERROR,
                    ACE_TEXT ("invalid enemy aircraft type (was: %d), continuing\n"),
                    thisEnemy->type_));
        break;
    } // end SWITCH
    thisEnemy = thisEnemy->get_next ();
  } // end WHILE

  const Configuration_t& configuration =
    SPLOT_CONFIGURATION_SINGLETON::instance ()->get ();
  if (configuration.debug && num)
    ACE_DEBUG ((LM_DEBUG,
                ACE_TEXT ("# enemies on screen: %d\n"),
                num));
}

void
Splot_EnemyFleet::toFirst ()
{
  currentShip_ = inherited::free_list_;
  if (currentShip_->type_ == ENEMYAIRCRAFT_INVALID)
    currentShip_ = NULL;
}

Splot_EnemyAircraft*
Splot_EnemyFleet::getShip ()
{
  Splot_EnemyAircraft* return_value = currentShip_;

  if (currentShip_ &&
      (currentShip_->type_ != ENEMYAIRCRAFT_INVALID))
    currentShip_ = currentShip_->get_next ();

  return return_value;
}

void
Splot_EnemyFleet::retarget (EnemyAircraftType_t type_in,
                            Splot_GameElement* target_in)
{
  Splot_EnemyAircraft* thisEnemy = inherited::free_list_;
  while (thisEnemy &&
         (thisEnemy->type_ != ENEMYAIRCRAFT_INVALID))
  {
    if (thisEnemy->type_ == type_in)
      thisEnemy->target_ = target_in;
    thisEnemy = thisEnemy->get_next ();
  } // end WHILE
}

void
Splot_EnemyFleet::update ()
{
  State_t& state = SPLOT_STATE_SINGLETON::instance ()->get ();
  GameState_t& game_state = SPLOT_STATE_SINGLETON::instance ()->gameState ();

  //-- maintain fleet enemies
  float size, s[2], p[3], score = 0.0F;
  Splot_EnemyAircraft* prev, *tmp;
  Splot_EnemyAircraft* thisEnemy = inherited::free_list_;
  while (thisEnemy &&
         (thisEnemy->type_ != ENEMYAIRCRAFT_INVALID))
  {
    thisEnemy->update ();

    // add some damage explosions to the bosses when hit
    if (thisEnemy->type_ >= ENEMYAIRCRAFT_BOSS_0)
    {
      size = thisEnemy->size_[0]*0.7F;
      s[0] = size;
      s[1] = size;
      if ((thisEnemy->damage_ > thisEnemy->baseDamage_*0.7F) &&
          !(state.game_frame%18))
      {
        p[0] = thisEnemy->position_[0]+SRAND*s[0];
        p[1] = thisEnemy->position_[1]+SRAND*s[1];
        p[2] = thisEnemy->position_[2];
        state.explosions->add (EXPLOSION_ENEMY_DAMAGED, p, 0, 1.0);
      } // end IF
      if ((thisEnemy->damage_ > thisEnemy->baseDamage_*0.5F) &&
          !(state.game_frame%10))
      {
        p[0] = thisEnemy->position_[0]+SRAND*s[0];
        p[1] = thisEnemy->position_[1]+SRAND*s[1];
        p[2] = thisEnemy->position_[2];
        state.explosions->add (EXPLOSION_ENEMY_DAMAGED, p, 0, 1.0);
      } // end IF
      if ((thisEnemy->damage_ > thisEnemy->baseDamage_*0.3F) &&
          !(state.game_frame%4))
      {
        p[0] = thisEnemy->position_[0]+SRAND*s[0];
        p[1] = thisEnemy->position_[1]+SRAND*s[1];
        p[2] = thisEnemy->position_[2];
        state.explosions->add (EXPLOSION_ENEMY_DAMAGED, p, 0, 1.0);
      } // end IF
    } // end IF

    //-------------- remove enemies that got through

    if ((thisEnemy->position_[1] < -8.0F) &&
        (thisEnemy->type_ != ENEMYAIRCRAFT_GNAT))
      state.status_display->enemyWarning (1.0F-((thisEnemy->position_[1]+14.0F)/6.0F));
    if (thisEnemy->position_[1] < -14.0F)
    {
      thisEnemy->damage_ = 1.0F;
      thisEnemy->age_ = 0;
      state.player->loseShip ();
      state.tip_ship_past++;
    } // end IF

    //-------------- if enemies are critically damaged, destroy them
    if (thisEnemy->damage_ > 0.0F)
    {
      prev = inherited::free_list_;
      tmp = inherited::free_list_;
      do
      {
        if (tmp == thisEnemy)
          break;

        prev = tmp;
        tmp = tmp->get_next ();
      } while (true);
      tmp = thisEnemy->get_next ();
      if (prev)
        prev->set_next (tmp);
      else
        inherited::free_list_ = tmp;
      inherited::size_--;

      if (thisEnemy->age_) // *NOTE*: set age to 0 for quiet deletion...
        switch (thisEnemy->type_)
        {
          case ENEMYAIRCRAFT_BOSS_0:
          case ENEMYAIRCRAFT_BOSS_1:
            destroyAll ();
            bossExplosion (thisEnemy);

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
            state.explosions->add (EXPLOSION_ENEMY_DAMAGED, thisEnemy->position_);
            state.explosions->add (EXPLOSION_ENEMY_DAMAGED, thisEnemy->position_, -3, 0.7F);
            state.explosions->add (EXPLOSION_ENEMY_AMMUNITION_4, thisEnemy->position_);
            state.audio->play (SOUND_EXPLOSION_LIGHT, thisEnemy->position_);
            break;
          case ENEMYAIRCRAFT_RAYGUN:
            score += SCORE_KILL_RAYGUN;
            state.explosions->add (EXPLOSION_ENEMY_DESTROYED, p);
            p[0] = thisEnemy->position_[0]+0.55F;
            state.explosions->add (EXPLOSION_ENEMY_DESTROYED, p, -5, 1.5F);
            p[0] = thisEnemy->position_[0]-0.5F;
            p[1] = thisEnemy->position_[1]+0.2F;
            state.explosions->add (EXPLOSION_ENEMY_DESTROYED, p, -15);
            p[0] = thisEnemy->position_[0];
            state.explosions->add (EXPLOSION_ENEMY_DESTROYED, p, -20, 2.0F);
            state.explosions->add (EXPLOSION_ENEMY_DAMAGED, p, -30, 2.0F);
            state.audio->play (SOUND_EXPLOSION_DEFAULT, thisEnemy->position_);
            state.audio->play (SOUND_EXPLOSION_HEAVY, thisEnemy->position_);
            break;
          case ENEMYAIRCRAFT_TANK:
            score += SCORE_KILL_TANK;
            p[0] = thisEnemy->position_[0];
            p[1] = thisEnemy->position_[1];
            state.explosions->add (EXPLOSION_ENEMY_DESTROYED, p, -5, 2.5F);
            p[0] = thisEnemy->position_[0]-0.9F;
            p[1] = thisEnemy->position_[1]-1.0F;
            state.explosions->add (EXPLOSION_ENEMY_DESTROYED, p, -0, 1.5F);
            p[0] = thisEnemy->position_[0]+1.0F;
            p[1] = thisEnemy->position_[1]-0.8F;
            state.explosions->add (EXPLOSION_ENEMY_DESTROYED, p, -13, 2.0F);
            state.explosions->add (EXPLOSION_ENEMY_DESTROYED, p, -2, 1.0F);
            p[0] = thisEnemy->position_[0]+0.7F;
            p[1] = thisEnemy->position_[1]+0.7F;
            state.explosions->add (EXPLOSION_ENEMY_DESTROYED, p, -20, 1.7F);
            p[0] = thisEnemy->position_[0]-0.7F;
            p[1] = thisEnemy->position_[1]+0.9F;
            state.explosions->add (EXPLOSION_ENEMY_DESTROYED, p, -8, 1.5F);
            state.audio->play (SOUND_EXPLOSION_DEFAULT, thisEnemy->position_);
            state.audio->play (SOUND_EXPLOSION_HEAVY, thisEnemy->position_);
            break;
          case ENEMYAIRCRAFT_GNAT:
            score += SCORE_KILL_GNAT;
            state.explosions->add (EXPLOSION_ENEMY_AMMUNITION_4, thisEnemy->position_);
            state.audio->play (SOUND_EXPLOSION_LIGHT, thisEnemy->position_);
            break;
          default: //-- extra damage explosion delayed for bloom effect *TODO*
            score += SCORE_KILL_DEFAULT;
            state.explosions->add (EXPLOSION_ENEMY_DESTROYED, thisEnemy->position_);
            state.explosions->add (EXPLOSION_ENEMY_DAMAGED, thisEnemy->position_, -15);
            state.audio->play (SOUND_EXPLOSION_DEFAULT, thisEnemy->position_);
            break;
        } // end SWITCH

      Splot_Screen::remove (thisEnemy);

      thisEnemy = tmp;
    } // end IF
    else
      thisEnemy = thisEnemy->get_next ();
  } // end WHILE
  game_state.score += score;
}

void
Splot_EnemyFleet::bossExplosion (Splot_EnemyAircraft* enemy_in)
{
  ACE_ASSERT (enemy_in);

  float a, b;
  float p[3];
  State_t& state = SPLOT_STATE_SINGLETON::instance ()->get ();
  for (int i = 4; i > 0; i--)
  {
    a = enemy_in->size_[0]*(i*0.2F);
    b = enemy_in->size_[1]*(i*0.2F);
    p[0] = enemy_in->position_[0]+a*FRAND;
    p[1] = enemy_in->position_[1]+b*FRAND;
    state.explosions->add (EXPLOSION_ENEMY_DESTROYED, p, (int)(-FRAND*8.0F), 1.5F + FRAND);
    p[0] = enemy_in->position_[0]-a*FRAND;
    p[1] = enemy_in->position_[1]+b*FRAND;
    state.explosions->add (EXPLOSION_ENEMY_DESTROYED, p, (int)(-FRAND*8.0F), 1.5F + FRAND);
    p[0] = enemy_in->position_[0]+a*FRAND;
    p[1] = enemy_in->position_[1]-b*FRAND;
    state.explosions->add (EXPLOSION_ENEMY_DESTROYED, p, (int)(-FRAND*8.0F), 1.5F + FRAND);
    p[0] = enemy_in->position_[0]-a*FRAND;
    p[1] = enemy_in->position_[1]-b*FRAND;
    state.explosions->add (EXPLOSION_ENEMY_DESTROYED, p, (int)(-FRAND*8.0), 1.5F + FRAND);
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
    xsin = r*scaleX*sin (ii*a);
    ycos = r*scaleY*cos (ii*b);
    p[0] = enemy_in->position_[0]+xsin;
    p[1] = enemy_in->position_[1]+ycos;
    if (!(i%4))
      state.explosions->add (EXPLOSION_ENEMY_DESTROYED, p, (int)(-ii*2.0F), 1.5F+FRAND);
    xsin = r*scaleX*sin (i*c);
    ycos = r*scaleY*cos (i*d);
    p[0] = enemy_in->position_[0]+xsin*1.5F;
    p[1] = enemy_in->position_[1]-ycos*1.5F;
    if (!(i%5))
    {
      state.explosions->add (EXPLOSION_ENEMY_DESTROYED, p, (int)(-40.0F-(ii*3.0F)), 1.5F+FRAND);
      state.explosions->add (EXPLOSION_ENEMY_DAMAGED, p, (int)(-60.0F-(ii*3.0F)), 1.5F+FRAND);
    } // end IF
    p[0] = enemy_in->position_[0]+xsin*1.5F;
    p[1] = enemy_in->position_[1]+ycos*1.0F;
    if (!(i%3))
      state.explosions->add (EXPLOSION_ENEMY_DESTROYED, p, (int)-ii, 1.5F+FRAND);
    p[0] = enemy_in->position_[0]-ycos;
    p[1] = enemy_in->position_[1]+xsin;
    if (!(i%3))
      state.explosions->add (EXPLOSION_ENEMY_DESTROYED, p, (int)-ii, 1.5F+FRAND);
  } // end FOR
  //-- Boss Audio Explosion
  p[0] = -10.0F; p[1] = -5.0F;
  state.audio->play (SOUND_EXPLOSION_DEFAULT, p);
  state.audio->play (SOUND_EXPLOSION_DEFAULT, p, -27);
  state.audio->play (SOUND_EXPLOSION_LIGHT, p, -45);
  p[0] = 10.0F;
  state.audio->play (SOUND_EXPLOSION_DEFAULT, p, -3);
  state.audio->play (SOUND_EXPLOSION_DEFAULT, p, -25);
  state.audio->play (SOUND_EXPLOSION_LIGHT, p, -40);
  p[0] = 0.0F;
  state.audio->play (SOUND_EXPLOSION_HEAVY, p, 0);
  state.audio->play (SOUND_EXPLOSION_HEAVY, p, -60);
  state.audio->play (SOUND_EXPLOSION_LIGHT, p, -55);
  state.audio->play (SOUND_EXPLOSION_LIGHT, p, -80);
  state.audio->play (SOUND_EXPLOSION_LIGHT, p, -120);
  state.audio->play (SOUND_EXPLOSION_LIGHT, p, -160);
}

void
Splot_EnemyFleet::destroyAll ()
{
  Splot_EnemyAircraft* thisEnemy = inherited::free_list_;
  while (thisEnemy &&
         (thisEnemy->type_ != ENEMYAIRCRAFT_INVALID))
  {
    thisEnemy->damage_ = 1.0F;
    thisEnemy = thisEnemy->get_next ();
  } // end WHILE
}

void
Splot_EnemyFleet::addEnemy (Splot_EnemyAircraft* enemy_in)
{
  ACE_ASSERT (enemy_in);

  if (enemy_in->over_) // insert this enemy after 'over' aircraft in list
  {
    Splot_EnemyAircraft* tmp = inherited::free_list_;
    do
    {
      if (!tmp ||
          (tmp == enemy_in->over_))
        break;

      tmp = tmp->get_next ();
    } while (true);
    if (tmp)
    {
      enemy_in->set_next (tmp->get_next ());
      tmp->set_next (enemy_in);
      return;
    } // end IF

    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("over not found (was: %@), continuing\n"),
                enemy_in->over_));
  } // end IF

  inherited::add (enemy_in);
}

//void
//Splot_EnemyFleet::killEnemy (Splot_EnemyAircraft* enemy_in)
//{
//  ACE_ASSERT (enemy_in);
//
//  State_t& state = SPLOT_STATE_SINGLETON::instance ()->get ();
//  state.item_add->killScreenItem (enemy_in);
//}
