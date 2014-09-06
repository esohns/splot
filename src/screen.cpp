#include "stdafx.h"

#include "screen.h"

#include "ace/Assert.h"
#include "ace/OS_Memory.h"
#include "ace/Numeric_Limits.h"

#include "state.h"
#include "configuration.h"
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
  std::vector<ScreenElementsIterator_t> expired_elements;
  for (ScreenElementsIterator_t iterator = state.screen_elements.begin ();
       iterator != state.screen_elements.end ();
       iterator++)
  {
    if ((*iterator).release_time > state.game_frame)
      continue;

    switch ((*iterator).game_element->type_)
    {
      case GAMEELEMENT_BULLET:
      case GAMEELEMENT_BONUS:
        break;
      case GAMEELEMENT_ENEMY:
      {
        state.enemies->add ((Splot_EnemyAircraft*)(*iterator).game_element);
        break;
      }
      case GAMEELEMENT_PLAYER:
        ACE_ASSERT (!state.player);
        state.player = (Splot_PlayerAircraft*)(*iterator).game_element;
        ACE_ASSERT (state.player);
        break;
      case GAMEELEMENT_POWERUP:
      {
        state.power_ups->add ((Splot_PowerUp*)(*iterator).game_element);
        break;
      }
      default:
      {
        ACE_DEBUG ((LM_ERROR,
                    ACE_TEXT ("invalid/unknown element type (was: %d), continuing\n"),
                    (*iterator).game_element->type_));

        continue;
      }
    } // end SWITCH

    expired_elements.push_back (iterator);
  } // end FOR

  for (std::vector<ScreenElementsIterator_t>::reverse_iterator iterator = expired_elements.rbegin ();
       iterator != expired_elements.rend ();
       iterator++)
    state.screen_elements.erase (*iterator);

  //const Configuration_t& configuration =
  //  SPLOT_CONFIGURATION_SINGLETON::instance ()->get ();
  //if (configuration.debug)
  //  state.enemies->getFleet ()->dump ();
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

  delete element_in;
  if (iterator != state.screen_elements.end ())
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

unsigned int
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
  int num = 0;
  for (int i = wave_in.begin;
       i < wave_in.end;
       i++)
  {
    interval--;
    if (interval >= 1)
      continue;

    switch (wave_in.formation)
    {
      case FORMATION_ARROW:
        position[0] = wave_in.position[0]+wave_in.xJitter*iteration;
        enemy_aircraft = Splot_EnemyAircraft::make (wave_in.type,
                                                    position,
                                                    0.0);
        if (!enemy_aircraft)
        {
          ACE_DEBUG ((LM_ERROR,
                      ACE_TEXT ("failed to Splot_EnemyAircraft::make(%d), continuing\n"),
                      wave_in.type));

          break;
        } // end IF
        Splot_Screen::add (i, enemy_aircraft);
        num++;
        if (iteration > 0)
        {
          position[0] = wave_in.position[0]-wave_in.xJitter*iteration;
          enemy_aircraft = Splot_EnemyAircraft::make (wave_in.type,
                                                      position,
                                                      0.0);
          if (!enemy_aircraft)
          {
            ACE_DEBUG ((LM_ERROR,
                        ACE_TEXT ("failed to Splot_EnemyAircraft::make(%d), continuing\n"),
                        wave_in.type));

            break;
          } // end IF
          Splot_Screen::add (i, enemy_aircraft);
          num++;
        } // end IF
        interval = period+(int)(SRAND*jitter);
        iteration++;
        break;
      case FORMATION_NONE:
        position[0] = wave_in.position[0]+wave_in.xJitter*SRAND;
        enemy_aircraft = Splot_EnemyAircraft::make (wave_in.type,
                                                    position,
                                                    1.0);
        if (!enemy_aircraft)
        {
          ACE_DEBUG ((LM_ERROR,
                      ACE_TEXT ("failed to Splot_EnemyAircraft::make(%d), continuing\n"),
                      wave_in.type));

          break;
        } // end IF
        Splot_Screen::add (i, enemy_aircraft);
        num++;
        interval = period+(int)(SRAND*jitter);
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

        return num;
    } // end SWITCH
  } // end FOR

  return num;
}

bool
Splot_Screen::makeAdd (EnemyAircraftType_t type_in,
                       const float (&position_in)[3],
                       int releaseTime_in)
{
  Splot_EnemyAircraft* enemy_aircraft = Splot_EnemyAircraft::make (type_in,
                                                                   position_in,
                                                                   1.0F);
  if (!enemy_aircraft)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to Splot_EnemyAircraft::make(%d), aborting\n"),
                type_in));

    return false;
  } // end IF

  //if (deadPool[et]->next)
  //{
  //  addEnemy = deadPool[et]->next;
  //  deadPool[et]->next = addEnemy->next;
  //  addEnemy->init(pos);
  //} // end IF
  //else
  Splot_Screen::add (releaseTime_in, enemy_aircraft);

  return true;
}

void
Splot_Screen::loadLevel ()
{
  State_t& state = SPLOT_STATE_SINGLETON::instance ()->get ();
  int num_ships = 0;
  int num_iterations = 12000;
  ////	int	numIterations = (int)(11100 * game->gameSkill);
  ////	int	numIterations = (int)(1100 * game->gameSkill);

  //clearDeadPool();
  //-- Enemies
  float r, d;
  int wave_duration = 500;
  int i = 600;
  if (!DEBUG_NO_ENEMIES)
    num_ships += Splot_Screen::addStraightWave (1, i, 0.4F);
  while (i < num_iterations-1000)
  {
    if (i < 1500)
      d = (i+250.0F)/2000.0F;
    else
      d = 1.0F;
    r = FRAND;
    if (!DEBUG_NO_ENEMIES)
    {
      if      (r < 0.15) num_ships += Splot_Screen::addStraightArrowWave (i, wave_duration, d);
      else if (r < 0.25) num_ships += Splot_Screen::addOmniArrowWave (i, wave_duration, d);
      else if (r > 0.60) num_ships += Splot_Screen::addStraightWave (i, wave_duration, d);
      else               num_ships += Splot_Screen::addOmniWave (i, wave_duration, d);
    } // end IF
    i += wave_duration;
    wave_duration = (int)(600.0*state.game_skill)+(int)(100*SRAND);
    i += 50+(int)(50*FRAND);
  } // end WHILE

  //-- ray gun enemy starts halfway through...
  EnemyWave_t wave;
  wave.type = ENEMYAIRCRAFT_RAYGUN;
  wave.xJitter = 8.0;
  wave.setFrequency (60, 5);
  wave.setFrequency (2000, 1000);
  wave.setInOut (num_iterations/2, i-1000);
  //wave.setInOut (100, i-1000);
  if (!DEBUG_NO_ENEMIES)
    num_ships += Splot_Screen::add (wave);

  //-- Boss
  wave.type = ENEMYAIRCRAFT_BOSS_0;
  wave.setInOut (i+75, i+1000);
  wave.setPos (0.0, 15.0);
  wave.xJitter = 4.0;
  wave.setFrequency (5000, 0);
  if (!DEBUG_NO_ENEMIES)
    num_ships += Splot_Screen::add (wave);

  //-- Ammunition and PowerUps
  if (!DEBUG_NO_AMMUNITION)
    Splot_Screen::addPowerUps (POWERUPCATEGORY_AMMUNITION,    0, num_iterations+9000);
  if (!DEBUG_NO_POWERUPS)
    Splot_Screen::addPowerUps (POWERUPCATEGORY_REPAIR_SHIELD, 0, num_iterations+9000);

  //ACE_DEBUG ((LM_DEBUG,
  //            ACE_TEXT ("added %u enemy aircraft(s)\n"),
  //            num_ships));
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
  int num_ships = 0;
  int num_iterations = 14000;
  ////int	num_iterations = 1400;

  //clearDeadPool ();
  //-- Enemies
  int waves = 0;
  float r;
  int wave_duration = 500;	
  int i = 500;
  if (!DEBUG_NO_ENEMIES)
    num_ships += Splot_Screen::addStraightWave (100, i, 0.4F);
  while (i < num_iterations)
  {
    r = FRAND;
    waves++;

    if (waves == 5  || waves == 6  || 
        waves == 11 || waves == 12 ||
        waves == 15 || waves == 16 )
    {
      if (!DEBUG_NO_ENEMIES)
        num_ships += Splot_Screen::addGnatWave (i, wave_duration, 1.0, false);
    } // end IF
    else if (waves < 5)
    {
      if (!DEBUG_NO_ENEMIES)
      {
        if      (r < 0.20) num_ships += Splot_Screen::addStraightArrowWave (i, wave_duration);
        else if (r < 0.30) num_ships += Splot_Screen::addOmniArrowWave (i, wave_duration);
        else if (r < 0.60) num_ships += Splot_Screen::addOmniWave (i, wave_duration);
        else               num_ships += Splot_Screen::addStraightWave (i, wave_duration);
      } // end IF
    } // end ELSE
    else
    {
      if (!DEBUG_NO_ENEMIES)
      {
        if      (r < 0.25) num_ships += Splot_Screen::addGnatWave (i, wave_duration);
        else if (r < 0.35) num_ships += Splot_Screen::addStraightArrowWave (i, wave_duration);
        else if (r < 0.50) num_ships += Splot_Screen::addOmniArrowWave (i, wave_duration);
        else if (r < 0.80) num_ships += Splot_Screen::addOmniWave (i, wave_duration);
        else               num_ships += Splot_Screen::addStraightWave (i, wave_duration);
      } // end IF
    } // end ELSE
    i += wave_duration;
    wave_duration = (int)(700.0+100.0*SRAND);
    i += 50+(int)(50*FRAND);
  } // end WHILE

  EnemyWave_t wave;
  wave.type = ENEMYAIRCRAFT_GNAT;
  wave.setInOut (2200, 5000);
  wave.setPos (FRAND*4.0F, 10.0F);
  wave.setFrequency (150, 140);
  wave.xJitter = 5.0;
  if (!DEBUG_NO_ENEMIES)
    num_ships += Splot_Screen::add (wave);
  //wave.setInOut (8000, 11000);

  //-- Boss
  wave.type = ENEMYAIRCRAFT_BOSS_1;
  wave.setPos (0.0, 15.0);
  wave.xJitter = 4.0;
  wave.setFrequency (5000, 0);
  wave.setInOut (num_iterations+700, num_iterations+710);
  if (!DEBUG_NO_ENEMIES)
    num_ships += Splot_Screen::add (wave);

  //-- Ammunition and PowerUps
  if (!DEBUG_NO_AMMUNITION)
    Splot_Screen::addPowerUps (POWERUPCATEGORY_AMMUNITION,    0, num_iterations+9000);
  if (!DEBUG_NO_POWERUPS)
    Splot_Screen::addPowerUps (POWERUPCATEGORY_REPAIR_SHIELD, 0, num_iterations+9000);

  //ACE_DEBUG ((LM_DEBUG,
  //            ACE_TEXT ("added %u enemy aircraft(s)\n"),
  //            num_ships));
}

void
Splot_Screen::loadLevel3 ()
{
  int num_ships = 0;
  int	num_iterations = 14000;
  //int num_iterations = 1400;

  //clearDeadPool ();
  //-- Enemies
  int waves = 0;
  float r;
  int wave_duration = 500;	
  int i = 500;
  if (!DEBUG_NO_ENEMIES)
    num_ships += Splot_Screen::addStraightWave (100, i, 0.5);
  while (i < num_iterations)
  {
    r = FRAND;
    waves++;

    if (waves ==  5 || 
        waves == 12 )
    {
      if (!DEBUG_NO_ENEMIES)
        num_ships += Splot_Screen::addGnatWave (i, wave_duration, 0.9F, false);
    } // end IF
    else if (waves ==  6 ||
             waves == 11 ||
             waves == 15 || waves == 16 )
    {
      EnemyWave_t wave;
      wave.type = ENEMYAIRCRAFT_TANK;
      wave.setPos (0.0, 11.0);
      wave.xJitter = 10.0;
      wave.setFrequency (700, 100);
      wave.setInOut (i+50, wave_duration-50);
      if (!DEBUG_NO_ENEMIES)
        num_ships += Splot_Screen::add (wave);
      if (!DEBUG_NO_ENEMIES)
        num_ships += Splot_Screen::addStraightWave (i, 300);
    } // end ELSE
    else if (waves < 5)
    {
      if (!DEBUG_NO_ENEMIES)
      {
        if      (r < 0.20) num_ships += Splot_Screen::addStraightArrowWave (i, wave_duration);
        else if (r < 0.30) num_ships += Splot_Screen::addOmniArrowWave (i, wave_duration);
        else if (r < 0.60) num_ships += Splot_Screen::addOmniWave (i, wave_duration);
        else               num_ships += Splot_Screen::addStraightWave (i, wave_duration);
      } // end IF
    } // end ELSE
    else
    {
      if (!DEBUG_NO_ENEMIES)
      {
        if      (r < 0.25) num_ships += Splot_Screen::addGnatWave (i, wave_duration);
        else if (r < 0.35) num_ships += Splot_Screen::addStraightArrowWave (i, wave_duration);
        else if (r < 0.50) num_ships += Splot_Screen::addOmniArrowWave (i, wave_duration);
        else if (r < 0.80) num_ships += Splot_Screen::addOmniWave (i, wave_duration);
        else               num_ships += Splot_Screen::addStraightWave (i, wave_duration);
      } // end IF
    } // end ELSE
    i += wave_duration;
    wave_duration = (int)(700.0+100.0*SRAND);
    i += 50+(int)(50*FRAND);
  } // end WHILE

  EnemyWave_t wave;
  wave.type = ENEMYAIRCRAFT_GNAT;
  wave.setInOut (3000, 5000);
  wave.setPos (FRAND*4.0F, 10.0F);
  wave.setFrequency (150, 140);
  wave.xJitter = 5.0;
  if (!DEBUG_NO_ENEMIES)
    num_ships += Splot_Screen::add (wave);

  wave.setInOut (8000, 11000);
  if (!DEBUG_NO_ENEMIES)
    num_ships += Splot_Screen::add (wave);

  //-- give some extra power ups...
  if (!DEBUG_NO_AMMUNITION)
  {
    Splot_Screen::addPowerUps (POWERUPCATEGORY_AMMUNITION, 0, 2000, 500, 700, 0);
    Splot_Screen::addPowerUps (POWERUPCATEGORY_AMMUNITION, 10000, 2000, 500, 700, 0);
  } // end IF
  if (!DEBUG_NO_POWERUPS)
  {
    Splot_Screen::addPowerUps (POWERUPCATEGORY_REPAIR_SHIELD, 3000, 2000, 2500, 2500, 0);
    Splot_Screen::addPowerUps (POWERUPCATEGORY_REPAIR_SHIELD, 9500, 2000, 2500, 2500, 0);
    Splot_Screen::addPowerUps (POWERUPCATEGORY_REPAIR_SHIELD, num_iterations, 2000, 2500, 2500, 1);
  } // end IF

  //-- Boss
  wave.type = ENEMYAIRCRAFT_BOSS_1;
  wave.setPos (0.0, 15.0);
  wave.xJitter = 4.0;
  wave.setFrequency (5000, 0);
  wave.setInOut (num_iterations+700, num_iterations+710);
  if (!DEBUG_NO_ENEMIES)
    num_ships += Splot_Screen::add (wave);

  wave.type = ENEMYAIRCRAFT_TANK;
  wave.setPos (0.0, 11.0);
  wave.xJitter = 10.0;
  wave.setFrequency (2000, 200);
  wave.setInOut (num_iterations+400, num_iterations+3000);
  if (!DEBUG_NO_ENEMIES)
    num_ships += Splot_Screen::add (wave);

  //-- Ammunition and PowerUps
  if (!DEBUG_NO_AMMUNITION)
    Splot_Screen::addPowerUps (POWERUPCATEGORY_AMMUNITION,    0, num_iterations+9000);
  if (!DEBUG_NO_POWERUPS)
    Splot_Screen::addPowerUps (POWERUPCATEGORY_REPAIR_SHIELD, 0, num_iterations+9000);

  //ACE_DEBUG ((LM_DEBUG,
  //            ACE_TEXT ("added %u enemy aircraft(s)\n"),
  //            num_ships));
}

void
Splot_Screen::loadLevel4 ()
{
  State_t& state = SPLOT_STATE_SINGLETON::instance ()->get ();
  int num_ships = 0;
  int num_iterations = 12000;

  //clearDeadPool ();
  //-- Enemies
  float r, d;
  int wave_duration = 500;
  int i = 600;
  if (!DEBUG_NO_ENEMIES)
    num_ships += Splot_Screen::addStraightWave (1, i, 0.4F);
  while (i < num_iterations-1000)
  {
    if (i < 1500)
      d = (i+250.0F)/2000.0F;
    else
      d = 1.0F;
    r = FRAND;
    if (!DEBUG_NO_ENEMIES)
    {
      if      (r < 0.15) num_ships += Splot_Screen::addStraightArrowWave (i, wave_duration, d);
      else if (r < 0.25) num_ships += Splot_Screen::addOmniArrowWave (i, wave_duration, d);
      else if (r > 0.60) num_ships += Splot_Screen::addStraightWave (i, wave_duration, d);
      else
      {
        if      (r < 0.25) num_ships += Splot_Screen::addGnatWave (i, wave_duration);
        else if (r < 0.35) num_ships += Splot_Screen::addStraightArrowWave (i, wave_duration);
        else if (r < 0.50) num_ships += Splot_Screen::addOmniArrowWave (i, wave_duration);
        else if (r < 0.80) num_ships += Splot_Screen::addOmniWave (i, wave_duration);
        else               num_ships += Splot_Screen::addStraightWave (i, wave_duration);
      } // end ELSE
    } // end IF
    i += wave_duration;
    wave_duration = (int)(600.0*state.game_skill)+(int)(100*SRAND);
    i += 50 + (int)(50*FRAND);
  } // end WHILE

  EnemyWave_t wave;
  wave.type = ENEMYAIRCRAFT_GNAT;
  wave.setInOut (3000, 5000);
  wave.setPos (FRAND*4.0F, 10.0F);
  wave.setFrequency (150, 140);
  wave.xJitter = 5.0;
  if (!DEBUG_NO_ENEMIES)
    num_ships += Splot_Screen::add (wave);

  wave.setInOut (8000, 11000);
  if (!DEBUG_NO_ENEMIES)
    num_ships += Splot_Screen::add (wave);

  //-- ray gun enemy starts halfway through...
  wave.type = ENEMYAIRCRAFT_RAYGUN;
  wave.xJitter = 8.0;
  wave.setFrequency (60, 5);
  wave.setFrequency (2000, 1000);
  wave.setInOut (num_iterations/2, i-1000);
  if (!DEBUG_NO_ENEMIES)
    num_ships += Splot_Screen::add (wave);

  //-- Boss
  wave.type = ENEMYAIRCRAFT_BOSS_0;
  wave.setInOut (i+75, i+1000);
  wave.setPos (0.0, 15.0);
  wave.xJitter = 4.0;
  wave.setFrequency (5000, 0);
  if (!DEBUG_NO_ENEMIES)
    num_ships += Splot_Screen::add (wave);

  //-- Ammunition and PowerUps
  if (!DEBUG_NO_AMMUNITION)
    Splot_Screen::addPowerUps (POWERUPCATEGORY_AMMUNITION,    0, num_iterations+9000);
  if (!DEBUG_NO_POWERUPS)
    Splot_Screen::addPowerUps (POWERUPCATEGORY_REPAIR_SHIELD, 0, num_iterations+9000);

  //ACE_DEBUG ((LM_DEBUG,
  //            ACE_TEXT ("added %u enemy aircraft(s)\n"),
  //            num_ships));
}

unsigned int
Splot_Screen::addStraightWave (int o, int duration, float density)
{
  EnemyWave_t straight_wave;
  straight_wave.type = ENEMYAIRCRAFT_STRAIGHT;
  straight_wave.xJitter = 8.0;
  straight_wave.setFrequency ((int)(60*(1.0/density)), 5);
  straight_wave.setInOut (o, o+duration);
  return Splot_Screen::add (straight_wave);
}

unsigned int
Splot_Screen::addOmniWave (int o, int duration, float density)
{
  int num_ships = 0;

  EnemyWave_t wave;
  wave.type = ENEMYAIRCRAFT_OMNI;
  wave.xJitter = 1.0;
  wave.setFrequency ((int)(39*(1.0/density)), 7);
  wave.setPos ((SRAND*8.0F), 9.0F);
  wave.setInOut (o, o+(duration/2)+50);
  num_ships += Splot_Screen::add (wave);

  wave.setPos ((SRAND*8.0F), 9.0F);
  wave.setInOut (o+(duration/2)-50, o+duration);
  num_ships += Splot_Screen::add (wave);

  wave.type = ENEMYAIRCRAFT_STRAIGHT;
  wave.xJitter = 8.0;
  wave.setFrequency (200, 50);
  wave.setInOut (o+100, o+duration);
  num_ships += Splot_Screen::add (wave);

  return num_ships;
}

unsigned int
Splot_Screen::addStraightArrowWave (int o, int /*duration*/, float density)
{
  int num_ships = 0;
  float c = SRAND;

  EnemyWave_t wave;
  wave.type = ENEMYAIRCRAFT_STRAIGHT;
  wave.setInOut (o, o+130);
  wave.setPos (c, 10.0);
  wave.setFrequency ((int)(50*(1.0/density)), 0);
  wave.xJitter = 1.6F;
  wave.formation = FORMATION_ARROW;
  num_ships += Splot_Screen::add (wave);

  wave.type = ENEMYAIRCRAFT_OMNI;
  wave.setFrequency ((int)(15*(1.0/density)), 5);
  wave.setPos (c, 9.0);
  wave.xJitter = 2.0F;
  wave.setInOut (o+220, o+260);
  num_ships += Splot_Screen::add (wave);

  wave.xJitter = 3.0F;
  wave.setFrequency ((int)(22*(1.0/density)), 5);
  wave.setInOut (o+440, o+600);
  num_ships += Splot_Screen::add (wave);

  return num_ships;
}

unsigned int
Splot_Screen::addOmniArrowWave (int o, int /*duration*/, float density)
{
  int num_ships = 0;
  float c = SRAND*2.0F;

  EnemyWave_t wave;
  wave.type = ENEMYAIRCRAFT_OMNI;
  wave.formation = FORMATION_ARROW;
  wave.setPos (c, 10.0);
  wave.setFrequency ((int)(25*(1.0/density)), 0);
  wave.xJitter = 1.0;
  wave.setInOut (o+50, o+150);
  num_ships += Splot_Screen::add (wave);

  wave.setInOut (o+250, o+320);
  num_ships += Splot_Screen::add (wave);
  wave.setInOut (o+300, o+330);
  num_ships += Splot_Screen::add (wave);
  wave.setInOut (o+350, o+470);
  num_ships += Splot_Screen::add (wave);

  wave.setFrequency (5, 0);
  wave.xJitter = 1.8F;
  wave.setInOut (o+550, o+555);
  num_ships += Splot_Screen::add (wave);

  return num_ships;
}

unsigned int
Splot_Screen::addGnatWave (int o, int duration, float density, bool mixed)
{
  State_t& state = SPLOT_STATE_SINGLETON::instance ()->get ();
  int num_ships = 0;
  float c = -FRAND*3.0F;
  EnemyWave_t wave;
  if (mixed)
  {
    wave.type = ENEMYAIRCRAFT_STRAIGHT;
    wave.setInOut (o+50, o+duration);
    wave.setPos (c, 10.0);
    wave.setFrequency ((int)(90 * (1.0 / density)), 20);
    wave.xJitter = 8.0;
    if (c > 0.0)
      num_ships += Splot_Screen::add (wave);
    else
    {
      EnemyWave_t wave_2;
      wave_2.type = ENEMYAIRCRAFT_OMNI;
      wave_2.setInOut (o+50, o+130);
      wave_2.setPos (c, 10.0);
      wave_2.setFrequency ((int)(20*(1.0/density)), 0);
      wave_2.xJitter = 1.1F;
      num_ships += Splot_Screen::add (wave_2);
      wave_2.setInOut (o+320, o+400);
      num_ships += Splot_Screen::add (wave_2);

      wave.setInOut (o+200, o+250);
      num_ships += Splot_Screen::add (wave);
    } // end ELSE

    wave.type = ENEMYAIRCRAFT_GNAT;
    //wave.setInOut (o, o+(17*game->gameSkill)*(1.0+FRAND*0.2));
    wave.setInOut (o, o+(int)((25*state.game_skill)*(1.0+FRAND*0.2)));
    wave.setPos (SRAND*5.0F, 10.0F);
    wave.setFrequency ((int)(3*(1.0/density)), 0);
    wave.xJitter = 3.0;
    num_ships += Splot_Screen::add (wave);
  } // end IF
  else
  {
    wave.type = ENEMYAIRCRAFT_GNAT;
    //wave.setInOut (o, o+(17*game->gameSkill)*(1.0+FRAND*0.2));
    wave.setPos (-3.0F+(SRAND*5.0F), 10.0F);
    wave.setFrequency ((int)(1*(1.0/density)), 0);
    wave.xJitter = 3.0;
    wave.setInOut (o, o+35);
    num_ships += Splot_Screen::add (wave);
    wave.setPos (SRAND*5.0F, 10.0F);
    wave.setInOut (o+300, o+310);
    num_ships += Splot_Screen::add (wave);
    wave.setFrequency ((int)(30*(1.0/density)), 0);
    wave.setInOut (o+300, o+400);
    num_ships += Splot_Screen::add (wave);
  } // end ELSE

  return num_ships;
}

void
Splot_Screen::addPowerUps (PowerUpCategory_t category_in,
                           int o_in,
                           int duration_in,
                           int delay_a_in,
                           int delay_b_in,
                           int delay_c_in)
{
  int pause[3];
  if ((((category_in == POWERUPCATEGORY_REPAIR_SHIELD) ||
        (category_in == POWERUPCATEGORY_SPECIAL)) && DEBUG_NO_POWERUPS) ||
      ((category_in == POWERUPCATEGORY_SPECIAL) && DEBUG_NO_AMMUNITION))
  {
    pause[0] = ACE_Numeric_Limits<int>::max ();
    pause[1] = ACE_Numeric_Limits<int>::max ();
    pause[2] = ACE_Numeric_Limits<int>::max ();
  } // end IF
  else
    ACE_OS::memset (&pause, 0, sizeof (pause));
  int delay_a = delay_a_in;
  int delay_b = delay_b_in;
  int delay_c = delay_c_in;
  float position[3] = {0.0, 0.0, 0.0};
  float translation_vector[3] = {0.0, 0.0, 0.0};
  switch (category_in)
  {
    case POWERUPCATEGORY_AMMUNITION:
    {
      if (delay_b == 0)
        delay_b = 100;
      if (delay_c == 0)
        delay_c = 1000;
      pause[0] = delay_a+(int)(FRAND*200);
      pause[1] = delay_b+(int)(FRAND*200);
      pause[2] = delay_c+(int)(FRAND*500);

      position[1] = 9.0;
      position[2] = 25.0;
      break;
    }
    case POWERUPCATEGORY_REPAIR_SHIELD:
    {
      if (delay_a == 0)
        delay_a = 300;
      if (delay_b == 0)
        delay_b = 1200;
      if (delay_c == 0)
        delay_c = 1000;
      pause[0] = delay_a+(int)(FRAND*500);
      pause[1] = delay_b+(int)(FRAND*500);
      pause[2] = delay_c+(int)(FRAND*1500);

      position[1] = 9.0;
      position[2] = 25.0;
      break;
    }
    case POWERUPCATEGORY_SPECIAL:
    {
      // *TODO*
      pause[0] = ACE_Numeric_Limits<int>::max ();
      pause[1] = ACE_Numeric_Limits<int>::max ();
      pause[2] = ACE_Numeric_Limits<int>::max ();
      break;
    }
    default:
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("invalid/unknown category (was: %d), returning\n"),
                  category_in));
      return;
    }
  } // end SWITCH

  State_t& state = SPLOT_STATE_SINGLETON::instance ()->get ();
  Splot_PowerUp* power_up = NULL;
  int count = 0;
  for (int i = o_in; i < o_in+duration_in; i++)
  {
    for (int j = 0; j < 3; j++)
      pause[j]--;

    power_up = NULL;
    switch (category_in)
    {
      case POWERUPCATEGORY_AMMUNITION:
      {
        position[0] = 8.0F*SRAND;
        if (pause[0] < 1)
        {
          pause[0] = (int)((2.0F-state.game_skill)*2000+(int)(SRAND*500));
          ACE_NEW_NORETURN (power_up,
                            Splot_PowerUp (POWERUP_AMMUNITION_0, position, translation_vector));
          if (!power_up)
          {
            ACE_DEBUG ((LM_CRITICAL,
                        ACE_TEXT ("failed to allocate memory, continuing\n")));

            break;
          } // end IF
          Splot_Screen::add (i, power_up);
          position[0] = 8.0F*SRAND;
          count++;
        } // end IF
        if (pause[1] < 1)
        {
          pause[1] = (int)((2.0F-state.game_skill)*2500+(int)(SRAND*1000));
          ACE_NEW_NORETURN (power_up,
                            Splot_PowerUp (POWERUP_AMMUNITION_1, position, translation_vector));
          if (!power_up)
          {
            ACE_DEBUG ((LM_CRITICAL,
                        ACE_TEXT ("failed to allocate memory, continuing\n")));

            break;
          } // end IF
          Splot_Screen::add (i, power_up);
          position[0] = 8.0F*SRAND;
          count++;
        } // end IF
        if (pause[2] < 1)
        {
          pause[2] = (int)((2.0F-state.game_skill)*4000+(int)(SRAND*1000));
          ACE_NEW_NORETURN (power_up,
                            Splot_PowerUp (POWERUP_AMMUNITION_2, position, translation_vector));
          if (!power_up)
          {
            ACE_DEBUG ((LM_CRITICAL,
                        ACE_TEXT ("failed to allocate memory, continuing\n")));

            break;
          } // end IF
          Splot_Screen::add (i, power_up);
          count++;
        } // end IF
        break;
      }
      case POWERUPCATEGORY_REPAIR_SHIELD:
      {
        position[0] = 8.0F*SRAND;
        if (pause[0] < 1)
        {
          pause[0] = 2500+(int)(SRAND*700);
          ACE_NEW_NORETURN (power_up,
                            Splot_PowerUp (POWERUP_SHIELD, position, translation_vector));
          if (!power_up)
          {
            ACE_DEBUG ((LM_CRITICAL,
                        ACE_TEXT ("failed to allocate memory, continuing\n")));

            break;
          } // end IF
          Splot_Screen::add (i, power_up);
          position[0] = 8.0F*SRAND;
          count++;
        } // end IF
        if (pause[1] < 1)
        {
          pause[1] = 4000+(int)(SRAND*900);
          ACE_NEW_NORETURN (power_up,
                            Splot_PowerUp (POWERUP_REPAIR, position, translation_vector));
          if (!power_up)
          {
            ACE_DEBUG ((LM_CRITICAL,
                        ACE_TEXT ("failed to allocate memory, continuing\n")));

            break;
          } // end IF
          Splot_Screen::add (i, power_up);
          position[0] = 8.0F*SRAND;
          count++;
        } // end IF
        if (pause[2] < 1)
        {
          pause[2] = 5000+(int)(SRAND*3000);
          ACE_NEW_NORETURN (power_up,
                            Splot_PowerUp (POWERUP_SHIELD_SUPER, position, translation_vector));
          if (!power_up)
          {
            ACE_DEBUG ((LM_CRITICAL,
                        ACE_TEXT ("failed to allocate memory, continuing\n")));

            break;
          } // end IF
          Splot_Screen::add (i, power_up);
          count++;
        } // end IF
        break;
      }
      case POWERUPCATEGORY_SPECIAL:
      {
        // *TODO*
        //count++;
        break;
      }
      default:
      {
        ACE_DEBUG ((LM_ERROR,
                    ACE_TEXT ("invalid/unknown category (was: %d), returning\n"),
                    category_in));
        return;
      }
    } // end SWITCH
  } // end FOR

  //const Configuration_t& configuration =
  //  SPLOT_CONFIGURATION_SINGLETON::instance ()->get ();
  //if (configuration.debug)
  //  ACE_DEBUG ((LM_INFO,
  //              ACE_TEXT ("added %d power-up(s) (category: %d)\n"),
  //              count,
  //              category_in));
}
