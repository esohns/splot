#include "stdafx.h"

#include "enemy_bullets.h"

//#ifdef HAVE_CONFIG_H
//#include "splot-config.h"
//#endif

//#include "Splot_EnemyBullets.h"
//
//#include <cstdio>
//#include <cstdlib>
//#include <cmath>
//
//#include "compatibility.h"
//
//#if defined(HAVE_APPLE_OPENGL_FRAMEWORK) || defined(HAVE_OPENGL_GL_H)
//#include <OpenGL/gl.h>
//#else
//#include <GL/gl.h>
//#endif
//

#include <string>

#include "ace/OS.h"
#include "ace/Log_Msg.h"

#include "common.h"
#include "state.h"
#include "configuration.h"
#include "explosion.h"
//#include "EnemyFleet.h"
//#include "EnemyAircraft.h"
#include "image.h"
#include "player_aircraft.h"

// init statics
float Splot_EnemyBullets::bullet_size[NUM_ENEMY_AMMO_TYPES][2];
float Splot_EnemyBullets::bullet_damage[NUM_ENEMY_AMMO_TYPES];

void
Splot_EnemyBullets::initialize ()
{
  ////-- initialize everything to sane values...
  //for (int i = 0; i < NUM_ENEMY_AMMO_TYPES; i++)
  //{
  //  Splot_EnemyBullets::bullet_size[i][0] = 0.13;
  //  Splot_EnemyBullets::bullet_size[i][1] = 0.13;
  //  Splot_EnemyBullets::bullet_damage[i] = 510.0;
  //} // end FOR

  Splot_EnemyBullets::bullet_damage[0] = 75.0;
  Splot_EnemyBullets::bullet_size[0][0] = 0.25F; Splot_EnemyBullets::bullet_size[0][1] = 0.55F;

  Splot_EnemyBullets::bullet_damage[1] = 6.0;
  Splot_EnemyBullets::bullet_size[1][0] = 0.225F; Splot_EnemyBullets::bullet_size[1][1] = 0.225F;

  Splot_EnemyBullets::bullet_damage[2] = 100.0;
  Splot_EnemyBullets::bullet_size[2][0] = 0.45F; Splot_EnemyBullets::bullet_size[2][1] = 0.45F;

  Splot_EnemyBullets::bullet_damage[3] = 20.0;
  Splot_EnemyBullets::bullet_size[3][0] = 0.3F; Splot_EnemyBullets::bullet_size[3][1] = 0.5F;

  Splot_EnemyBullets::bullet_damage[4] = 8.5;
  Splot_EnemyBullets::bullet_size[4][0] = 0.12F; Splot_EnemyBullets::bullet_size[4][1] = 0.5F;
}

Splot_EnemyBullets::Splot_EnemyBullets ()
{
  //-- initialize everything to sane values...
  for (int i = 0; i < NUM_ENEMY_AMMO_TYPES; i++)
  {
    //Splot_EnemyBullets::bullet_size[i][0] = 0.13;
    //Splot_EnemyBullets::bullet_size[i][1] = 0.13;
    //Splot_EnemyBullets::bullet_damage[i] = 510.0;
    texBullet_[i] = 0;
  } // end FOR

  loadTextures ();
}

Splot_EnemyBullets::~Splot_EnemyBullets ()
{
  clear ();

  deleteTextures ();
}

void
Splot_EnemyBullets::loadTextures ()
{
  std::string path_base = ACE_TEXT_ALWAYS_CHAR (SPLOT_IMAGE_DATA_DIR);
  path_base += ACE_DIRECTORY_SEPARATOR_STR;
  std::string filename;
  char buffer[PATH_MAX];
  for (int i = 0; i < NUM_ENEMY_AMMO_TYPES; i++)
  {
    filename = path_base;
    ACE_OS::sprintf (buffer, ACE_TEXT_ALWAYS_CHAR ("EnemyAmmo%02d.png"), i);
    filename += buffer;
    texBullet_[i] = Splot_Image::load (dataLoc (filename.c_str ()));
    if (!texBullet_[i])
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to Splot_Image::load(\"%s\"), continuing\n"),
                  filename.c_str ()));
  } // end FOR
}

void
Splot_EnemyBullets::deleteTextures ()
{
  glDeleteTextures (NUM_ENEMY_AMMO_TYPES, &(texBullet_[0]));
}

//ActiveAmmo*
//Splot_EnemyBullets::getNewAmmo ()
//{
//	ActiveAmmo *retAmmo;
//	if(ammoPool->next)
//	{
//		retAmmo = ammoPool->next;
//		ammoPool->next = retAmmo->next;
//	}
//	else
//	{
//		retAmmo = new ActiveAmmo();
//	}
//	return retAmmo;
//}

//void
//Splot_EnemyBullets::remove (const Bullet_t& bullet_in)
//{
//  for (int i = 0; i < NUM_ENEMY_AMMO_TYPES; i++)
//  {
//    BulletsIterator_t iterator;
//    for (iterator = bullets_[i].begin ();
//         iterator != bullets_[i].end ();
//         iterator++)
//      if (*iterator == bullet_in)
//        break;
//    if (iterator != bullets_[i].end ())
//    {
//      bullets_[i].erase (iterator);
//      break;
//    } // end IF
//  } // end FOR
//}

void
Splot_EnemyBullets::clear ()
{
  for (int i = 0; i < NUM_ENEMY_AMMO_TYPES; i++)
    bullets_[i].clear ();
}

void
Splot_EnemyBullets::add (int type_in,
                         float position_in[3],
                         float velocity_in[3])
{
  if (type_in < 0 ||
      type_in >= NUM_ENEMY_AMMO_TYPES)
    return;

  State_t& state = SPLOT_STATE_SINGLETON::instance ()->get ();
  float v[3] = {velocity_in[0]*state.speed_adj,
                velocity_in[1]*state.speed_adj,
                velocity_in[2]*state.speed_adj};

  Bullet_t new_bullet;
  new_bullet.damage = Splot_EnemyBullets::bullet_damage[type_in];
  //new_bullet.position = position_in;
  ACE_OS::memcpy (&new_bullet.position, &position_in, sizeof (position_in));
  //new_bullet.translation_step = v;
  ACE_OS::memcpy (&new_bullet.translation_step, &v, sizeof (v));
  bullets_[type_in].push_back (new_bullet);

  Bullet_t::count++;
}

void
Splot_EnemyBullets::update ()
{
  const Configuration_t& configuration =
    SPLOT_CONFIGURATION_SINGLETON::instance ()->get ();

  std::vector<BulletsConstIterator_t> finished_bullets;
  for (int i = 0; i < NUM_ENEMY_AMMO_TYPES; i++)
  {
    finished_bullets.clear ();
    for (BulletsIterator_t iterator = bullets_[i].begin ();
         iterator != bullets_[i].end ();
         iterator++)
    {
      if ((*iterator).position[1] > configuration.screen_bound[1])
        finished_bullets.push_back (iterator);
      else
      {
        Bullet_t& current = *iterator;
        current.position[0] += current.translation_step[0];
        current.position[1] += current.translation_step[1];
        current.position[2] += current.translation_step[2];
      } // end ELSE
    } // end FOR

    for (std::vector<BulletsConstIterator_t>::reverse_iterator iterator = finished_bullets.rbegin ();
         iterator != finished_bullets.rend ();
         iterator++)
      bullets_[i].erase (*iterator);
    Bullet_t::count -= finished_bullets.size ();
  } // end FOR
}

void
Splot_EnemyBullets::checkForHits (Splot_PlayerAircraft* aircraft_in)
{
  if (!aircraft_in->isVisible ())
    return; // nothing to do

  float min_dist = (aircraft_in->getSize (0)+aircraft_in->getSize (1))*0.5F;
  float dist;
  State_t& state = SPLOT_STATE_SINGLETON::instance ()->get ();
  Explosion_t* explosion;

  //-- Go through all the ammunition and check for hits
  std::vector<BulletsConstIterator_t> finished_bullets;
  for (int i = 0; i < NUM_ENEMY_AMMO_TYPES; i++)
  {
    finished_bullets.clear ();

    for (BulletsConstIterator_t iterator = bullets_[i].begin ();
         iterator != bullets_[i].end ();
         iterator++)
    {
      dist = fabs ((*iterator).position[0]-aircraft_in->position_[0]) + 
             fabs ((*iterator).position[1]-aircraft_in->position_[1]);
      if (dist >= min_dist)
        continue;

      //do damage
      //				hero->doDamage(ammoDamage[i]);
      aircraft_in->ammoDamage ((*iterator).damage, (*iterator).translation_step);
      //add explosion
      explosion =
        state.explosions->add ((ExplosionType_t)(EXPLOSION_ENEMY_AMMUNITION_0 + i), (*iterator).position);
      if (!explosion)
        ACE_DEBUG ((LM_ERROR,
                    ACE_TEXT ("failed to Splot_Explosions::add(%d), continuing\n"),
                    EXPLOSION_ENEMY_AMMUNITION_0 + i));
      if (i > 1)	// add second explosion for the bug guns...
        explosion =
          state.explosions->add ((ExplosionType_t)(EXPLOSION_ENEMY_AMMUNITION_0 + i), (*iterator).position, -5);
      else
        if (explosion)
          explosion->velocity[1] = -0.1F;

      finished_bullets.push_back (iterator);
    } // end FOR

    for (std::vector<BulletsConstIterator_t>::reverse_iterator iterator = finished_bullets.rbegin ();
         iterator != finished_bullets.rend ();
         iterator++)
      bullets_[i].erase (*iterator);
    Bullet_t::count -= finished_bullets.size ();
  } // end FOR
}

void
Splot_EnemyBullets::drawGL ()
{
  glColor4f (1.0, 1.0, 1.0, 1.0);

  for (int i = 0; i < NUM_ENEMY_AMMO_TYPES; i++)
  {
    glBindTexture (GL_TEXTURE_2D, texBullet_[i]);
    glBegin (GL_QUADS);

    for (BulletsConstIterator_t iterator = bullets_[i].begin ();
         iterator != bullets_[i].end ();
         iterator++)
    {
      switch (IRAND%4)
      {
        case 0:
          glTexCoord2f (0.0, 0.0);
          glVertex3f ((*iterator).position[0]-Splot_EnemyBullets::bullet_size[i][0],
                      (*iterator).position[1]+Splot_EnemyBullets::bullet_size[i][1],
                      (*iterator).position[2]);
          glTexCoord2f (0.0, 1.0);
          glVertex3f ((*iterator).position[0]-Splot_EnemyBullets::bullet_size[i][0],
                      (*iterator).position[1]-Splot_EnemyBullets::bullet_size[i][1],
                      (*iterator).position[2]);
          glTexCoord2f (1.0, 1.0);
          glVertex3f ((*iterator).position[0]+Splot_EnemyBullets::bullet_size[i][0],
                      (*iterator).position[1]-Splot_EnemyBullets::bullet_size[i][1],
                      (*iterator).position[2]);
          glTexCoord2f (1.0, 0.0);
          glVertex3f ((*iterator).position[0]+Splot_EnemyBullets::bullet_size[i][0],
                      (*iterator).position[1]+Splot_EnemyBullets::bullet_size[i][1],
                      (*iterator).position[2]);
          break;
        case 1:
          glTexCoord2f (1.0, 0.0);
          glVertex3f ((*iterator).position[0]-Splot_EnemyBullets::bullet_size[i][0],
                      (*iterator).position[1]+Splot_EnemyBullets::bullet_size[i][1],
                      (*iterator).position[2]);
          glTexCoord2f (1.0, 1.0);
          glVertex3f ((*iterator).position[0]-Splot_EnemyBullets::bullet_size[i][0],
                      (*iterator).position[1]-Splot_EnemyBullets::bullet_size[i][1],
                      (*iterator).position[2]);
          glTexCoord2f (0.0, 1.0);
          glVertex3f ((*iterator).position[0]+Splot_EnemyBullets::bullet_size[i][0],
                      (*iterator).position[1]-Splot_EnemyBullets::bullet_size[i][1],
                      (*iterator).position[2]);
          glTexCoord2f (0.0, 0.0);
          glVertex3f ((*iterator).position[0]+Splot_EnemyBullets::bullet_size[i][0],
                      (*iterator).position[1]+Splot_EnemyBullets::bullet_size[i][1],
                      (*iterator).position[2]);
          break;
        case 2:
          glTexCoord2f (0.0, 1.0);
          glVertex3f ((*iterator).position[0]-Splot_EnemyBullets::bullet_size[i][0],
                      (*iterator).position[1]+Splot_EnemyBullets::bullet_size[i][1],
                      (*iterator).position[2]);
          glTexCoord2f (0.0, 0.0);
          glVertex3f ((*iterator).position[0]-Splot_EnemyBullets::bullet_size[i][0],
                      (*iterator).position[1]-Splot_EnemyBullets::bullet_size[i][1],
                      (*iterator).position[2]);
          glTexCoord2f (1.0, 0.0);
          glVertex3f ((*iterator).position[0]+Splot_EnemyBullets::bullet_size[i][0],
                      (*iterator).position[1]-Splot_EnemyBullets::bullet_size[i][1],
                      (*iterator).position[2]);
          glTexCoord2f (1.0, 1.0);
          glVertex3f ((*iterator).position[0]+Splot_EnemyBullets::bullet_size[i][0],
                      (*iterator).position[1]+Splot_EnemyBullets::bullet_size[i][1],
                      (*iterator).position[2]);
          break;
        case 3:
          glTexCoord2f (1.0, 1.0);
          glVertex3f ((*iterator).position[0]-Splot_EnemyBullets::bullet_size[i][0],
                      (*iterator).position[1]+Splot_EnemyBullets::bullet_size[i][1],
                      (*iterator).position[2]);
          glTexCoord2f (1.0, 0.0);
          glVertex3f ((*iterator).position[0]-Splot_EnemyBullets::bullet_size[i][0],
                      (*iterator).position[1]-Splot_EnemyBullets::bullet_size[i][1],
                      (*iterator).position[2]);
          glTexCoord2f (0.0, 0.0);
          glVertex3f ((*iterator).position[0]+Splot_EnemyBullets::bullet_size[i][0],
                      (*iterator).position[1]-Splot_EnemyBullets::bullet_size[i][1],
                      (*iterator).position[2]);
          glTexCoord2f (0.0, 1.0);
          glVertex3f ((*iterator).position[0]+Splot_EnemyBullets::bullet_size[i][0],
                      (*iterator).position[1]+Splot_EnemyBullets::bullet_size[i][1],
                      (*iterator).position[2]);
          break;
      } // end SWITCH
    } // end FOR
    glEnd ();
  } // end FOR
}