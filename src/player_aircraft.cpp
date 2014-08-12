#include "stdafx.h"

#include "player_aircraft.h"

#ifdef HAVE_CONFIG_H
#include "splot-config.h"
#endif

//#include <cstdio>
//#include <cstdlib>
//#include <cmath>

#include <string>

//#include "compatibility.h"

//#if defined(HAVE_APPLE_OPENGL_FRAMEWORK) || defined(HAVE_OPENGL_GL_H)
//#include <OpenGL/gl.h>
//#else
//#include <GL/gl.h>
//#endif

#include "ace/OS.h"
#include "ace/OS_Memory.h"
#include "ace/Log_Msg.h"

#include "defines.h"
#include "common.h"
#include "configuration.h"
#include "state.h"
#include "player_bullets.h"
#include "Explosions.h"
#include "enemy_fleet.h"
//#include "enemy_aircraft.h"
#include "powerup.h"
#include "Audio.h"
#include "StatusDisplay.h"
#include "image.h"

Splot_PlayerAircraft::Splot_PlayerAircraft ()
 : inherited (),
   heroTex_ (0),
   bombTex_ (0)
{
  inherited::type_ = GAMEELEMENT_PLAYER;

  ACE_OS::memset (&gunFlash0_, 0, sizeof (gunFlash0_));
  ACE_OS::memset (&gunFlash1_, 0, sizeof (gunFlash1_));
  ACE_OS::memset (&gunActive_, 0, sizeof (gunActive_));

  size_[0] = 0.7F;
  size_[1] = 0.85F;

  bound_[0][0] = -10.0F;
  bound_[0][1] = 10.0F;
  bound_[1][0] = -7.5F;
  bound_[1][1] = 7.5F;

  ACE_OS::memset (&deathCircle_, 0, sizeof (deathCircle_));

  ACE_OS::memset (&move_, 0, sizeof (move_));

  tipShipPast_ = 0;
  tipSuperShield_ = 0;

  reset ();

  loadTextures ();
}

Splot_PlayerAircraft::~Splot_PlayerAircraft ()
{
  deleteTextures ();
}

void
Splot_PlayerAircraft::loadTextures ()
{
  std::string path_base = ACE_TEXT_ALWAYS_CHAR (SPLOT_IMAGE_DATA_DIR);
  path_base += ACE_DIRECTORY_SEPARATOR_STR;
  std::string filename = path_base + ACE_TEXT_ALWAYS_CHAR ("hero.png");
  heroTex_ = Splot_Image::load (dataLoc (filename));
  if (!heroTex_)
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to Splot_Image::load (\"%s\"), continuing\n"),
                ACE_TEXT (filename.c_str ())));
  filename = path_base + ACE_TEXT_ALWAYS_CHAR ("superBomb.png");
  bombTex_ = Splot_Image::load (dataLoc (filename));
  if (!bombTex_)
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to Splot_Image::load (\"%s\"), continuing\n"),
                ACE_TEXT (filename.c_str ())));
}

void Splot_PlayerAircraft::deleteTextures()
{
  if (heroTex_)
    glDeleteTextures (1, &heroTex_);
  if (bombTex_)
    glDeleteTextures (1, &bombTex_);
}

//float *Splot_PlayerAircraft::getPos()
//{
//	epos[0] =  pos[0];
//	epos[1] =  pos[1];
//	epos[2] =  pos[2];
//	return epos; 
//}

void
Splot_PlayerAircraft::newGame ()
{
  reset ();
}

void
Splot_PlayerAircraft::reset ()
{
  inherited::position_[0] = 0.0F;
  inherited::position_[1] = -3.0F;
  inherited::position_[2] = HERO_Z;

  dontShow_ = 0;

  secondaryMove_[0] = secondaryMove_[1] = 0.0;

  GameState_t& game_state = SPLOT_STATE_SINGLETON::instance ()->gameState ();
  for (int i = 0; i < NUM_PLAYER_AMMO_TYPES; i++)
  {
    game_state.gun_pause[i] = -1;
    game_state.ammo_stock[i] = 0;
    // ammoStock_[i] = AMMO_REFILL;
    gunActive_[i] = false;
    gunFlash0_[i] = 0.0;
    gunFlash1_[i] = 0.0;
  } // end FOR
}

void
Splot_PlayerAircraft::fullRepair ()
{
  GameState_t& game_state = SPLOT_STATE_SINGLETON::instance ()->gameState ();
  State_t& state = SPLOT_STATE_SINGLETON::instance()->get();

  game_state.damage = PLAYER_DEFAULT_DAMAGE;
  game_state.shields = PLAYER_DEFAULT_SHIELDS;
  game_state.gun_trigger = false;
  game_state.gun_swap = false;

  float p0[3] = {10.4F,-8.30F, 25.0F };
  float v0[3] = { 0.0F, 0.08F, 0.0F };
  float clr[4] = { 1.0F, 1.0F, 1.0F, 1.0F };
  p0[0] = -10.4F;
  state.explosions->addElectric (p0, v0, clr, 0);
  state.explosions->addElectric (p0, v0, clr, 0);
  state.explosions->addElectric (p0, v0, clr, -1);
  state.explosions->addElectric (p0, v0, clr, -3);
  state.explosions->addElectric (p0, v0, clr, -4);
  p0[0] = 10.4F;
  state.explosions->addElectric(p0, v0, clr, 0);
  state.explosions->addElectric(p0, v0, clr, 0);
  state.explosions->addElectric(p0, v0, clr, -1);
  state.explosions->addElectric(p0, v0, clr, -3);
  state.explosions->addElectric(p0, v0, clr, -4);

  secondaryMove_[0] = secondaryMove_[1] = 0.0F;
}

void
Splot_PlayerAircraft::addShip (bool score_in)
{
  State_t& state = SPLOT_STATE_SINGLETON::instance ()->get ();
  GameState_t& game_state = SPLOT_STATE_SINGLETON::instance ()->gameState ();

  float p[3] = { 10.2F, 0.0F, 25.0F };
  p[1] = 7.4F-game_state.ships*state.player->size_[1];
  state.audio->playSound (Audio::AddLife, p);
  state.explosions->addExplo (Explosions::AddLife, p);
  state.explosions->addExplo (Explosions::AddLife, p, -10);
  state.explosions->addExplo (Explosions::AddLife, p, -13);

  if (score_in)
  {
    p[0] = -7.9F;
    p[1] = -8.0F;
    state.audio->playSound (Audio::AddLife, p);
    state.explosions->addExplo (Explosions::PowerBurst, p);
    state.explosions->addExplo (Explosions::ScoreLife, p, 0);
    state.explosions->addExplo (Explosions::ScoreLife, p, -3);
    state.explosions->addExplo (Explosions::ScoreLife, p, -9);
    state.explosions->addExplo (Explosions::ScoreLife, p, -15);
  } // end IF

  if (game_state.ships < MAX_PLAYER_SHIPS)
    game_state.ships++;
  else
    game_state.super_bomb_exploding = 1;
}

void
Splot_PlayerAircraft::loseShip ()
{
  State_t& state = SPLOT_STATE_SINGLETON::instance ()->get ();
  GameState_t& game_state = SPLOT_STATE_SINGLETON::instance ()->gameState ();

  float p[3] = { 10.2F, 0.0F, 25.0F };
  //-- this has to be in sync w/ StatusDisplay positions...
  game_state.ships--;
  p[1] = 7.4F-game_state.ships*state.player->size_[1];

  if (game_state.ships > -2)
  {
    state.audio->playSound (Audio::LoseLife, p);
    state.explosions->addExplo (Explosions::LoseLife, p, 0, 1.5);
    state.explosions->addExplo (Explosions::LoseLife, p, -10, 1.5);
    state.explosions->addExplo (Explosions::LoseLife, p, -13, 1.5);
    state.explosions->addExplo (Explosions::PowerBurst, p);
  } // end IF
  if (game_state.ships < 0 && state.game_mode != GAMEMODE_GAME_OVER)
  {
    game_state.damage = 0;
    game_state.shields = 0;
    startDeath ();
  } // end IF
}

void
Splot_PlayerAircraft::setShips (int ships_in)
{
  GameState_t& game_state = SPLOT_STATE_SINGLETON::instance ()->gameState ();

  int num_ships = ships_in;
  if (num_ships > MAX_PLAYER_SHIPS)
    num_ships = MAX_PLAYER_SHIPS;
  game_state.ships = num_ships;
}

void
Splot_PlayerAircraft::useItem (int index_in)
{
  GameState_t& game_state = SPLOT_STATE_SINGLETON::instance ()->gameState ();

  if (game_state.current_item_index == index_in)
  {
    useItem ();

    return;
  } // end IF

  if (index_in < MAX_PLAYER_ITEMS && index_in >= 0)
  {
    game_state.current_item_index = index_in;
    game_state.use_item_armed = 0.0;

    useItem ();
  } // end IF
}

void
Splot_PlayerAircraft::useItem ()
{
  State_t& state = SPLOT_STATE_SINGLETON::instance ()->get ();
  GameState_t& game_state = SPLOT_STATE_SINGLETON::instance ()->gameState ();

  if ((state.game_mode != GAMEMODE_GAME) ||
      game_state.super_bomb_exploding    ||
      state.game_pause)
    return;

  if (!game_state.use_item_armed)
    game_state.use_item_armed = 1.0F;
  else
  {
    game_state.use_item_armed = 0.0F;

    float v[3] = { 0.0F, 0.7F, 0.0F };
    Splot_PowerUp* pwrUp = NULL;
    switch (game_state.current_item_index)
    {
      case 0: // self destruct
        for (int i = 0; i < NUM_PLAYER_AMMO_TYPES; i++)
        {
          //eject all ammo - return remaining ammo to game->powerUps
          if (game_state.ammo_stock[i] > 1.0)
          {
            v[0] = SRAND * 0.15;
            v[1] = 0.1 + (FRAND * 0.1);
            ACE_NEW (pwrUp,
                     Splot_PowerUp ((PowerUpType_t)(i + (int)POWERUP_AMMO_0),
                                    inherited::position_,
                                    v,
                                    (game_state.ammo_stock[i] / AMMO_REFILL)));
            state.power_ups->add (pwrUp);
          } // end IF
        } // end FOR
        game_state.damage = 0.0;
        game_state.shields = 0.0;
        game_state.ships--;
        startDeath ();
        break;
      case 1: // double fire
        break;
      default:
        ACE_DEBUG ((LM_ERROR,
                    ACE_TEXT ("invalid item index (was: %d), returning\n"),
                    game_state.current_item_index));

        return;
    } // end SWITCH
  } // end ELSE
}

void
Splot_PlayerAircraft::nextItem ()
{
  GameState_t& game_state = SPLOT_STATE_SINGLETON::instance ()->gameState ();

  game_state.use_item_armed = 0.0;
  game_state.current_item_index = ++game_state.current_item_index%MAX_PLAYER_ITEMS;
}

void
Splot_PlayerAircraft::moveEvent (int x_in, int y_in)
{
  State_t& state = SPLOT_STATE_SINGLETON::instance ()->get ();
  const Configuration_t& configuration =
   SPLOT_CONFIGURATION_SINGLETON::instance ()->get ();

  if ((state.game_mode == GAMEMODE_GAME_OVER) ||
      state.game_pause)
    return;

  inherited::position_[0] +=  x_in*configuration.movement_speed;
  inherited::position_[1] += -y_in*configuration.movement_speed;
  if (inherited::position_[0] < bound_[0][0])
    inherited::position_[0] = bound_[0][0];
  else if (inherited::position_[0] > bound_[0][1])
    inherited::position_[0] = bound_[0][1];
  if (inherited::position_[1] < bound_[1][0])
    inherited::position_[1] = bound_[1][0];
  else if (inherited::position_[1] > bound_[1][1])
    inherited::position_[1] = bound_[1][1];
}

void
Splot_PlayerAircraft::ammoDamage (float damage_in, float vec_in[3])
{
  GameState_t& game_state = SPLOT_STATE_SINGLETON::instance()->gameState();
  if (game_state.super_bomb_exploding)
    return;

  float f = damage_in/50.0;
  secondaryMove_[0] = vec_in[0]*f;
  secondaryMove_[1] = vec_in[1]*f;
  float a = 2.0;
  inherited::position_[0] += vec_in[0]*f*a;
  inherited::position_[1] += vec_in[1]*f*a;
  inherited::position_[2] += vec_in[2]*f*a;

  doDamage (damage_in);
}

void
Splot_PlayerAircraft::doDamage (float damage_in)
{
  GameState_t& game_state = SPLOT_STATE_SINGLETON::instance ()->gameState ();
  if (game_state.super_bomb_exploding)
    return;

  State_t& state = SPLOT_STATE_SINGLETON::instance ()->get ();
  if (game_state.shields > PLAYER_DEFAULT_SHIELDS)
  {
    game_state.shields -= damage_in*0.25;
    state.explosions->addExplo (Explosions::HeroShields, inherited::position_);
    state.status_display->setShieldAlpha (1.0);
  } // end IF
  else if (game_state.shields > 0.0) // *TODO*: not accurate
  {
    game_state.shields -= damage_in*0.8;
    game_state.damage += damage_in*0.2;
    if (game_state.shields < 0.0)
      game_state.shields = 0.0;
    state.explosions->addExplo (Explosions::HeroShields, inherited::position_);
    state.status_display->setShieldAlpha (1.0);
    state.status_display->setDamageAlpha (1.0);
  } // end IF
  else
  {
    game_state.damage += damage_in;
    state.status_display->setDamageAlpha (1.0);
  } // end ELSE

  if (game_state.damage > 0.0)
  {
    game_state.damage = 0.0;
    game_state.ships--;
    startDeath ();
  } // end IF
}

void
Splot_PlayerAircraft::fireGun (bool status_in)
{
  if (dontShow_)
    return;

  State_t& state = SPLOT_STATE_SINGLETON::instance ()->get ();
  GameState_t& game_state = SPLOT_STATE_SINGLETON::instance ()->gameState ();
  if (status_in &&
      state.game_mode != GAMEMODE_GAME_OVER)
  {
    game_state.gun_trigger = true;
    game_state.gun_pause[0] = 0.0;
    if (gunActive_[1])
      if (game_state.gun_pause[1] < 0.0)
        game_state.gun_pause[1] = 0.0;
    if (gunActive_[2])
      if (game_state.gun_pause[2] < 0.0)
        game_state.gun_pause[2] = 0.0;
  } // end IF
  else
  {
    game_state.gun_trigger = false;
    game_state.gun_pause[0] = -1.0;
  } // end ELSE
}

void
Splot_PlayerAircraft::shootGun ()
{
  float p[3] = { 0.0F, 0.0F, inherited::position_[2] };

  State_t& state = SPLOT_STATE_SINGLETON::instance ()->get ();
  GameState_t& game_state = SPLOT_STATE_SINGLETON::instance ()->gameState ();
  if (game_state.gun_pause[0] <= 0.0F)
  {
    game_state.gun_pause[0] = 5.0F;

    p[0] = inherited::position_[0]+0.3F;
    p[1] = inherited::position_[1]+0.8F;
    state.player_bullets->addBullet (0, p);
    p[0] = inherited::position_[0]-0.3F;
    state.player_bullets->addBullet (0, p);
    if (gunActive_[0])
    {
      p[0] = inherited::position_[0]+0.45F;
      p[1] = inherited::position_[1]+0.2F;
      state.player_bullets->addBullet (0, p);
      p[0] = inherited::position_[0]-0.45F;
      state.player_bullets->addBullet (0, p);
      game_state.ammo_stock[0] -= 0.5F;
    } // end IF

    if ((game_state.current_item_index == 1) &&
        game_state.use_item_armed) // double fire
    {
      p[0] = inherited::position_[0]+0.37F;
      p[1] = inherited::position_[1]+0.6F;
      state.player_bullets->addBullet (0, p);
      p[0] = inherited::position_[0]-0.37F;
      state.player_bullets->addBullet (0, p);
      if (gunActive_[0])
      {
        p[0] = inherited::position_[0]+0.52F;
        p[1] = inherited::position_[1];
        state.player_bullets->addBullet (0, p);
        p[0] = inherited::position_[0]-0.52F;
        state.player_bullets->addBullet (0, p);
        game_state.ammo_stock[0] -= 0.75F;
      } // end IF
    } // end IF
  } // end IF

  if (gunActive_[1] &&
      (game_state.gun_pause[1] <= 0.0F))
  {
    game_state.gun_pause[1] = 25.0F;
    p[0] = inherited::position_[0];
    p[1] = inherited::position_[1]+1.1F;
    state.player_bullets->addBullet (1, p);
    game_state.ammo_stock[1] -= 1.5F;
    if ((game_state.current_item_index == 1) &&
        game_state.use_item_armed) // double fire
    {
      p[1] -= 0.2F;
      p[0] = inherited::position_[0]+0.09F;
      state.player_bullets->addBullet (1, p);
      p[0] = inherited::position_[0]-0.09F;
      state.player_bullets->addBullet (1, p);
      game_state.ammo_stock[1] -= 4.5F;
    } // end IF
  } // end IF

  if (gunActive_[2] &&
      (game_state.gun_pause[2] <= 0.0F))
  {
    game_state.gun_swap = !game_state.gun_swap;
    game_state.gun_pause[2] = 5.0F;
    p[1] = inherited::position_[1]+0.4F;
    if (game_state.gun_swap)
    {
      p[0] = inherited::position_[0]+0.7F;
      state.player_bullets->addBullet (2, p);
    } // end IF
    else
    {
      p[0] = inherited::position_[0]-0.7F;
      state.player_bullets->addBullet (2, p);
    } // end ELSE
    game_state.ammo_stock[2] -= 1.5F;
    if ((game_state.current_item_index == 1) &&
        game_state.use_item_armed) // double fire
    {
      if (game_state.gun_swap)
      {
        p[0] = inherited::position_[0]+0.85F;
        p[1] -= 0.75F;
        state.player_bullets->addBullet (2, p);
      } // end IF
      else
      {
        p[0] = inherited::position_[0]-0.85F;
        p[1] -= 0.75F;
        state.player_bullets->addBullet (2, p);
      } // end ELSE
      game_state.ammo_stock[2] -= 3.5F;
    } // end IF
  } // end IF

  //-- clean up gun active
  for (int i = 0; i < NUM_PLAYER_AMMO_TYPES; i++)
    if (game_state.ammo_stock[i] < 0.0F)
    {
      game_state.ammo_stock[i] = 0.0F;
      gunActive_[i] = false;
    } // end IF
}

void
Splot_PlayerAircraft::setAmmoStock (int index_in, float value_in)
{
  if (index_in < 0 ||
      index_in > NUM_PLAYER_AMMO_TYPES)
    return;

  State_t& state = SPLOT_STATE_SINGLETON::instance ()->get ();
  GameState_t& game_state = SPLOT_STATE_SINGLETON::instance ()->gameState ();

  state.status_display->setAmmoAlpha (1.0F);
  game_state.gun_pause[index_in] = 1.0F;
  gunActive_[index_in] = true;
  game_state.ammo_stock[index_in] = value_in;
}

void
Splot_PlayerAircraft::checkForCollisions (Splot_EnemyFleet* fleet_in)
{
  float p[3];
  float r1,r2;
  float diffX, diffY;
  float dist;
  float power;

  ACE_ASSERT (fleet_in);
  fleet_in->toFirst ();
  Splot_EnemyAircraft* enemy = NULL;
  State_t& state = SPLOT_STATE_SINGLETON::instance ()->get ();
  GameState_t& game_state = SPLOT_STATE_SINGLETON::instance ()->gameState ();
  while (enemy = fleet_in->getShip ())
  {
    diffX = inherited::position_[0]-enemy->position_[0];
    diffY = inherited::position_[1]-enemy->position_[1];
    dist = fabs(diffX) + fabs(diffY);
    if (!dontShow_ &&
        (dist < enemy->size_[0]+size_[0]))
    {
      //-- damage
      power = -enemy->damage_*0.5F;
      if (power > MAX_COLLISION_DAMAGE)
        power = MAX_COLLISION_DAMAGE;
      doDamage (power);
      if (game_state.shields > PLAYER_DEFAULT_SHIELDS)
      {
        power *= 0.5; // reduce secondary movement when super shields are enabled
        enemy->damage_ += 70.0F;
      } // end IF
      else
        enemy->damage_ += 40.0F; // normal collision

      //-- explosions
      r1 = SRAND*0.3F;
      r2 = SRAND*0.4F;
      p[0] = enemy->position_[0]+r1;
      p[1] = enemy->position_[1]+r2;
      p[2] = enemy->position_[2];
      state.explosions->addExplo (Explosions::EnemyDamage, p);
      p[0] = inherited::position_[0]+r1;
      p[1] = inherited::position_[1]+0.2F+r2;
      p[2] = inherited::position_[2];
      if (game_state.shields > 0.0F)
        state.explosions->addExplo (Explosions::HeroShields, p);
      else
        state.explosions->addExplo (Explosions::HeroDamage, p);

      secondaryMove_[0] = diffX*power*0.03F;
      secondaryMove_[1] = diffY*power*0.03F;
      enemy->secondaryMove_[0] -= diffX*enemy->collisionMove_;
      enemy->secondaryMove_[1] -= diffY*enemy->collisionMove_*0.5F;
    } // end IF

    if (game_state.super_bomb_exploding)
    {
      diffX = -enemy->position_[0];
      diffY = -15.0-enemy->position_[1];
      dist = sqrt (diffX*diffX + diffY*diffY);
      if ((dist < game_state.super_bomb_exploding*0.1F &&
           enemy->type () < ENEMYAIRCRAFT_BOSS_0) ||
          (enemy->position_[1] < -11.0F))
        enemy->damage_ += SUPER_BOMB_DAMAGE;

      if (game_state.super_bomb_exploding > 300)
        game_state.super_bomb_exploding = 0;
    } // end IF
  } // end WHILE

  if (game_state.super_bomb_exploding)
    game_state.super_bomb_exploding += 2;
}

void
Splot_PlayerAircraft::checkForPowerUps (Splot_PowerUps* powerUps_in)
{
  if (dontShow_)
    return;

  GameState_t& game_state = SPLOT_STATE_SINGLETON::instance ()->gameState ();
  if (game_state.score > game_state.score_target)
  {
    game_state.score_target += game_state.score_step;
    addShip (true);
  } // end IF

  State_t& state = SPLOT_STATE_SINGLETON::instance ()->get ();
  if (state.game_mode != GAMEMODE_GAME)
    return;

  float dist, stock;
  ACE_ASSERT (powerUps_in);
  Splot_PowerUp* pwrUp = powerUps_in->getFirst ();
  Splot_PowerUp* delUp = NULL;
  float p0[3] = {10.4F,-8.3F, 25.0F };
  float v0[3] = { 0.0F, 0.08F, 0.0F };
  float clr[4] = { 1.0F, 1.0F, 1.0F, 1.0F };
  while (pwrUp &&
         (pwrUp->type () != POWERUP_INVALID))
  {
    dist = fabs (inherited::position_[0]-pwrUp->position_[0]) +
           fabs (inherited::position_[1]-pwrUp->position_[1]);
    if (dist >= size_[1])
    {
      pwrUp = pwrUp->get_next ();
      continue;
    } // end IF

    state.audio->playSound (Audio::PowerUp, inherited::position_);
    switch (pwrUp->type ())
    {
      case POWERUP_AMMO_0:
        addScore (SCORE_POWERUP_AMMUNITION);
        stock = game_state.ammo_stock[0] + pwrUp->potency_*AMMO_REFILL;
        if (stock > AMMO_REFILL)
          stock = AMMO_REFILL;
        setAmmoStock (0, stock);
        break;
      case POWERUP_AMMO_1:
        addScore (SCORE_POWERUP_AMMUNITION);
        stock = game_state.ammo_stock[1] + pwrUp->potency_*AMMO_REFILL;
        if (stock > AMMO_REFILL)
          stock = AMMO_REFILL;
        setAmmoStock (1, stock);
        break;
      case POWERUP_AMMO_2:
        addScore (SCORE_POWERUP_AMMUNITION);
        stock = game_state.ammo_stock[2] + pwrUp->potency_*AMMO_REFILL;
        if (stock > AMMO_REFILL)
          stock = AMMO_REFILL;
        setAmmoStock (2, stock);
        break;
      case POWERUP_REPAIR:
        game_state.damage = PLAYER_DEFAULT_DAMAGE;
        state.status_display->setDamageAlpha(5.0);
        p0[0] = 10.4;
        state.explosions->addElectric (p0, v0, clr, 0);
        state.explosions->addElectric (p0, v0, clr, -1);
        state.explosions->addElectric (p0, v0, clr, -3);
        state.explosions->addElectric (p0, v0, clr, -4);
        break;
      case POWERUP_SHIELD:
        if (game_state.shields < PLAYER_DEFAULT_SHIELDS)
          game_state.shields = PLAYER_DEFAULT_SHIELDS;
        state.status_display->setShieldAlpha (5.0);
        p0[0] = -10.4;
        state.explosions->addElectric (p0, v0, clr,  0);
        state.explosions->addElectric (p0, v0, clr, -1);
        state.explosions->addElectric (p0, v0, clr, -3);
        state.explosions->addElectric (p0, v0, clr, -4);
        break;
      case POWERUP_SHIELD_SUPER:
        state.tip_super_shield++;
        game_state.damage -= game_state.shields;
        if (game_state.damage < PLAYER_DEFAULT_DAMAGE)
          game_state.damage = PLAYER_DEFAULT_DAMAGE;
        game_state.shields = PLAYER_DEFAULT_SHIELDS*2.0;
        state.status_display->setDamageAlpha (5.0);
        state.status_display->setShieldAlpha (5.0);
        p0[0] = -10.4;
        state.explosions->addElectric (p0, v0, clr, 0);
        state.explosions->addElectric (p0, v0, clr, -1);
        state.explosions->addElectric (p0, v0, clr, -3);
        state.explosions->addElectric (p0, v0, clr, -4);
        state.explosions->addElectric (p0, v0, clr, -10);
        state.explosions->addElectric (p0, v0, clr, -11);
        state.explosions->addElectric (p0, v0, clr, -13);
        state.explosions->addElectric (p0, v0, clr, -14);
        p0[0] = 10.4;
        state.explosions->addElectric (p0, v0, clr, 0);
        state.explosions->addElectric (p0, v0, clr, -1);
        state.explosions->addElectric (p0, v0, clr, -3);
        state.explosions->addElectric (p0, v0, clr, -4);
        state.explosions->addElectric (p0, v0, clr, -10);
        state.explosions->addElectric (p0, v0, clr, -11);
        state.explosions->addElectric (p0, v0, clr, -13);
        state.explosions->addElectric (p0, v0, clr, -14);
        break;
      default:
        ACE_DEBUG ((LM_ERROR,
                    ACE_TEXT ("invalid power-up type (was: %d), continuing\n"),
                    pwrUp->type ()));
        break;
    } // end SWITCH

    state.explosions->addExplo (Explosions::PowerBurst, pwrUp->position_);
    delUp = pwrUp;
    pwrUp = pwrUp->get_next ();
    powerUps_in->remove (delUp);
  } // end WHILE
}

void
Splot_PlayerAircraft::update ()
{
  State_t& state = SPLOT_STATE_SINGLETON::instance ()->get ();
  if (dontShow_ > 1)
  {
    inherited::position_[0] = cos (state.frame*0.02)*9.0;
    inherited::position_[1] = 4.0 + sin (state.frame*0.07)*2.0;
  } // end IF
  else if (dontShow_ == 1)
  {
    inherited::position_[0] = 0.0F;
    inherited::position_[1] = -3.0F;
  } // end IF

  //-- Gun flashes are drawn in StatusDisplay
  GameState_t& game_state = SPLOT_STATE_SINGLETON::instance ()->gameState ();
  if (game_state.gun_trigger)
    shootGun ();
  float flash, pause;
  for (int i = 0; i < NUM_PLAYER_AMMO_TYPES; i++)
  {
    if (game_state.gun_pause[i] >= 0.0)
      game_state.gun_pause[i] -= state.speed_adj;
    if (game_state.gun_trigger)
    {
      switch (i)
      {
        case 0:
          flash = 5.0/state.speed_adj;
          pause = game_state.gun_pause[0]/state.speed_adj;
          gunFlash0_[0] = (flash-pause)/flash;
          if (gunActive_[0])
            gunFlash1_[0] = (flash-pause)/flash;
          else
            gunFlash1_[0] = 0.0;
          break;
        case 1:
          flash = 10.0/state.speed_adj;
          pause = game_state.gun_pause[1]/state.speed_adj;
          if (gunActive_[1] &&
              (game_state.gun_pause[i] < flash))
            gunFlash0_[1] = (flash-pause)/flash;
          else
            gunFlash0_[1] = 0.0;
          break;
        case 2:
          flash = 5.0/state.speed_adj;
          pause = game_state.gun_pause[2]/state.speed_adj;
          if (gunActive_[2])
          {
            if (game_state.gun_pause[2] < flash)
            {
              if (game_state.gun_swap)
              {
                gunFlash0_[2] = (flash-pause)/flash;
                gunFlash1_[2] = 0.0;
              } // end IF
              else
              {
                gunFlash0_[2] = 0.0;
                gunFlash1_[2] = (flash-pause)/flash;
              } // end ELSE
            } // end IF
          } // end IF
          else
          {
            gunFlash0_[2] = 0.0;
            gunFlash1_[2] = 0.0;
          } // end ELSE
          break;
        default:
          ACE_DEBUG ((LM_ERROR,
                      ACE_TEXT ("invalid ammo type (was: %d), continuing\n"),
                      i));
          break;
      } // end SWITCH
    } // end IF
    else
    {
      if (gunFlash0_[i] > 0.0)
        gunFlash0_[i] -= 0.075*state.speed_adj;
      else
        gunFlash0_[i] = 0.0;
      if (gunFlash1_[i] > 0.0)
        gunFlash1_[i] -= 0.075*state.speed_adj;
      else
        gunFlash1_[i] = 0.0;
    } // end ELSE
  } // end FOR

  //-- decrement item activation
  switch (game_state.current_item_index)
  {
    case 0: // self destruct
      game_state.use_item_armed -= 0.02;
      break;
    case 1: // double fire
      if (game_state.use_item_armed)
        doDamage (1.0);
      break;
    default:
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("invalid item index (was: %d), continuing\n"),
                  game_state.current_item_index));

      break;
  } // end SWITCH
  if (game_state.use_item_armed < 0.0)
    game_state.use_item_armed = 0.0;

  //-- decrement supershields
  if (game_state.shields >= PLAYER_DEFAULT_SHIELDS)
    game_state.shields -= 0.15*state.speed_adj;

  float s = (1.0-state.speed_adj)+(state.speed_adj*0.8);
  secondaryMove_[0] *= s;
  secondaryMove_[1] *= s;
  inherited::position_[0] += secondaryMove_[0]*state.speed_adj;
  inherited::position_[1] += secondaryMove_[1]*state.speed_adj;
  moveEvent (0, 0); // --> bound checks only
}

void
Splot_PlayerAircraft::drawGL ()
{
  //-- draw player
  glPushMatrix ();
  glTranslatef (inherited::position_[0],
                inherited::position_[1],
                inherited::position_[2]);
  if (!dontShow_)
  {
    glColor4f (1.0, 1.0, 1.0, 1.0);
    glBindTexture (GL_TEXTURE_2D, heroTex_);
    drawQuad (size_[0], size_[1]);
  } // end IF
  else
    dontShow_--;

  //-- draw super shields in StatusDisplay to get better blend mode...
  glPopMatrix ();

  GameState_t& game_state = SPLOT_STATE_SINGLETON::instance ()->gameState ();
  if (game_state.super_bomb_exploding)
  {
    float s = game_state.super_bomb_exploding*0.1;
    glBlendFunc (GL_SRC_ALPHA, GL_ONE);
    glBindTexture (GL_TEXTURE_2D, bombTex_);
    glPushMatrix ();
    glTranslatef (0.0, -15.0, HERO_Z);
    glRotatef (IRAND, 0.0, 0.0, 1.0);
    drawQuad (s, s);
    glPopMatrix ();
    glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  } // end IF

  // check OpenGL error(s)
  GLenum err = GL_NO_ERROR;
  while ((err = glGetError()) != GL_NO_ERROR)
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("OpenGL error: %d, continuing\n"),
                err));
}

void
Splot_PlayerAircraft::startDeath ()
{
  deathExplosions ();

  GameState_t& game_state = SPLOT_STATE_SINGLETON::instance ()->gameState ();
  if (game_state.ships < 0)
  {
    fireGun (false);
    State_t& state = SPLOT_STATE_SINGLETON::instance ()->get ();
    state.hero_death = DEATH_TIME;
    state.game_mode = GAMEMODE_GAME_OVER;
    dontShow_ = -1;
  } // end IF
  else
  {
    game_state.super_bomb_exploding = 1;
    reset ();
    dontShow_ = 130;
  } // end ELSE
}

void
Splot_PlayerAircraft::deathExplosions ()
{
  State_t& state = SPLOT_STATE_SINGLETON::instance ()->get ();
  GameState_t& game_state = SPLOT_STATE_SINGLETON::instance ()->gameState ();

  float	p[3] = {0.0, -0.5, inherited::position_[2]};

  game_state.death_stereo = 5.0;
  p[0] = -game_state.death_stereo;
  state.audio->playSound (Audio::Explosion, p);
  state.audio->playSound (Audio::Explosion, p, -45);
  p[0] = game_state.death_stereo;
  state.audio->playSound (Audio::Explosion, p);
  state.audio->playSound (Audio::Explosion, p, -20);
  p[0] =  0.0;
  state.audio->playSound (Audio::ExploBig, p);
  state.audio->playSound (Audio::ExploPop, p);

  //-- Caclulate radius
  int i;
  for (i = 0; i < DEATH_SPIKES; i++)
  {
    double rnd = FRAND;
    double a = rnd+2.0*M_PI*((double)i/(double)DEATH_SPIKES);
    double s = 0.5+0.5*FRAND;
    deathCircle_[i][0] = sin(a)*s;
    deathCircle_[i][1] = cos(a)*s;
    p[0] = inherited::position_[0]+deathCircle_[i][0];
    p[1] = inherited::position_[1]+deathCircle_[i][1];
    p[2] = inherited::position_[2];
    state.explosions->addExplo (Explosions::EnemyDestroyed, p);
  } // end FOR

  //-- Set up explosions
  float r;
  int w, skip;
  for (i = 0; i < DEATH_TIME; i++)
  {
    w = i%DEATH_SPIKES; 
    skip = 1+(int)(3.0f*FRAND);

    //p[2] = inherited::position_[2];
    if (i < DEATH_TIME-DEATH_SPIKES)
    {
      if (!(i%skip))
      {
        r = 1.25+4.0*((float)i/(float)DEATH_TIME);
        p[0] = inherited::position_[0]+deathCircle_[w][0]*r;
        p[1] = inherited::position_[1]+deathCircle_[w][1]*r;
        state.explosions->addExplo (Explosions::EnemyDestroyed, p, -i);
      } // end IF
    } // end IF
    else
    {
      p[0] = inherited::position_[0]+deathCircle_[w][0];
      p[1] = inherited::position_[1]+deathCircle_[w][1];
      state.explosions->addExplo (Explosions::EnemyDestroyed, p, -i);
    } // end ELSE

    if (!(i%21))
    {
      game_state.death_stereo = -game_state.death_stereo*1.5;
      p[0] = game_state.death_stereo;
      p[1] = -5.0;
      state.audio->playSound (Audio::Explosion, p, -i);
    } // end IF
  } // end FOR
}
