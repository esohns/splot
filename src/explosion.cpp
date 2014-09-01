#include "stdafx.h"

#include "explosion.h"

#include <string>

#include "ace/Default_Constants.h"
#include "ace/OS_Memory.h"

#include "common.h"
#include "state.h"
#include "player_aircraft.h"
#include "image.h"

// init statics
unsigned int Explosion_t::count = 0;

Splot_Explosions::Splot_Explosions ()
{
  for (int i = 0; i < MAX_EXPLOSION_TYPES; i++)
  {
    tex_[i]  = 0;
    exploSize_[i][0] = 0.5;
    exploSize_[i][1] = 0.5;
    exploStay_[i] = 20.0;
    exploPause_[i][0] = 0.0;
    exploPause_[i][1] = 0.0;
    exploPause_[i][2] = 0.0;
  } // end FOR
  //exploPool = new Explo ();

  loadTextures ();

  exploSize_[EXPLOSION_ENEMY_DESTROYED][0] = 1.35F;
  exploSize_[EXPLOSION_ENEMY_DESTROYED][1] = 1.35F;
  exploStay_[EXPLOSION_ENEMY_DESTROYED] = 30.0;

  exploSize_[EXPLOSION_ENEMY_DAMAGED][0] = 1.0;
  exploSize_[EXPLOSION_ENEMY_DAMAGED][1] = 1.0;
  exploStay_[EXPLOSION_ENEMY_DAMAGED] = 20.0;
  exploPause_[EXPLOSION_ENEMY_DAMAGED][1] = 3;

  exploSize_[EXPLOSION_ENEMY_AMMUNITION_0][0] = 1.5;
  exploSize_[EXPLOSION_ENEMY_AMMUNITION_0][1] = 1.5;
  exploStay_[EXPLOSION_ENEMY_AMMUNITION_0] = 15.0;
  exploPause_[EXPLOSION_ENEMY_AMMUNITION_0][1] = 1;

  exploSize_[EXPLOSION_ENEMY_AMMUNITION_1][0] = 0.5;
  exploSize_[EXPLOSION_ENEMY_AMMUNITION_1][1] = 0.5;
  exploStay_[EXPLOSION_ENEMY_AMMUNITION_1] = 10.0;
  exploPause_[EXPLOSION_ENEMY_AMMUNITION_1][1] = 3;

  exploSize_[EXPLOSION_ENEMY_AMMUNITION_2][0] = 1.7F;
  exploSize_[EXPLOSION_ENEMY_AMMUNITION_2][1] = 1.7F;
  exploStay_[EXPLOSION_ENEMY_AMMUNITION_2] = 10.0;
  exploPause_[EXPLOSION_ENEMY_AMMUNITION_2][1] = 2;

  exploSize_[EXPLOSION_ENEMY_AMMUNITION_3][0] = 1.7F;
  exploSize_[EXPLOSION_ENEMY_AMMUNITION_3][1] = 1.7F;
  exploStay_[EXPLOSION_ENEMY_AMMUNITION_3] = 10.0;
  exploPause_[EXPLOSION_ENEMY_AMMUNITION_3][1] = 2;

  exploSize_[EXPLOSION_ENEMY_AMMUNITION_4][0] = 2.0;
  exploSize_[EXPLOSION_ENEMY_AMMUNITION_4][1] = 1.5;
  exploStay_[EXPLOSION_ENEMY_AMMUNITION_4] = 10.0;
  exploPause_[EXPLOSION_ENEMY_AMMUNITION_4][1] = 5;

  exploSize_[EXPLOSION_PLAYER_DESTROYED][0] = 1.5;
  exploSize_[EXPLOSION_PLAYER_DESTROYED][1] = 1.5;
  exploStay_[EXPLOSION_PLAYER_DESTROYED] = 25.0;

  exploSize_[EXPLOSION_PLAYER_DAMAGED][0] = 1.1F;
  exploSize_[EXPLOSION_PLAYER_DAMAGED][1] = 1.1F;
  exploStay_[EXPLOSION_PLAYER_DAMAGED] = 25.0;
  exploPause_[EXPLOSION_PLAYER_DAMAGED][1] = 3;

  exploSize_[EXPLOSION_PLAYER_AMMUNITION_0][0] = 0.25;
  exploSize_[EXPLOSION_PLAYER_AMMUNITION_0][1] = 0.25;
  exploStay_[EXPLOSION_PLAYER_AMMUNITION_0] = 10.0;

  exploSize_[EXPLOSION_PLAYER_AMMUNITION_1][0] = 0.5;
  exploSize_[EXPLOSION_PLAYER_AMMUNITION_1][1] = 1.0;
  exploStay_[EXPLOSION_PLAYER_AMMUNITION_1] = 15.0;
  exploPause_[EXPLOSION_PLAYER_AMMUNITION_1][1] = 1;

  exploSize_[EXPLOSION_PLAYER_AMMUNITION_2][0] = 0.9F;
  exploSize_[EXPLOSION_PLAYER_AMMUNITION_2][1] = 1.0;
  exploStay_[EXPLOSION_PLAYER_AMMUNITION_2] = 23.0;

  exploSize_[EXPLOSION_PLAYER_SHIELD][0] = 1.6F;
  exploSize_[EXPLOSION_PLAYER_SHIELD][1] = 1.6F;
  exploStay_[EXPLOSION_PLAYER_SHIELD] = 25.0;
  exploPause_[EXPLOSION_PLAYER_SHIELD][1] = 5;

  exploSize_[EXPLOSION_POWER_BURST][0] = 1.8F;
  exploSize_[EXPLOSION_POWER_BURST][1] = 1.8F;
  exploStay_[EXPLOSION_POWER_BURST] = 35.0;

  exploSize_[EXPLOSION_SHIP_ADD][0] = 2.5;
  exploSize_[EXPLOSION_SHIP_ADD][1] = 2.5;
  exploStay_[EXPLOSION_SHIP_ADD] = 25.0;

  exploSize_[EXPLOSION_SHIP_LOSE][0] = 3.5;
  exploSize_[EXPLOSION_SHIP_LOSE][1] = 3.5;
  exploStay_[EXPLOSION_SHIP_LOSE] = 35.0;

  exploSize_[EXPLOSION_SHIP_SCORE][0] = 3.5;
  exploSize_[EXPLOSION_SHIP_SCORE][1] = 3.5;
  exploStay_[EXPLOSION_SHIP_SCORE] = 35.0;

  exploSize_[EXPLOSION_EFFECT_ELECTRIC][0] = 1.7F;
  exploSize_[EXPLOSION_EFFECT_ELECTRIC][1] = 0.5;
  exploStay_[EXPLOSION_EFFECT_ELECTRIC] = 43.0;
  exploPause_[EXPLOSION_EFFECT_ELECTRIC][1] = 0;

  exploSize_[EXPLOSION_EFFECT_GLITTER][0] = 0.8F;
  exploSize_[EXPLOSION_EFFECT_GLITTER][1] = 1.0;
  exploStay_[EXPLOSION_EFFECT_GLITTER] = 20.0;
  exploPause_[EXPLOSION_EFFECT_GLITTER][1] = 0;
}

Splot_Explosions::~Splot_Explosions()
{
  //clear (); //-- move all Splot_Explosions into the pool

  //cur = exploPool->next;
  //while (cur)
  //{
	 // del = cur;
	 // cur = cur->next;
	 // delete del;
  //}

  //for (int i = 0; i < (int)MAX_EXPLOSION_TYPES; i++)
  //  explosions_[i].clear ();

  deleteTextures ();
}

void
Splot_Explosions::loadTextures ()
{
  std::string path_base = ACE_TEXT_ALWAYS_CHAR (SPLOT_IMAGE_DATA_DIR);
  path_base += ACE_DIRECTORY_SEPARATOR_STR;
  std::string filename = path_base + ACE_TEXT_ALWAYS_CHAR ("enemyExplo.png");
  tex_[EXPLOSION_ENEMY_DESTROYED]     = Splot_Image::load (dataLoc (filename), IMG_NOMIPMAPS, IMG_ALPHA, GL_CLAMP, GL_NEAREST, GL_LINEAR);
  tex_[EXPLOSION_ENEMY_DAMAGED]       = tex_[EXPLOSION_ENEMY_DESTROYED];
  filename = path_base + ACE_TEXT_ALWAYS_CHAR ("enemyAmmoExplo00.png");
  tex_[EXPLOSION_ENEMY_AMMUNITION_0]  = Splot_Image::load (dataLoc (filename));
  filename = path_base + ACE_TEXT_ALWAYS_CHAR ("enemyAmmoExplo01.png");
  tex_[EXPLOSION_ENEMY_AMMUNITION_1]  = Splot_Image::load (dataLoc (filename), IMG_NOMIPMAPS, IMG_ALPHA, GL_CLAMP, GL_NEAREST, GL_LINEAR);
  filename = path_base + ACE_TEXT_ALWAYS_CHAR ("enemyAmmoExplo02.png");
  tex_[EXPLOSION_ENEMY_AMMUNITION_2]  = Splot_Image::load (dataLoc (filename));
  filename = path_base + ACE_TEXT_ALWAYS_CHAR ("enemyAmmoExplo03.png");
  tex_[EXPLOSION_ENEMY_AMMUNITION_3]  = Splot_Image::load (dataLoc (filename));
  filename = path_base + ACE_TEXT_ALWAYS_CHAR ("enemyAmmoExplo04.png");
  tex_[EXPLOSION_ENEMY_AMMUNITION_4]  = Splot_Image::load (dataLoc (filename));
  filename = path_base + ACE_TEXT_ALWAYS_CHAR ("enemyExplo.png");
  tex_[EXPLOSION_PLAYER_DESTROYED]    = Splot_Image::load (dataLoc (filename), IMG_NOMIPMAPS, IMG_ALPHA, GL_CLAMP, GL_NEAREST, GL_LINEAR);
  tex_[EXPLOSION_PLAYER_DAMAGED]      = tex_[EXPLOSION_PLAYER_DESTROYED];
  filename = path_base + ACE_TEXT_ALWAYS_CHAR ("heroAmmoExplo00.png");
  tex_[EXPLOSION_PLAYER_AMMUNITION_0] = Splot_Image::load (dataLoc (filename), IMG_NOMIPMAPS, IMG_ALPHA, GL_CLAMP, GL_NEAREST, GL_NEAREST);
  filename = path_base + ACE_TEXT_ALWAYS_CHAR ("heroAmmoExplo01.png");
  tex_[EXPLOSION_PLAYER_AMMUNITION_1] = Splot_Image::load (dataLoc (filename), IMG_NOMIPMAPS, IMG_ALPHA, GL_CLAMP, GL_NEAREST, GL_LINEAR);
  filename = path_base + ACE_TEXT_ALWAYS_CHAR ("heroAmmoExplo02.png");
  tex_[EXPLOSION_PLAYER_AMMUNITION_2] = Splot_Image::load (dataLoc (filename), IMG_NOMIPMAPS, IMG_ALPHA, GL_CLAMP, GL_NEAREST, GL_LINEAR);
  filename = path_base + ACE_TEXT_ALWAYS_CHAR ("heroShields.png");
  tex_[EXPLOSION_PLAYER_SHIELD]       = Splot_Image::load (dataLoc (filename), IMG_NOMIPMAPS, IMG_BLEND3, GL_CLAMP, GL_LINEAR, GL_LINEAR);
  //tex_[EXPLOSION_PLAYER_SHIELD] = Splot_Image::load(dataLoc(filename), IMG_NOMIPMAPS, IMG_BLEND1, GL_CLAMP, GL_LINEAR, GL_LINEAR);
  filename = path_base + ACE_TEXT_ALWAYS_CHAR ("powerUpTex.png");
  tex_[EXPLOSION_POWER_BURST]         = Splot_Image::load (dataLoc (filename), IMG_NOMIPMAPS, IMG_BLEND3, GL_CLAMP, GL_LINEAR, GL_LINEAR);
  filename = path_base + ACE_TEXT_ALWAYS_CHAR ("life.png");
  tex_[EXPLOSION_SHIP_ADD]            = Splot_Image::load (dataLoc (filename));
  tex_[EXPLOSION_SHIP_LOSE]           = tex_[EXPLOSION_SHIP_ADD];
  tex_[EXPLOSION_SHIP_SCORE]          = tex_[EXPLOSION_SHIP_ADD];
  filename = path_base + ACE_TEXT_ALWAYS_CHAR ("electric.png");
  tex_[EXPLOSION_EFFECT_ELECTRIC]     = Splot_Image::load (dataLoc (filename), IMG_NOMIPMAPS, IMG_BLEND3, GL_REPEAT, GL_LINEAR, GL_LINEAR);
  filename = path_base + ACE_TEXT_ALWAYS_CHAR ("glitter.png");
  tex_[EXPLOSION_EFFECT_GLITTER]      = Splot_Image::load (dataLoc (filename));
}

void
Splot_Explosions::deleteTextures ()
{
  glDeleteTextures (MAX_EXPLOSION_TYPES, tex_);
}

void
Splot_Explosions::clear ()
{
  for (int i = 0; i < MAX_EXPLOSION_TYPES; i++)
  {
    for (ExplosionsIterator_t iterator = explosions_[i].begin ();
         iterator != explosions_[i].end ();
         iterator++)
      delete (*iterator);
    explosions_[i].clear ();
  } // end FOR
}

//Explo*
//Splot_Explosions::getNewExplo ()
//{
//	Explo *retExplo;
//	if(exploPool->next)
//	{
//		retExplo = exploPool->next;
//		exploPool->next = retExplo->next;
//	}
//	else
//	{
//		retExplo = new Explo();
//	}
//	return retExplo;
//}

//void Splot_Explosions::killExplo(Explo *dead)
//{
//	dead->back = 0;
//	dead->next = exploPool->next;
//	exploPool->next = dead;
//}

Explosion_t*
Splot_Explosions::add (ExplosionType_t type_in,
                       const float (&position_in)[3],
                       int age_in,
                       float size_in)
{
  Explosion_t* result = NULL;

  ACE_ASSERT ((int)type_in >= 0 && type_in < MAX_EXPLOSION_TYPES);
  if ((exploPause_[type_in][0]) <= 0)
  {
    exploPause_[type_in][2] = 1; //-- set flag to init explo pause count
    ACE_NEW_RETURN (result,
                    Explosion_t (),
                    NULL);
    result->init (position_in, age_in, size_in);
    explosions_[type_in].push_back (result);
  } // end IF

  return result;
}

Explosion_t*
Splot_Explosions::addEffect (ExplosionType_t type_in,
                             const float (&position_in)[3],
                             const float (&velocity_in)[3],
                             const float (&color_in)[4],
                             int age_in,
                             float size_in)
{
  Explosion_t* result = NULL;

  ACE_ASSERT (type_in == EXPLOSION_EFFECT_ELECTRIC ||
              type_in == EXPLOSION_EFFECT_GLITTER);
  exploPause_[type_in][2] = 1; //-- set flag to init explo pause count
  ACE_NEW_RETURN (result,
                  Explosion_t (),
                  NULL);
  result->init (position_in, velocity_in, color_in, age_in, size_in);
  explosions_[type_in].push_back (result);

  return result;
}

void
Splot_Explosions::update ()
{
  State_t& state = SPLOT_STATE_SINGLETON::instance ()->get ();

  for (int i = 0; i < MAX_EXPLOSION_TYPES; i++)
  {
    if (exploPause_[i][0] > 0.0)
      exploPause_[i][0] -= state.speed_adj;
    else
      exploPause_[i][0] = 0.0;
    if (exploPause_[i][2]) //-- if flag was set, init count
    {
      exploPause_[i][0] = exploPause_[i][1];
      exploPause_[i][2] = 0.0;
    } // end IF

    std::vector<ExplosionsIterator_t> finished;
    for (ExplosionsIterator_t iterator = explosions_[i].begin ();
         iterator != explosions_[i].end ();
         iterator++)
    {
      if (++(*iterator)->age > 0)
      {
        (*iterator)->position[0] += (*iterator)->velocity[0]*state.speed_adj;
        (*iterator)->position[1] += (*iterator)->velocity[1]*state.speed_adj;
        (*iterator)->position[2] += (*iterator)->velocity[2]*state.speed_adj;
      } // end IF

      if ((*iterator)->age > (exploStay_[i]/state.speed_adj))
        finished.push_back (iterator);
    } // end FOR

    for (std::vector<ExplosionsIterator_t>::reverse_iterator iterator = finished.rbegin ();
         iterator != finished.rend ();
         iterator++)
    {
      delete (**iterator);
      explosions_[i].erase (*iterator);
    } // end FOR
  } // end FOR
}

void
Splot_Explosions::drawGL ()
{
  draw         (EXPLOSION_ENEMY_DESTROYED);
  draw         (EXPLOSION_ENEMY_DAMAGED);
  drawAmmo     (EXPLOSION_ENEMY_AMMUNITION_0);
  drawAmmo     (EXPLOSION_ENEMY_AMMUNITION_1);
  drawAmmo     (EXPLOSION_ENEMY_AMMUNITION_2);
  drawAmmo     (EXPLOSION_ENEMY_AMMUNITION_3);
  drawAmmo     (EXPLOSION_ENEMY_AMMUNITION_4);

  draw         (EXPLOSION_PLAYER_DESTROYED);
  draw         (EXPLOSION_PLAYER_DAMAGED);
  drawAmmo     (EXPLOSION_PLAYER_AMMUNITION_0);
  drawAmmo     (EXPLOSION_PLAYER_AMMUNITION_1);
  drawAmmo     (EXPLOSION_PLAYER_AMMUNITION_2);

  drawShields  (EXPLOSION_PLAYER_SHIELD);
  drawBurst    (EXPLOSION_POWER_BURST);

  drawLife     (EXPLOSION_SHIP_ADD);
  drawLife     (EXPLOSION_SHIP_LOSE);
  drawLife     (EXPLOSION_SHIP_SCORE);

  drawElectric (EXPLOSION_EFFECT_ELECTRIC);
  drawGlitter  (EXPLOSION_EFFECT_GLITTER);
}

void
Splot_Explosions::draw (ExplosionType_t type_in)
{
  // sanity check(s)
  ACE_ASSERT (type_in >= 0 &&
              (type_in < MAX_EXPLOSION_TYPES));
  if (explosions_[type_in].empty ())
    return;

  glColor4f (1.0, 1.0, 1.0, 1.0);
  glBindTexture (GL_TEXTURE_2D, tex_[type_in]);
  glBegin (GL_QUADS);

  State_t& state = SPLOT_STATE_SINGLETON::instance ()->get ();
  float age;
  float ex, ey;
  float exs, eys;
  float* p;
  float clr, tmp;
  float xoff, yoff;
  for (ExplosionsIterator_t iterator = explosions_[type_in].begin ();
       iterator != explosions_[type_in].end ();
       iterator++)
  {
    age = (*iterator)->age*state.speed_adj;
    if (age < 0.0F)
      continue;

    clr = age/exploStay_[type_in];
    ex = (*iterator)->size*exploSize_[type_in][0]*clr;
    ey = (*iterator)->size*exploSize_[type_in][1]*clr;
    //			ex = thisExplo->size*exploSize_[type][0]*((thisExplo->age)/(exploStay_[type]));
    //			ey = thisExplo->size*exploSize_[type][1]*((thisExplo->age)/(exploStay_[type]));
    exs = ex*0.7F;
    eys = ey*0.7F;
    tmp = clr*clr;
    clr = 1.2F-clr;
    tmp = 0.5F+clr*0.5F;
    glColor4f (tmp, tmp, tmp, clr);

    if (type_in == EXPLOSION_PLAYER_DAMAGED)
      p = state.player->position_;
    else
      p = (*iterator)->position;

    if (type_in == EXPLOSION_ENEMY_DESTROYED)
    {
      xoff = 0.1F;
      yoff = 0.3F;
      glTexCoord2f (0.0, 1.0); glVertex3f (p[0]-exs+xoff, p[1]+eys+yoff, p[2]);
      glTexCoord2f (0.0, 0.0); glVertex3f (p[0]-exs+xoff, p[1]-eys+yoff, p[2]);
      glTexCoord2f (1.0, 0.0); glVertex3f (p[0]+exs+xoff, p[1]-eys+yoff, p[2]);
      glTexCoord2f (1.0, 1.0); glVertex3f (p[0]+exs+xoff, p[1]+eys+yoff, p[2]);

      xoff = -0.2F;
      yoff = -0.4F;
      glTexCoord2f (0.0, 1.0); glVertex3f (p[0]-exs+xoff, p[1]+eys+yoff, p[2]);
      glTexCoord2f (0.0, 0.0); glVertex3f (p[0]-exs+xoff, p[1]-eys+yoff, p[2]);
      glTexCoord2f (1.0, 0.0); glVertex3f (p[0]+exs+xoff, p[1]-eys+yoff, p[2]);
      glTexCoord2f (1.0, 1.0); glVertex3f (p[0]+exs+xoff, p[1]+eys+yoff, p[2]);
    } // end IF
    xoff =  0.0F;
    yoff = -0.3F;
    glTexCoord2f (0.0, 1.0); glVertex3f (p[0]-ex+xoff, p[1]+ey+yoff, p[2]);
    glTexCoord2f (0.0, 0.0); glVertex3f (p[0]-ex+xoff, p[1]-ey+yoff, p[2]);
    glTexCoord2f (1.0, 0.0); glVertex3f (p[0]+ex+xoff, p[1]-ey+yoff, p[2]);
    glTexCoord2f (1.0, 1.0); glVertex3f (p[0]+ex+xoff, p[1]+ey+yoff, p[2]);
  } // end FOR

  glEnd ();
}

void
Splot_Explosions::drawAmmo (ExplosionType_t type_in)
{
  // sanity check(s)
  ACE_ASSERT (type_in >= 0 &&
              (type_in < MAX_EXPLOSION_TYPES));
  if (explosions_[type_in].empty ())
    return;

  glBindTexture (GL_TEXTURE_2D, tex_[type_in]);
  glBegin (GL_QUADS);

  State_t& state = SPLOT_STATE_SINGLETON::instance ()->get ();
  float age;
  float ex, ey;
  float clr;
  for (ExplosionsIterator_t iterator = explosions_[type_in].begin ();
       iterator != explosions_[type_in].end ();
       iterator++)
  {
    age = (*iterator)->age*state.speed_adj;
    ex = exploSize_[type_in][0]*((age+5.0f)/(exploStay_[type_in]+5.0f));
    ey = exploSize_[type_in][1]*((age+5.0f)/(exploStay_[type_in]+5.0f));
    clr = age/exploStay_[type_in];
    if ((clr = (1.2F-clr)) > 1.0F)
      clr = 1.0;
    glColor4f (1.0, 1.0, 1.0, clr);
    glTexCoord2f (0.0, 0.0); glVertex3f ((*iterator)->position[0]-ex, (*iterator)->position[1]+ey, (*iterator)->position[2]);
    glTexCoord2f (0.0, 1.0); glVertex3f ((*iterator)->position[0]-ex, (*iterator)->position[1]-ey, (*iterator)->position[2]);
    glTexCoord2f (1.0, 1.0); glVertex3f ((*iterator)->position[0]+ex, (*iterator)->position[1]-ey, (*iterator)->position[2]);
    glTexCoord2f (1.0, 0.0); glVertex3f ((*iterator)->position[0]+ex, (*iterator)->position[1]+ey, (*iterator)->position[2]);
  } // end FOR

  glEnd ();
}

void
Splot_Explosions::drawBurst (ExplosionType_t type_in)
{
  // sanity check(s)
  ACE_ASSERT (type_in >= 0 &&
              (type_in < MAX_EXPLOSION_TYPES));
  if (explosions_[type_in].empty ())
    return;

  glBindTexture (GL_TEXTURE_2D, tex_[type_in]);

  State_t& state = SPLOT_STATE_SINGLETON::instance ()->get ();
  float age;
  float ex, ey;
  float clr, tmp;
  for (ExplosionsIterator_t iterator = explosions_[type_in].begin ();
       iterator != explosions_[type_in].end ();
       iterator++)
  {
    age = (*iterator)->age*state.speed_adj;
    tmp = 1.0F-(age/exploStay_[type_in]);
    ex = (*iterator)->size*exploSize_[type_in][0]*tmp;
    ey = (*iterator)->size*exploSize_[type_in][1]*tmp;
    clr = tmp*0.75F;
    glColor4f (clr+0.5F, clr+0.2F, clr+0.1F, clr);
    glPushMatrix ();
    glTranslatef ((*iterator)->position[0], (*iterator)->position[1], (*iterator)->position[2]);
    glRotatef ((float)IRAND, 0.0, 0.0, 1.0);
    glBegin (GL_QUADS);
    glTexCoord2f (0.0, 0.0); glVertex3f (-ex,  ey, 0.0);
    glTexCoord2f (0.0, 1.0); glVertex3f (-ex, -ey, 0.0);
    glTexCoord2f (1.0, 1.0); glVertex3f ( ex, -ey, 0.0);
    glTexCoord2f (1.0, 0.0); glVertex3f ( ex,  ey, 0.0);
    glEnd ();
    glRotatef ((float)IRAND, 0.0, 0.0, 1.0);
    glBegin (GL_QUADS);
    glTexCoord2f (0.0, 0.0); glVertex3f (-ex,  ey, 0.0);
    glTexCoord2f (0.0, 1.0); glVertex3f (-ex, -ey, 0.0);
    glTexCoord2f (1.0, 1.0); glVertex3f ( ex, -ey, 0.0);
    glTexCoord2f (1.0, 0.0); glVertex3f ( ex,  ey, 0.0);
    glEnd ();
    glPopMatrix ();
  } // end FOR
}

void
Splot_Explosions::drawShields (ExplosionType_t type_in)
{
  State_t& state = SPLOT_STATE_SINGLETON::instance ()->get ();

  // sanity check(s)
  ACE_ASSERT (type_in >= 0 &&
              (type_in < MAX_EXPLOSION_TYPES));
  if (explosions_[type_in].empty () ||
      !state.player->isVisible ())
    return;

  glBindTexture (GL_TEXTURE_2D, tex_[type_in]);

  float age;
  float ex, ey;
  float clr, tmp;
  for (ExplosionsIterator_t iterator = explosions_[type_in].begin ();
       iterator != explosions_[type_in].end ();
       iterator++)
  {
    age = (*iterator)->age*state.speed_adj;
    tmp = age/exploStay_[type_in];
    clr = 1.0F-tmp;
    tmp = 0.5F+(clr*0.5F);
    ex = exploSize_[type_in][0]*tmp;
    ey = exploSize_[type_in][1]*tmp;
    glColor4f (clr, clr, 1.0, clr*0.7F);
    glPushMatrix ();
    glTranslatef (state.player->position_[0], state.player->position_[1], state.player->position_[2]);
    glRotatef ((float)IRAND, 0.0, 0.0, 1.0);
    glBegin (GL_QUADS);
    glTexCoord2f (0.0, 0.0); glVertex3f (-ex,  ey, 0.0);
    glTexCoord2f (0.0, 1.0); glVertex3f (-ex, -ey, 0.0);
    glTexCoord2f (1.0, 1.0); glVertex3f ( ex, -ey, 0.0);
    glTexCoord2f (1.0, 0.0); glVertex3f ( ex,  ey, 0.0);
    glEnd ();
    glPopMatrix ();
  } // end FOR
}

void
Splot_Explosions::drawLife (ExplosionType_t type_in)
{
  // sanity check(s)
  ACE_ASSERT (type_in >= 0 &&
              (type_in < MAX_EXPLOSION_TYPES));
  if (explosions_[type_in].empty ())
      return;

  glBindTexture (GL_TEXTURE_2D, tex_[type_in]);
  glBegin (GL_QUADS);

  State_t& state = SPLOT_STATE_SINGLETON::instance ()->get ();
  float age;
  float ex, ey;
  float clr[4] = {1.0, 1.0, 1.0, 1.0};
  float tmp;
  for (ExplosionsIterator_t iterator = explosions_[type_in].begin ();
       iterator != explosions_[type_in].end ();
       iterator++)
  {
    age = (*iterator)->age*state.speed_adj;
    if (age < 0.0)
      continue;

    tmp = age/exploStay_[type_in];
    switch (type_in)
    {
      case EXPLOSION_SHIP_ADD:
        clr[0] = clr[1] = tmp;
        clr[2] = 1.0;
        clr[3] = tmp+0.2F;
        tmp = 1.0F-tmp;
        break;
      case EXPLOSION_SHIP_LOSE:
        clr[0] = 1.0;
        clr[1] = clr[2] = 0.1F;
        clr[3] = 1.0F-tmp;
        break;
      case EXPLOSION_SHIP_SCORE:
        clr[0] = clr[1] = clr[2] = 1.0;
        clr[3] = 1.0F-tmp;
        break;
      default:
      {
        ACE_DEBUG ((LM_ERROR,
                    ACE_TEXT ("invalid/unknown explosion type (was: %d), continuing\n"),
                    type_in));
        continue;
      }
    } // end SWITCH
    ex = (*iterator)->size*exploSize_[type_in][0]*tmp;
    ey = (*iterator)->size*exploSize_[type_in][1]*tmp;
    glColor4fv (clr);
    glTexCoord2f (0.0, 1.0); glVertex3f ((*iterator)->position[0]-ex, (*iterator)->position[1]+ey, (*iterator)->position[2]);
    glTexCoord2f (0.0, 0.0); glVertex3f ((*iterator)->position[0]-ex, (*iterator)->position[1]-ey, (*iterator)->position[2]);
    glTexCoord2f (1.0, 0.0); glVertex3f ((*iterator)->position[0]+ex, (*iterator)->position[1]-ey, (*iterator)->position[2]);
    glTexCoord2f (1.0, 1.0); glVertex3f ((*iterator)->position[0]+ex, (*iterator)->position[1]+ey, (*iterator)->position[2]);
  } // end FOR

  glEnd ();
}

void
Splot_Explosions::drawElectric (ExplosionType_t type_in)
{
  // sanity check(s)
  ACE_ASSERT (type_in >= 0 &&
              (type_in < MAX_EXPLOSION_TYPES));
  if (explosions_[type_in].empty ())
      return;

  glBindTexture (GL_TEXTURE_2D, tex_[type_in]);

  State_t& state = SPLOT_STATE_SINGLETON::instance ()->get ();
  float age;
  float ex, ey;
  float alpha, tmp;
  float tOff;
  for (ExplosionsIterator_t iterator = explosions_[type_in].begin ();
       iterator != explosions_[type_in].end ();
       iterator++)
  {
    age = (*iterator)->age*state.speed_adj;
    if (age < 0.0)
      continue;

    tmp = age/exploStay_[type_in];
    alpha = 1.0F-tmp;
    alpha = 5.0F*(alpha*alpha);
    glColor4f ((*iterator)->clr[0], (*iterator)->clr[1], (*iterator)->clr[2], (*iterator)->clr[3]*alpha);
    ex = exploSize_[type_in][0];
    ey = exploSize_[type_in][1]*tmp;
    tmp = (1.0F-state.speed_adj)+(state.speed_adj*1.075F);
    (*iterator)->velocity[0] *= tmp;
    (*iterator)->velocity[1] *= tmp;
    (*iterator)->velocity[2] *= tmp;
    tOff = FRAND;
    glPushMatrix ();
    glTranslatef ((*iterator)->position[0], (*iterator)->position[1], (*iterator)->position[2]);
    glBegin (GL_QUADS);
    glTexCoord2f (0.0, 0.0F+tOff); glVertex3f (-ex,  ey, 0.0);
    glTexCoord2f (0.0, 0.2F+tOff); glVertex3f (-ex, -ey, 0.0);
    glTexCoord2f (1.0, 0.2F+tOff); glVertex3f ( ex, -ey, 0.0);
    glTexCoord2f (1.0, 0.0F+tOff); glVertex3f ( ex,  ey, 0.0);
    glEnd ();
    glPopMatrix ();
  } // end FOR
}

void
Splot_Explosions::drawGlitter (ExplosionType_t type_in)
{
  // sanity check(s)
  ACE_ASSERT (type_in >= 0 &&
              (type_in < MAX_EXPLOSION_TYPES));
  if (explosions_[type_in].empty ())
    return;

  glBindTexture (GL_TEXTURE_2D, tex_[type_in]);

  State_t& state = SPLOT_STATE_SINGLETON::instance ()->get ();
  float age;
  float ex, ey;
  float alpha, tmp;
  for (ExplosionsIterator_t iterator = explosions_[type_in].begin ();
       iterator != explosions_[type_in].end ();
       iterator++)
  {
    age = (*iterator)->age*state.speed_adj;
    if (age < 0.0)
      continue;

    tmp = age/exploStay_[type_in];
    alpha = 1.0F-tmp;
    //alpha = 5.0*(alpha*alpha);
    glColor4f ((*iterator)->clr[0], (*iterator)->clr[1], (*iterator)->clr[2], (*iterator)->clr[3]*alpha);
    tmp = alpha*alpha;
    ex = tmp*(*iterator)->size*exploSize_[type_in][0];
    ey = tmp*(*iterator)->size*exploSize_[type_in][1]+(0.02F*age);
    glPushMatrix ();
    glTranslatef ((*iterator)->position[0], (*iterator)->position[1], (*iterator)->position[2]);
    glBegin (GL_QUADS);
    glTexCoord2f (0.0, 0.0); glVertex3f (-ex,  ey, 0.0);
    glTexCoord2f (0.0, 1.0); glVertex3f (-ex, -ey, 0.0);
    glTexCoord2f (1.0, 1.0); glVertex3f ( ex, -ey, 0.0);
    glTexCoord2f (1.0, 0.0); glVertex3f ( ex,  ey, 0.0);
    glEnd ();
    glPopMatrix ();
  } // end FOR
}

/////////////////////////////////////////
void
Explosion_t::init (const float (&position_in)[3],
                   int age_in,
                   float size_in)
{
  position[0] = position_in[0]; position[1] = position_in[1]; position[2] = position_in[2];
  velocity[0] = 0.0; velocity[1] = 0.0; velocity[2] = 0.0;
  clr[0] = 1.0; clr[1] = 1.0; clr[2] = 1.0; clr[3] = 1.0;
  State_t& state = SPLOT_STATE_SINGLETON::instance ()->get ();
  age = (int)(age_in/state.speed_adj);
  size = size_in;
}

void
Explosion_t::init (const float (&position_in)[3],
                   const float (&velocity_in)[3],
                   const float (&color_in)[4],
                   int age_in,
                   float size_in)
{
  position[0] = position_in[0]; position[1] = position_in[1]; position[2] = position_in[2];
  velocity[0] = velocity_in[0]; velocity[1] = velocity_in[1]; velocity[2] = velocity_in[2];
  clr[0] = color_in[0]; clr[1] = color_in[1]; clr[2] = color_in[2]; clr[3] = color_in[3];
  State_t& state = SPLOT_STATE_SINGLETON::instance ()->get ();
  age = (int)(age_in/state.speed_adj);
  size = size_in;
}
