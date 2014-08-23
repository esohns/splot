#include "stdafx.h"

#include "screen.h"

//#ifdef HAVE_CONFIG_H
//#include "splot-config.h"
//#endif

//#include "gettext.h"

//#include <cstdio>
//#include <cstdlib>

#include "ace/Assert.h"
#include "ace/OS_Memory.h"

//#include "define.h"
#include "state.h"
#include "game_element.h"
#include "enemy_aircraft.h"
#include "enemies.h"
#include "player_aircraft.h"
#include "powerup.h"

//Splot_Screen::Splot_Screen ()
//{
//  //float p[3] = { 0.0, 0.0, 0.0 };
//  //for(int i = 0; i < NumEnemyTypes; i++)
//  //{
//  ////		deadPool[i] = new EnemyAircraft((EnemyType)i, p);
//  //  deadPool[i] = EnemyAircraft::makeNewEnemy((EnemyType)i, p);
//  //  deadPool[i]->next = 0;
//  //}
//}

//Splot_Screen::~Splot_Screen ()
//{
//	clear();
//	for(int i = 0; i < NumEnemyTypes; i++)
//	{
//		delete deadPool[i];
//		deadPool[i] = NULL;
//	}
//	delete root;
//}

void
Splot_Screen::clear ()
{
  State_t& state = SPLOT_STATE_SINGLETON::instance ()->get ();
  for (ScreenElementsIterator_t iterator = state.screen_elements.begin ();
       iterator != state.screen_elements.end ();
       iterator++)
    delete (*iterator).game_element;
  state.screen_elements.clear ();
}

//void
//Splot_Screen::clearDeadPool ()
//{
//	EnemyAircraft *cur;
//	EnemyAircraft *del;
//	for(int i = 0; i < NumEnemyTypes; i++)
//	{
//		cur = deadPool[i]->next;
//		while(cur)
//		{
//			del = cur;
//			cur = cur->next;
//			delete del;
//		}
//		deadPool[i]->next = 0;
//	}
//}

void
Splot_Screen::put ()
{
  State_t& state = SPLOT_STATE_SINGLETON::instance ()->get ();
  for (ScreenElementsIterator_t iterator = state.screen_elements.begin ();
       iterator != state.screen_elements.end ();
       iterator++)
  {
    switch ((*iterator).game_element->type ())
    {
      case GAMEELEMENT_BULLET:
      case GAMEELEMENT_BONUS:
        break;
      case GAMEELEMENT_ENEMY:
      {
        Splot_EnemyAircraft* enemy_aircraft =
          dynamic_cast<Splot_EnemyAircraft*> ((*iterator).game_element);
        ACE_ASSERT (enemy_aircraft);
        state.enemies->addEnemy (enemy_aircraft);
        break;
      }
      case GAMEELEMENT_PLAYER:
        ACE_ASSERT (!state.player);
        state.player =
          dynamic_cast<Splot_PlayerAircraft*> ((*iterator).game_element);
        ACE_ASSERT (state.player);
        break;
      case GAMEELEMENT_POWERUP:
      {
        Splot_PowerUp* power_up =
          dynamic_cast<Splot_PowerUp*> ((*iterator).game_element);
        ACE_ASSERT (power_up);
        state.power_ups->add (power_up);
        break;
      }
      default:
      {
        ACE_DEBUG ((LM_ERROR,
                    ACE_TEXT ("invalid/unknown element type (was: %d), continuing\n"),
                    (*iterator).game_element->type ()));

        continue;
      }
    } // end SWITCH
  } // end FOR
}

void
Splot_Screen::remove (Splot_GameElement* element_in)
{
  State_t& state = SPLOT_STATE_SINGLETON::instance ()->get ();
  ScreenElementsIterator_t iterator = state.screen_elements.begin ();
  for (;
       iterator != state.screen_elements.end ();
       iterator++)
    if ((*iterator).game_element == element_in)
      break;
  if (iterator == state.screen_elements.end ())
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("element not found (was: %@), returning\n"),
                element_in));

    return;
  } // end IF

  delete (*iterator).game_element;
  state.screen_elements.erase (iterator);

  //switch (del->itemType())
  //{
	 // case ScreenItem::ItemEnemy: // add enemy to dead pool
		//  enemy = (EnemyAircraft*)del;
		//  enemy->back = 0;
		//  enemy->next = deadPool[enemy->type]->next;
		//  deadPool[enemy->type]->next = enemy;
		//  break;
	 // default:
		//  delete del;
		//  break;
  //}
}

bool
Splot_Screen::load ()
{
  State_t& state = SPLOT_STATE_SINGLETON::instance ()->get ();
  switch (state.game_level)
  {
    case 1:
      loadLevel1 ();
      break;
    case 2:
      loadLevel2 ();
      break;
    case 3:
      loadLevel3 ();
      break;
    case 4:
      loadLevel4 ();
      break;
    default:
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("invalid game level (was: %u), aborting\n"),
                  state.game_level));

      return false;
  } // end SWITCH

  return true;
}

void
Splot_Screen::add (int releaseTime_in,
                   Splot_GameElement* element_in)
{
  if (!element_in)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("invalid argument (was: NULL), returning\n")));

    return;
  } // end IF

  ScreenElement_t screen_element = {releaseTime_in, element_in};
  State_t& state = SPLOT_STATE_SINGLETON::instance ()->get ();
  state.screen_elements.push_back (screen_element);
}

void
Splot_Screen::add (const EnemyWave_t& wave_in)
{
  State_t& state = SPLOT_STATE_SINGLETON::instance ()->get ();
  int jitter = (int)(wave_in.jitter*(2.0-state.game_skill));
  int period = (int)(wave_in.period*(2.0-state.game_skill));
  if (jitter >= period)
    jitter = period - 1;

  int interval = 1;
  int iteration = 0;
  float position[3] = {wave_in.position[0],
                       wave_in.position[1],
                       wave_in.position[2]};
  Splot_EnemyAircraft* enemy_aircraft = NULL;
  for (int i = wave_in.begin;
       i < wave_in.end;
       i++)
  {
    interval--;
    if (interval < 1)
    {
      switch (wave_in.formation)
      {
        case FORMATION_ARROW:
          position[0] = wave_in.position[0] + wave_in.xJitter*iteration;
          enemy_aircraft = NULL;
          ACE_NEW (enemy_aircraft,
                   Splot_EnemyAircraft (wave_in.type,
                                        position,
                                        0.0));
          Splot_Screen::add (i, enemy_aircraft);
          if (iteration > 0)
          {
            position[0] = wave_in.position[0] - wave_in.xJitter*iteration;
            enemy_aircraft = NULL;
            ACE_NEW (enemy_aircraft,
                     Splot_EnemyAircraft (wave_in.type,
                                          position,
                                          0.0));
            Splot_Screen::add (i, enemy_aircraft);
          } // end IF
          interval = period + (int)(SRAND*jitter);
          iteration++;
          break;
        case FORMATION_NONE:
          position[0] = wave_in.position[0] + wave_in.xJitter*SRAND;
          enemy_aircraft = NULL;
          ACE_NEW (enemy_aircraft,
                   Splot_EnemyAircraft (wave_in.type,
                                        position,
                                        0.0));
          Splot_Screen::add (i, enemy_aircraft);
          interval = period + (int)(SRAND*jitter);
          break;
        default:
          //position[0] = 0.0;
          //enemy_aircraft = NULL;
          //ACE_NEW (enemy_aircraft,
          //         Splot_EnemyAircraft (wave_in.type,
          //                              position,
          //                              0.0));
          //Splot_Screen::add (i, enemy_aircraft);
          //interval = 25;

          ACE_DEBUG ((LM_ERROR,
                      ACE_TEXT ("unknown/invalid formation (was: %d), returning\n"),
                      wave_in.formation));

          return;
      } // end SWITCH
    } // end IF
  } // end FOR
}

Splot_EnemyAircraft*
Splot_Screen::makeAdd (EnemyAircraftType_t type_in,
                       float position_in[3],
                       int releaseTime_in)
{
  Splot_EnemyAircraft* enemy = NULL;
  //if (deadPool[et]->next)
  //{
  //  addEnemy = deadPool[et]->next;
  //  deadPool[et]->next = addEnemy->next;
  //  addEnemy->init(pos);
  //} // end IF
  //else
  ACE_NEW_RETURN (enemy,
                  Splot_EnemyAircraft (type_in, position_in),
                  NULL);
  Splot_Screen::add (releaseTime_in, enemy);

  return enemy;
}

void
Splot_Screen::loadLevel ()
{
  State_t& state = SPLOT_STATE_SINGLETON::instance ()->get ();

  int num_iterations = 12000;
  ////	int	numIterations = (int)(11100 * game->gameSkill);
  ////	int	numIterations = (int)(1100 * game->gameSkill);

  //clearDeadPool();
  //-- Enemies
  float r, d;
  int wave_duration = 500;
  int i = 600;
  Splot_Screen::addStraightWave (1, i, 0.4F);
  while (i < num_iterations-1000)
  {
    if (i < 1500)
      d = (i+250.0F)/2000.0F;
    else
      d = 1.0F;
    r = FRAND;
    if      (r < 0.15) Splot_Screen::addStraightArrowWave (i, wave_duration, d);
    else if (r < 0.25) Splot_Screen::addOmniArrowWave (i, wave_duration, d);
    else if (r > 0.60) Splot_Screen::addStraightWave (i, wave_duration, d);
    else               Splot_Screen::addOmniWave (i, wave_duration, d);

    i += wave_duration;
    wave_duration = (int)(600.0*state.game_skill) + (int)(100*SRAND);
    i += 50 + (int)(50*FRAND);
  } // end WHILE

  //-- ray gun enemy starts halfway through...
  EnemyWave_t ray_wave;
  ray_wave.type = ENEMYAIRCRAFT_RAYGUN;
  ray_wave.xJitter = 8.0;
  ray_wave.setFrequency (60, 5);
  ray_wave.setFrequency (2000, 1000);
  ray_wave.setInOut (num_iterations/2, i-1000);
  //ray_wave.setInOut (100, i-1000);
  Splot_Screen::add (ray_wave);

  //-- Boss
  EnemyWave_t boss_wave;
  boss_wave.type = ENEMYAIRCRAFT_BOSS_0;
  boss_wave.setInOut (i+75, i+1000);
  boss_wave.setPos (0.0, 15.0);
  boss_wave.xJitter = 4.0;
  boss_wave.setFrequency (5000, 0);
  Splot_Screen::add (boss_wave);

  //-- Ammunition and PowerUps
  Splot_Screen::addAmmunition (0, num_iterations + 9000);
  Splot_Screen::addPowerUps (0, num_iterations + 9000);
}

void
Splot_Screen::loadLevel1 ()
{
  loadLevel ();

//	addStraightArrowWave(051, 800);
//	addOmniArrowWave(800, 1400);
//	addStraightArrowWave(1400, 2000);
//	addOmniArrowWave(2000, 2600);
//	
//
//	addAmmunition(0, 10000);
//	addAmmunition(0, 10000);
//	addPowerUps(0, 10000);
//
//	//-- Boss
//	EnemyWave	bossWave(EnemyBoss00);
//	bossWave.setInOut(100, 1000);	
//	bossWave.setPos(0.0, 15.0);
//	bossWave.setXRand(4.0);
//	bossWave.setFrequency(5000, 0);
//	addWave(bossWave);
//	
//	EnemyWave	tankWave(EnemyTank);
//	tankWave.setPos(9.0, 11.0);
//	tankWave.setXRand(0.0);
//	tankWave.setFrequency(4000, 50);
//	tankWave.setInOut(1, 10000);
//	addWave(tankWave);
//	
//	tankWave.setPos(-9.0, 11.0);
//	tankWave.setXRand(0.0);
//	tankWave.setFrequency(4000, 50);
//	tankWave.setInOut(2001, 10000);
//	addWave(tankWave);
//	
//	
//	EnemyWave	gnatWave(EnemyGnat);
//	gnatWave.setInOut(1, 500);
//	gnatWave.setPos(FRAND*4.0, 10.0);
//	gnatWave.setFrequency(50, 40);
//	gnatWave.setXRand(5.0);
//	addWave(gnatWave);
//	
//	addAmmunition(5000);
//	addPowerUps(5000);
}

void
Splot_Screen::loadLevel2 ()
{
  int num_iterations = 14000;
  ////int	num_iterations = 1400;

  //clearDeadPool ();
  //-- Enemies
  int waves = 0;
  float r;
  int wave_duration = 500;	
  int i = 500;
  Splot_Screen::addStraightWave (100, i, 0.4F);
  while (i < num_iterations)
  {
    r = FRAND;
    waves++;

    if (waves == 5  || waves == 6  || 
        waves == 11 || waves == 12 ||
        waves == 15 || waves == 16 )
    {
      Splot_Screen::addGnatWave (i, wave_duration, 1.0, false);
    } // end IF
    else if (waves < 5)
    {
      if      (r < 0.20) Splot_Screen::addStraightArrowWave (i, wave_duration);
      else if (r < 0.30) Splot_Screen::addOmniArrowWave (i, wave_duration);
      else if (r < 0.60) Splot_Screen::addOmniWave (i, wave_duration);
      else               Splot_Screen::addStraightWave (i, wave_duration);
    } // end ELSE
    else
    {
      if      (r < 0.25) Splot_Screen::addGnatWave (i, wave_duration);
      else if (r < 0.35) Splot_Screen::addStraightArrowWave (i, wave_duration);
      else if (r < 0.50) Splot_Screen::addOmniArrowWave (i, wave_duration);
      else if (r < 0.80) Splot_Screen::addOmniWave (i, wave_duration);
      else               Splot_Screen::addStraightWave (i, wave_duration);
    } // end ELSE
    i += wave_duration;
    wave_duration = (int)(700.0 + 100.0*SRAND);
    i += 50 + (int)(50*FRAND);
  } // end WHILE

  EnemyWave_t gnat_wave;
  gnat_wave.type = ENEMYAIRCRAFT_GNAT;
  gnat_wave.setInOut (2200, 5000);
  gnat_wave.setPos (FRAND*4.0F, 10.0F);
  gnat_wave.setFrequency (150, 140);
  gnat_wave.xJitter = 5.0;
  Splot_Screen::add (gnat_wave);

  //gnat_wave.setInOut (8000, 11000);

  //-- Boss
  EnemyWave_t boss_wave;
  boss_wave.type = ENEMYAIRCRAFT_BOSS_1;
  boss_wave.setPos (0.0, 15.0);
  boss_wave.xJitter = 4.0;
  boss_wave.setFrequency (5000, 0);
  boss_wave.setInOut (num_iterations+700, num_iterations+710);
  Splot_Screen::add (boss_wave);

  //-- Ammunition and PowerUps
  Splot_Screen::addAmmunition (0, num_iterations + 9000);
  Splot_Screen::addPowerUps (0, num_iterations + 9000);
}

void
Splot_Screen::loadLevel3 ()
{
  int	num_iterations = 14000;
  //int num_iterations = 1400;

  //clearDeadPool ();
  //-- Enemies
  int waves = 0;
  float r;
  int wave_duration = 500;	
  int i = 500;
  Splot_Screen::addStraightWave (100, i, 0.5);
  while (i < num_iterations)
  {
    r = FRAND;
    waves++;

    if (waves ==  5 || 
        waves == 12 )
    {
      Splot_Screen::addGnatWave (i, wave_duration, 0.9F, false);
    } // end IF
    else if (waves ==  6 ||
             waves == 11 ||
             waves == 15 || waves == 16 )
    {
      EnemyWave_t tank_wave;
      tank_wave.type = ENEMYAIRCRAFT_TANK;
      tank_wave.setPos (0.0, 11.0);
      tank_wave.xJitter = 10.0;
      tank_wave.setFrequency (700, 100);
      tank_wave.setInOut (i+50, wave_duration-50);
      Splot_Screen::add (tank_wave);

      Splot_Screen::addStraightWave (i, 300);
    } // end ELSE
    else if (waves < 5)
    {
      if      (r < 0.20) Splot_Screen::addStraightArrowWave (i, wave_duration);
      else if (r < 0.30) Splot_Screen::addOmniArrowWave (i, wave_duration);
      else if (r < 0.60) Splot_Screen::addOmniWave (i, wave_duration);
      else               Splot_Screen::addStraightWave (i, wave_duration);
    } // end ELSE
    else
    {
      if      (r < 0.25) Splot_Screen::addGnatWave (i, wave_duration);
      else if (r < 0.35) Splot_Screen::addStraightArrowWave (i, wave_duration);
      else if (r < 0.50) Splot_Screen::addOmniArrowWave (i, wave_duration);
      else if (r < 0.80) Splot_Screen::addOmniWave (i, wave_duration);
      else               Splot_Screen::addStraightWave (i, wave_duration);
    } // end ELSE
    i += wave_duration;
    wave_duration = (int)(700.0 + 100.0*SRAND);
    i += 50 + (int)(50*FRAND);
  } // end WHILE

  EnemyWave_t gnat_wave;
  gnat_wave.type = ENEMYAIRCRAFT_GNAT;
  gnat_wave.setInOut (3000, 5000);
  gnat_wave.setPos (FRAND*4.0F, 10.0F);
  gnat_wave.setFrequency (150, 140);
  gnat_wave.xJitter = 5.0;
  Splot_Screen::add (gnat_wave);

  gnat_wave.setInOut (8000, 11000);
  Splot_Screen::add (gnat_wave);

  //-- give some extra power ups...
  Splot_Screen::addAmmunition (0, 2000, 500, 700, 0);
  Splot_Screen::addAmmunition (10000, 2000, 500, 700, 0);
  Splot_Screen::addPowerUps (3000, 2000, 2500, 2500, 0);
  Splot_Screen::addPowerUps (9500, 2000, 2500, 2500, 0);
  Splot_Screen::addPowerUps (num_iterations, 2000, 2500, 2500, 1);

  //-- Boss
  EnemyWave_t boss_wave;
  boss_wave.type = ENEMYAIRCRAFT_BOSS_1;
  boss_wave.setPos (0.0, 15.0);
  boss_wave.xJitter = 4.0;
  boss_wave.setFrequency (5000, 0);
  boss_wave.setInOut (num_iterations+700, num_iterations+710);
  Splot_Screen::add (boss_wave);

  EnemyWave_t tank_wave;
  tank_wave.type = ENEMYAIRCRAFT_TANK;
  tank_wave.setPos (0.0, 11.0);
  tank_wave.xJitter = 10.0;
  tank_wave.setFrequency (2000, 200);
  tank_wave.setInOut (num_iterations+400, num_iterations+3000);
  Splot_Screen::add (tank_wave);

  //-- Ammunition and PowerUps
  Splot_Screen::addAmmunition (0, num_iterations + 9000);
  Splot_Screen::addPowerUps (0, num_iterations + 9000);
}

void
Splot_Screen::loadLevel4 ()
{
  State_t& state = SPLOT_STATE_SINGLETON::instance ()->get ();

  int num_iterations = 12000;

  //clearDeadPool ();
  //-- Enemies
  float r, d;
  int wave_duration = 500;
  int i = 600;
  addStraightWave (1, i, 0.4F);
  while (i < num_iterations-1000)
  {
    if (i < 1500)
      d = (i+250.0F)/2000.0F;
    else
      d = 1.0F;
    r = FRAND;
    if      (r < 0.15) Splot_Screen::addStraightArrowWave (i, wave_duration, d);
    else if (r < 0.25) Splot_Screen::addOmniArrowWave (i, wave_duration, d);
    else if (r > 0.60) Splot_Screen::addStraightWave (i, wave_duration, d);
    else
    {
      if      (r < 0.25) Splot_Screen::addGnatWave (i, wave_duration);
      else if (r < 0.35) Splot_Screen::addStraightArrowWave (i, wave_duration);
      else if (r < 0.50) Splot_Screen::addOmniArrowWave (i, wave_duration);
      else if (r < 0.80) Splot_Screen::addOmniWave (i, wave_duration);
      else               Splot_Screen::addStraightWave (i, wave_duration);
    } // end ELSE

    i += wave_duration;
    wave_duration = (int)(600.0*state.game_skill)+(int)(100*SRAND);
    i += 50 + (int)(50*FRAND);
  } // end WHILE

  EnemyWave_t gnat_wave;
  gnat_wave.type = ENEMYAIRCRAFT_GNAT;
  gnat_wave.setInOut (3000, 5000);
  gnat_wave.setPos (FRAND*4.0F, 10.0F);
  gnat_wave.setFrequency (150, 140);
  gnat_wave.xJitter = 5.0;
  Splot_Screen::add (gnat_wave);

  gnat_wave.setInOut (8000, 11000);
  Splot_Screen::add (gnat_wave);

  //-- ray gun enemy starts halfway through...
  EnemyWave_t ray_wave;
  ray_wave.type = ENEMYAIRCRAFT_RAYGUN;
  ray_wave.xJitter = 8.0;
  ray_wave.setFrequency (60, 5);
  ray_wave.setFrequency (2000, 1000);
  ray_wave.setInOut (num_iterations/2, i-1000);
  Splot_Screen::add (ray_wave);

  //-- Boss
  EnemyWave_t boss_wave;
  boss_wave.type = ENEMYAIRCRAFT_BOSS_0;
  boss_wave.setInOut (i+75, i+1000);
  boss_wave.setPos (0.0, 15.0);
  boss_wave.xJitter = 4.0;
  boss_wave.setFrequency (5000, 0);
  Splot_Screen::add (boss_wave);

  //-- Ammunition and PowerUps
  Splot_Screen::addAmmunition (0, num_iterations + 9000);
  Splot_Screen::addPowerUps (0, num_iterations + 9000);
}

void
Splot_Screen::addStraightWave (int o, int duration, float density)
{
  EnemyWave_t straight_wave;
  straight_wave.type = ENEMYAIRCRAFT_STRAIGHT;
  straight_wave.xJitter = 8.0;
  straight_wave.setFrequency ((int)(60*(1.0/density)), 5);
  straight_wave.setInOut (o, o+duration);
  Splot_Screen::add (straight_wave);
}

void
Splot_Screen::addOmniWave (int o, int duration, float density)
{
  EnemyWave_t omni_wave;
  omni_wave.type = ENEMYAIRCRAFT_OMNI;
  omni_wave.xJitter = 1.0;
  omni_wave.setFrequency ((int)(39*(1.0/density)), 7);
  omni_wave.setPos ((SRAND*8.0F), 9.0F);
  omni_wave.setInOut (o, o+(duration/2)+50);
  Splot_Screen::add (omni_wave);
  omni_wave.setPos ((SRAND*8.0F), 9.0F);
  omni_wave.setInOut (o+(duration/2)-50, o+duration);
  Splot_Screen::add (omni_wave);

  EnemyWave_t straight_wave;
  straight_wave.type = ENEMYAIRCRAFT_STRAIGHT;
  straight_wave.xJitter = 8.0;
  straight_wave.setFrequency (200, 50);
  straight_wave.setInOut (o+100, o+duration);
  Splot_Screen::add (straight_wave);
}

void
Splot_Screen::addStraightArrowWave (int o, int /*duration*/, float density)
{
  float c = SRAND;

  EnemyWave_t straight_arrow;
  straight_arrow.type = ENEMYAIRCRAFT_STRAIGHT;
  straight_arrow.setInOut (o, o+130);
  straight_arrow.setPos (c, 10.0);
  straight_arrow.setFrequency ((int)(50*(1.0/density)), 0);
  straight_arrow.xJitter = 1.6F;
  straight_arrow.formation = FORMATION_ARROW;
  Splot_Screen::add (straight_arrow);

  EnemyWave_t omni_wave;
  omni_wave.type = ENEMYAIRCRAFT_OMNI;
  omni_wave.setFrequency ((int)(15*(1.0/density)), 5);
  omni_wave.setPos (c, 9.0);
  omni_wave.xJitter = 2.0F;
  omni_wave.setInOut (o+220, o+260);
  Splot_Screen::add (omni_wave);

  omni_wave.xJitter = 3.0F;
  omni_wave.setFrequency ((int)(22*(1.0/density)), 5);
  omni_wave.setInOut (o+440, o+600);
  Splot_Screen::add (omni_wave);
}

void
Splot_Screen::addOmniArrowWave (int o, int /*duration*/, float density)
{
  float c = SRAND*2.0F;

  EnemyWave_t omni_arrow;
  omni_arrow.type = ENEMYAIRCRAFT_OMNI;
  omni_arrow.formation = FORMATION_ARROW;
  omni_arrow.setPos (c, 10.0);
  omni_arrow.setFrequency ((int)(25*(1.0/density)), 0);
  omni_arrow.xJitter = 1.0;
  omni_arrow.setInOut (o+50, o+150);
  Splot_Screen::add (omni_arrow);

  omni_arrow.setInOut (o+250, o+320);
  Splot_Screen::add (omni_arrow);
  omni_arrow.setInOut (o+300, o+330);
  Splot_Screen::add (omni_arrow);
  omni_arrow.setInOut (o+350, o+470);
  Splot_Screen::add (omni_arrow);

  omni_arrow.setFrequency (5, 0);
  omni_arrow.xJitter = 1.8F;
  omni_arrow.setInOut (o+550, o+555);
  Splot_Screen::add (omni_arrow);
}

void
Splot_Screen::addGnatWave (int o, int duration, float density, bool mixed)
{
  State_t& state = SPLOT_STATE_SINGLETON::instance ()->get ();

  float c = -FRAND*3.0F;

  if (mixed)
  {
    EnemyWave_t straight_arrow;
    straight_arrow.type = ENEMYAIRCRAFT_STRAIGHT;
    straight_arrow.setInOut (o+50, o+duration);
    straight_arrow.setPos (c, 10.0);
    straight_arrow.setFrequency ((int)(90*(1.0/density)), 20);
    straight_arrow.xJitter = 8.0;
    if (c > 0.0)
    {
      Splot_Screen::add (straight_arrow);
    } // end IF
    else
    {
      EnemyWave_t omni_arrow;
      omni_arrow.type = ENEMYAIRCRAFT_OMNI;
      omni_arrow.setInOut (o+50, o+130);
      omni_arrow.setPos (c, 10.0);
      omni_arrow.setFrequency ((int)(20*(1.0/density)), 0);
      omni_arrow.xJitter = 1.1F;
      Splot_Screen::add (omni_arrow);
      omni_arrow.setInOut (o+320, o+400);
      Splot_Screen::add (omni_arrow);

      straight_arrow.setInOut (o+200, o+250);
      Splot_Screen::add (straight_arrow);
    } // end ELSE

    EnemyWave_t gnat_wave;
    gnat_wave.type = ENEMYAIRCRAFT_GNAT;
    //gnat_wave.setInOut (o, o+(17*game->gameSkill)*(1.0+FRAND*0.2));
    gnat_wave.setInOut (o, o+(int)((25*state.game_skill)*(1.0+FRAND*0.2)));
    gnat_wave.setPos (SRAND*5.0F, 10.0F);
    gnat_wave.setFrequency ((int)(3*(1.0/density)), 0);
    gnat_wave.xJitter = 3.0;
    Splot_Screen::add (gnat_wave);
  } // end IF
  else
  {
    EnemyWave_t gnat_wave;
    gnat_wave.type = ENEMYAIRCRAFT_GNAT;
    //gnat_wave.setInOut (o, o+(17*game->gameSkill)*(1.0+FRAND*0.2));
    gnat_wave.setPos (-3.0F+(SRAND*5.0F), 10.0F);
    gnat_wave.setFrequency ((int)(1*(1.0/density)), 0);
    gnat_wave.xJitter = 3.0;
    gnat_wave.setInOut (o, o+35);
    Splot_Screen::add (gnat_wave);
    gnat_wave.setPos (SRAND*5.0F, 10.0F);
    gnat_wave.setInOut (o+300, o+310);
    Splot_Screen::add (gnat_wave);
    gnat_wave.setFrequency ((int)(30*(1.0/density)), 0);
    gnat_wave.setInOut (o+300, o+400);
    Splot_Screen::add (gnat_wave);
  } // end ELSE
}

void
Splot_Screen::addAmmunition (int o, int duration, int a, int b, int c)
{
  State_t& state = SPLOT_STATE_SINGLETON::instance ()->get ();

  //-- Ammunition
//#if 0
//  int ammoPause00 = 1;
//  int ammoPause01 = 1;
//  int ammoPause02 = 1;
//#else
  int ammoPause00 = a + (int)(FRAND*200);
  int ammoPause01 = b + (int)(FRAND*200);
  int ammoPause02 = c + (int)(FRAND*500);
//#endif
  float skill = 2.0F-state.game_skill;
  float position[3] = {0.0, 9.0, 25.0};
  float velocity[3] = {0.0, 0.0, 0.0};
  Splot_PowerUp* power_up = NULL;

  for (int i = o; i < o+duration; i++)
  {
    ammoPause00--;
    ammoPause01--;
    ammoPause02--;
    position[0] = 8.0F*SRAND;
    power_up = NULL;
    if (ammoPause00 < 1)
    {
      ammoPause00 = (int)(skill*2000+(int)(SRAND*500));
      ACE_NEW_NORETURN (power_up,
                        Splot_PowerUp (POWERUP_AMMO_0, position, velocity));
      Splot_Screen::add (i, power_up);
    } // end IF
    else if (ammoPause01 < 1)
    {
      ammoPause01 = (int)(skill*2500+(int)(SRAND*1000));
      ACE_NEW_NORETURN (power_up,
                        Splot_PowerUp (POWERUP_AMMO_1, position, velocity));
      Splot_Screen::add (i, power_up);
    } // end ELSE
    else if (ammoPause02 < 1)
    {
      ammoPause02 = (int)(skill*skill*4000+(int)(SRAND*1000));
      ACE_NEW_NORETURN (power_up,
                        Splot_PowerUp (POWERUP_AMMO_2, position, velocity));
      Splot_Screen::add (i, power_up);
    } // end ELSE
  } // end FOR
}

void
Splot_Screen::addPowerUps (int o, int duration, int a, int b, int c)
{
  //-- PowerUps
//#if 0
//  int		pwrPause00 = 1;
//  int		pwrPause01 = 1;
//  int		pwrPause02 = 1;
//#else
  int pwrPause00 = a+(int)(FRAND*500);
  int pwrPause01 = b+(int)(FRAND*500);
  int pwrPause02 = c+(int)(FRAND*1500);
//#endif
  float position[3] = {0.0, 9.0, 25.0};
  float velocity[3] = {0.0, 0.0, 0.0};
  Splot_PowerUp* power_up = NULL;
  for (int i = o; i < o+duration; i++)
  {
    pwrPause00--;
    pwrPause01--;
    pwrPause02--;
    position[0] = 8.0F*SRAND;
    if (pwrPause00 < 1)
    {
      pwrPause00 = 2500+(int)(SRAND*700);
      power_up = NULL;
      ACE_NEW (power_up,
               Splot_PowerUp (POWERUP_SHIELD, position, velocity));
      Splot_Screen::add (i, power_up);
    } // end IF
    else if (pwrPause01 < 1)
    {
      pwrPause01 = 4000+(int)(SRAND*900);
      power_up = NULL;
      ACE_NEW (power_up,
               Splot_PowerUp (POWERUP_REPAIR, position, velocity));
      Splot_Screen::add (i, power_up);
    } // end ELSE
    else if (pwrPause02 < 1)
    {
      pwrPause02 = 5000+(int)(SRAND*3000);
      //			pwrPause02 = 500;
      power_up = NULL;
      ACE_NEW (power_up,
               Splot_PowerUp (POWERUP_SHIELD_SUPER, position, velocity));
      Splot_Screen::add (i, power_up);
    } // end ELSE
  } // end FOR
}
