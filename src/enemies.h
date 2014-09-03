#ifndef ENEMIES_H
#define ENEMIES_H

#include "enemy_fleet.h"
#include "enemy_aircraft.h"

enum Formation_t
{
  FORMATION_INVALID = -1,
  FORMATION_ARROW = 0,
  FORMATION_NONE,
  ///////////////////////////////////////
  MAX_FORMATION_TYPES
};

struct EnemyWave_t
{
  // *TODO*: remove these
  EnemyWave_t ()
   : type (ENEMYAIRCRAFT_INVALID)
   , formation (FORMATION_NONE)
   , begin (1)
   , end (500)
   , period (60)
   , jitter (10)
   //, position ()
   , xJitter (8.0)
  { position [0] = 0.0; position[1] = 10.0; position[2] = 25.0; }
#if defined (__GNU_C__)
  __attribute__((noinline)) void setInOut (int b, int e) { begin = b; end = e; if (e <= b) end = b + 1; }
#elif defined (_MSC_VER)
	void setInOut (int b, int e) { begin = b; end = e; if (e <= b) end = b + 1; }
#endif
  void setPos (float x, float y) { position[0] = x; position[1] = y; }
  void setFrequency (int p, int j = 0) { period = p; jitter = j; }

  EnemyAircraftType_t type;
  Formation_t         formation;
  int                 begin;
  int                 end;
  int                 period;
  int                 jitter;
  float               position[3];
  float               xJitter;
};

class Splot_Enemies
{
 public:
  Splot_Enemies ();
  ~Splot_Enemies ();

  Splot_EnemyFleet* getFleet () { return &fleet_; };

  void drawGL () { fleet_.drawGL (); };
  void clear () { fleet_.clear (); };

  void add (Splot_EnemyAircraft* aircraft_in) { fleet_.add (aircraft_in); };
  void destroyAll () { fleet_.destroyAll (); };

  void update () { fleet_.update (); };

  void loadTextures () { fleet_.loadTextures (); };
  void deleteTextures () { fleet_.deleteTextures (); };

  private:
   Splot_EnemyFleet fleet_;
};

#endif // ENEMIES_H
