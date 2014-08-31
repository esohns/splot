#ifndef SCREEN_H
#define SCREEN_H

#include "ace/Global_Macros.h"

#include "enemy_aircraft.h"
#include "enemies.h"

class Splot_Screen
{
 public:
  static void put ();
  static void remove (Splot_GameElement*);
  static void clear ();

  static bool load ();
  static Splot_EnemyAircraft* makeAdd (EnemyAircraftType_t, // type
                                       const float (&)[3],  // position
                                       int);                // release time

 private:
  //void clearDeadPool();
  static void add (int,                 // release time
                   Splot_GameElement*); // game element
  static void add (const EnemyWave_t&); // wave

  static void loadLevel ();
  static void loadLevel1 ();
  static void loadLevel2 ();
  static void loadLevel3 ();
  static void loadLevel4 ();

  static void addStraightWave (int o, int duration, float density = 1.0);
  static void addOmniWave (int o, int duration, float density = 1.0);
  static void addStraightArrowWave (int o, int duration, float density = 1.0);
  static void addOmniArrowWave (int o, int duration, float density = 1.0);
  static void addGnatWave (int o, int duration, float density = 1.0, bool mixed = true);

  static void addAmmunition (int o, int duration, int a = 0, int b = 100, int c = 1000);
  static void addPowerUps (int o, int duration, int a = 300, int b = 1200, int c = 1000);

  //static Splot_EnemyAircraft* deadPool[MAX_ENEMYAIRCRAFT_TYPES];

 private:
  ACE_UNIMPLEMENTED_FUNC (Splot_Screen ());
  ACE_UNIMPLEMENTED_FUNC (~Splot_Screen ());
};

#endif // SCREEN_H
