#include "stdafx.h"

#include "player_bullets.h"

//#ifdef HAVE_CONFIG_H
//#include "splot-config.h"
//#endif
//
//#include "gettext.h"
//
//#include "Splot_PlayerBullets.h"
//
//#include <cstdio>
//#include <cstdlib>
//
//#include "compatibility.h"
//
//#if defined(HAVE_APPLE_OPENGL_FRAMEWORK) || defined(HAVE_OPENGL_GL_H)
//#include <OpenGL/gl.h>
//#else
//#include <GL/gl.h>
//#endif

#include <string>

#include "ace/OS.h"
#include "ace/Log_Msg.h"

#include "defines.h"
#include "common.h"
#include "configuration.h"
#include "state.h"
#include "Explosions.h"
#include "enemy_fleet.h"
//#include "EnemyAircraft.h"
#include "image.h"

// init statics
float Splot_PlayerBullets::bullet_size[NUM_PLAYER_AMMO_TYPES][2];
float Splot_PlayerBullets::bullet_damage[NUM_PLAYER_AMMO_TYPES];

void
Splot_PlayerBullets::initialize ()
{
  Splot_PlayerBullets::bullet_size[0][0] = 0.05F; Splot_PlayerBullets::bullet_size[0][1] = 0.65F;
  Splot_PlayerBullets::bullet_size[1][0] = 0.11F; Splot_PlayerBullets::bullet_size[1][1] = 1.5F;
  Splot_PlayerBullets::bullet_size[2][0] = 0.3F;  Splot_PlayerBullets::bullet_size[2][1] = 1.5F;

  Splot_PlayerBullets::bullet_damage[0] = 3.5F;
  Splot_PlayerBullets::bullet_damage[1] = 6.0F;
  Splot_PlayerBullets::bullet_damage[2] = 40.0F;
}

Splot_PlayerBullets::Splot_PlayerBullets ()
{
  for (int i = 0; i < NUM_PLAYER_AMMO_TYPES; i++)
  {
    //ammoRoot[i]  = new ActiveAmmo();
   // bulletSize_[i][0] = 0.1F;
   // bulletSize_[i][0] = 0.5F;
    //bulletDamage_[i]	= 2.5F;
    bulletTex_[i] = 0;
  } // end FOR
  ////ammoPool  = new ActiveAmmo();

  loadTextures ();
}

Splot_PlayerBullets::~Splot_PlayerBullets ()
{	
  clear ();
  //for (int i = 0; i < NUM_HERO_AMMO_TYPES; i++)
	 // delete ammoRoot[i];

  //ActiveAmmo *cur = ammoPool->next;
  //ActiveAmmo *del;
  //while (cur)
  //{
  // del = cur;
  // cur = cur->next;
  // delete del;
  //}	
  //delete ammoPool;
  //bulletPool_.clear ();

  deleteTextures ();
}

void
Splot_PlayerBullets::loadTextures ()
{
  char filename[PATH_MAX];

  std::string path_base = ACE_TEXT_ALWAYS_CHAR (SPLOT_IMAGE_DATA_DIR);
  path_base += ACE_DIRECTORY_SEPARATOR_STR;
  std::string format_string = ACE_TEXT_ALWAYS_CHAR ("%sAmmo%02d.png");
  int result = -1;
  for (int i = 0; i < NUM_PLAYER_AMMO_TYPES; i++)
  {
    if (ACE_OS::snprintf (filename,
                          PATH_MAX,
                          format_string.c_str (),
                          path_base.c_str(), i) < 0)
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to snprintf (\"%s\"): \"%m\", continuing\n"),
                  ACE_TEXT (filename)));

      continue;
    } // end IF
    bulletTex_[i] = Splot_Image::load (dataLoc (filename));
    if (!bulletTex_[i])
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to Splot_Image::load (\"%s\"), continuing\n"),
                  ACE_TEXT (dataLoc (filename).c_str ())));
  } // end FOR
}

void
Splot_PlayerBullets::deleteTextures ()
{
  glDeleteTextures (NUM_PLAYER_AMMO_TYPES, &(bulletTex_[0]));
}

//void
//Splot_PlayerBullets::killBullet (const bullet_t& bullet_in)
//{
//  bullets_const_iterator_t iterator;
//  for (int i = 0; i < NUM_PLAYER_AMMO_TYPES; i++)
//  {
//    for (iterator = bullets_[i].begin ();
//         iterator != bullets_[i].end ();
//         iterator++)
//      if (*iterator == bullet_in)
//        break;
//      if (iterator != bullets_[i].end())
//      {
//        bullets_[i].erase(iterator);
//        bullet_t::count--;
//      } // end IF
//  } // end FOR
//}

void
Splot_PlayerBullets::clear ()
{
  for (int i = 0; i < NUM_PLAYER_AMMO_TYPES; i++)
    bullets_[i].clear ();
}

void
Splot_PlayerBullets::addBullet (int type_in,
                                float position_in[3])
{
  State_t& state = SPLOT_STATE_SINGLETON::instance ()->get ();
  float v[3] = { 0.0, 0.0, 0.0 };
  switch (type_in)
  {
    case 0: v[1] = 0.5F*state.speed_adj; break;
    case 1: v[1] = 0.2F*state.speed_adj; break;
    case 2: v[1] = 0.3F*state.speed_adj; break;
    default:
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("invalid/unkown bullet type (was: %d), returning\n"),
                  type_in));

      return;
  } // end SWITCH

  const Configuration_t& configuration =
    SPLOT_CONFIGURATION_SINGLETON::instance ()->get ();
  if (configuration.debug)
    ACE_DEBUG ((LM_DEBUG,
                ACE_TEXT ("adding bullet (type/position): %d / [%f,%f,%f]\n"),
                type_in, position_in[0], position_in[1], position_in[2]));

  bullet_t bullet;
  bullet.damage = bullet_damage[type_in];
  //bullet.position = position_in;
  ACE_OS::memcpy (&(bullet.position), &position_in, sizeof (position_in));
  //bullet.translation_step = v;
  ACE_OS::memcpy (&(bullet.translation_step), &v, sizeof (v));
  bullets_[type_in].push_back(bullet);
  bullet_t::count++;
}

void
Splot_PlayerBullets::print (int type_in)
{
  ACE_ASSERT (type_in >= 0 && type_in < NUM_PLAYER_AMMO_TYPES);

  int i = 0;
  for (bullets_iterator_t iterator = bullets_[type_in].begin ();
       iterator != bullets_[type_in].end ();
       iterator++, i++)
    ACE_DEBUG ((LM_DEBUG,
                ACE_TEXT ("#%d\t(type/position/step): %d / [%f,%f,%f] / [%f,%f,%f]\n"),
                i + 1, type_in,
                (*iterator).position[0], (*iterator).position[1], (*iterator).position[2],
                (*iterator).translation_step[0], (*iterator).translation_step[1], (*iterator).translation_step[2]));
}

void
Splot_PlayerBullets::update ()
{
  const Configuration_t& configuration =
    SPLOT_CONFIGURATION_SINGLETON::instance ()->get ();

  std::vector<bullets_iterator_t> bullets;
  for (int i = 0; i < NUM_PLAYER_AMMO_TYPES; i++)
  {
    bullets.clear ();
    for (bullets_iterator_t iterator = bullets_[i].begin ();
         iterator != bullets_[i].end ();
         iterator++)
    {
      if ((*iterator).position[1] > configuration.screen_bound[1])
        bullets.push_back (iterator);
      else
      {
        bullet_t& current = *iterator;
        current.position[0] += current.translation_step[0];
        current.position[1] += current.translation_step[1];
        current.position[2] += current.translation_step[2];
      } // end ELSE
    } // end FOR

    for (std::vector<bullets_iterator_t>::reverse_iterator iterator = bullets.rbegin ();
         iterator != bullets.rend ();
         iterator++)
      bullets_[i].erase (*iterator);
    bullet_t::count -= bullets.size ();
  } // end FOR
}

void
Splot_PlayerBullets::checkForHits (Splot_EnemyFleet* fleet_in)
{
  ACE_ASSERT (fleet_in);

  //-- get minimum ship Y location --> ignore some of the bullets
  float	min_ship_y = 100.0;
  fleet_in->toFirst ();
  Splot_EnemyAircraft* enemy = NULL; fleet_in->getShip ();
  while (enemy = fleet_in->getShip ())
    if ((enemy->position_[1]-3.0F) < min_ship_y)
      min_ship_y = enemy->position_[1]-3.0F;

  //-- Go through all the ammunition and check for hits
  State_t& state = SPLOT_STATE_SINGLETON::instance ()->get ();
  std::vector<bullets_iterator_t> bullets;
  for (int i = 0; i < NUM_PLAYER_AMMO_TYPES; i++)
  {
    bullets.clear ();
    for (bullets_iterator_t iterator = bullets_[i].begin ();
         iterator != bullets_[i].end ();
         iterator++)
    {
      if ((*iterator).position[1] < min_ship_y)
        continue;

      fleet_in->toFirst ();
      while (enemy = fleet_in->getShip ())
      {
        if (!enemy->checkHit (*iterator))
          continue;

        // do damage / explosion
        if (i == 1)
          enemy->damage_ += Splot_PlayerBullets::bullet_damage[i]*state.speed_adj;
        else
          enemy->damage_ += Splot_PlayerBullets::bullet_damage[i];
        state.explosions->addExplo ((Explosions::ExploType)(Explosions::HeroAmmo00+i), (*iterator).position);

        if (i != 1) // bullet type 1 doesn't get removed immediately
        {
          bullets.push_back (iterator);
          break;
        } // end IF
      } // end WHILE
    } // end FOR

    for (std::vector<bullets_iterator_t>::reverse_iterator iterator = bullets.rbegin ();
         iterator != bullets.rend ();
         iterator++)
      bullets_[i].erase (*iterator);
    bullet_t::count -= bullets.size();
  } // end FOR
}

void
Splot_PlayerBullets::drawGL ()
{
  for (int i = 0; i < NUM_PLAYER_AMMO_TYPES; i++)
  {
    glColor4f (1.0F, 1.0F, 1.0F, 1.0F);
    glBindTexture (GL_TEXTURE_2D, bulletTex_[i]);
    glBegin (GL_QUADS);
    for (bullets_iterator_t iterator = bullets_[i].begin ();
         iterator != bullets_[i].end ();
         iterator++)
    {
      bullet_t& current = *iterator;
      glTexCoord2f (0.0F, 0.0F);
      glVertex3f (current.position[0]-Splot_PlayerBullets::bullet_size[i][0],
                  current.position[1],
                  current.position[2]);
      glTexCoord2f (0.0F, 1.0F);
      glVertex3f (current.position[0]-Splot_PlayerBullets::bullet_size[i][0],
                  current.position[1]-Splot_PlayerBullets::bullet_size[i][1],
                  current.position[2]);
      glTexCoord2f (1.0F, 1.0F);
      glVertex3f (current.position[0]+Splot_PlayerBullets::bullet_size[i][0],
                  current.position[1]-Splot_PlayerBullets::bullet_size[i][1],
                  current.position[2]);
      glTexCoord2f (1.0F, 0.0F);
      glVertex3f (current.position[0]+Splot_PlayerBullets::bullet_size[i][0],
                  current.position[1],
                  current.position[2]);
    } // end FOR
    glEnd ();
  } // end FOR
}
