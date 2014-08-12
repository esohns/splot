#ifndef ENEMIES_H
#define ENEMIES_H

#include "enemy_fleet.h"

class Splot_Enemies
{
 public:
  Splot_Enemies ();
  ~Splot_Enemies ();

  Splot_EnemyFleet* getFleet () { return &fleet_; };

  void drawGL () { fleet_.drawGL (); };
  void clear () { fleet_.clear (); };

  void addEnemy (Splot_EnemyAircraft* aircraft_in) { fleet_.addEnemy (aircraft_in); };
  void killEnemy (Splot_EnemyAircraft* aircraft_in) { fleet_.killEnemy (aircraft_in); };
  void destroyAll () { fleet_.destroyAll (); };

  void update () { fleet_.update (); };

  void loadTextures () { fleet_.loadTextures (); };
  void deleteTextures () { fleet_.deleteTextures (); };

  private:
   Splot_EnemyFleet fleet_;
};

#endif // ENEMIES_H
