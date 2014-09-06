#ifndef SCREEN_H
#define SCREEN_H

#include "ace/Global_Macros.h"

#include "enemy_aircraft.h"
#include "enemies.h"
#include "powerup.h"

// forward declarations
class Splot_GameElement;

class Splot_Screen
{
 public:
  static void put ();
  static void remove (Splot_GameElement*);
  static void clear ();

  static bool load ();
  static bool makeAdd (EnemyAircraftType_t, // type
                       const float (&)[3],  // position
                       int);                // release time

 private:
  //void clearDeadPool();
  static void add (int,                 // release time
                   Splot_GameElement*); // game element
  static unsigned int add (const EnemyWave_t&); // wave

  static void loadLevel ();
  static void loadLevel1 ();
  static void loadLevel2 ();
  static void loadLevel3 ();
  static void loadLevel4 ();

  static unsigned int addStraightWave (int o, int duration, float density = 1.0);
  static unsigned int addOmniWave (int o, int duration, float density = 1.0);
  static unsigned int addStraightArrowWave (int o, int duration, float density = 1.0);
  static unsigned int addOmniArrowWave (int o, int duration, float density = 1.0);
  static unsigned int addGnatWave (int o, int duration, float density = 1.0, bool mixed = true);

  static void addPowerUps (PowerUpCategory_t, // category
                           int,               // origin (release time begin)
                           int,               // duration (release time end)
                           int = 0,           // initial delay (base) (type a)
                           int = 0,           // initial delay (base) (type b)
                           int = 0);          // initial delay (base) (type c)

  //static Splot_EnemyAircraft* deadPool[MAX_ENEMYAIRCRAFT_TYPES];

 private:
  ACE_UNIMPLEMENTED_FUNC (Splot_Screen ());
  ACE_UNIMPLEMENTED_FUNC (~Splot_Screen ());
};

#endif // SCREEN_H
