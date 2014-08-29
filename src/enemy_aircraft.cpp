#include "stdafx.h"

#include "enemy_aircraft.h"

//#ifdef HAVE_CONFIG_H
//#include <chromium-bsu-config.h>
//#endif

#include "ace/OS.h"
#include "ace/Log_Msg.h"

//#include "gettext.h"
//
//#include "Splot_EnemyAircraft.h"
//
//#include <cstdio>
//#include <cstdlib>
//#include <cmath>
//
#include "configuration.h"
//
//#include "define.h"
//#include "Ammo.h"
#include "state.h"
//#include "EnemyAmmo.h"
#include "player_aircraft.h"
//#include "ScreenItemAdd.h"
//
//#include "Splot_EnemyAircraft_Straight.h"
//#include "Splot_EnemyAircraft_Omni.h"
//#include "Splot_EnemyAircraft_RayGun.h"
//#include "Splot_EnemyAircraft_Tank.h"
//#include "Splot_EnemyAircraft_Gnat.h"
//#include "Splot_EnemyAircraft_Boss00.h"
//#include "Splot_EnemyAircraft_Boss01.h"

unsigned int Splot_EnemyAircraft::count = 0;

Splot_EnemyAircraft::Splot_EnemyAircraft()
 : inherited ()
 , type_ (ENEMYAIRCRAFT_INVALID)
{
  inherited::type_ = GAMEELEMENT_ENEMY;

  float p[3] = { 0.0, 0.0, 0.0 };
  reset (p);

  count++;
}

Splot_EnemyAircraft::Splot_EnemyAircraft (EnemyAircraftType_t type_in,
                                          float position_in[3],
                                          float randFact)
 : inherited (),
   type_ (type_in)
{
  inherited::type_ = GAMEELEMENT_ENEMY;

  reset (position_in, randFact);

  const Configuration_t& configuration =
    SPLOT_CONFIGURATION_SINGLETON::instance ()->get ();
  float xBound = configuration.screen_bound[0]-2.0F;
  if (inherited::position_[0] < -xBound)
    inherited::position_[0] = -xBound;
  if (inherited::position_[0] > xBound)
    inherited::position_[0] = xBound;

  count++;
}

Splot_EnemyAircraft::~Splot_EnemyAircraft ()
{
  count--;
}

void
Splot_EnemyAircraft::printNumAllocated ()
{
  ACE_DEBUG ((LM_DEBUG,
              ACE_TEXT ("enemy count: %u (total elements: %u)\n"),
              count, inherited::count));
}

void
Splot_EnemyAircraft::drawGL (GLuint tex_in, GLuint xtraTex_in)
{
  glBindTexture (GL_TEXTURE_2D, tex_in);
  glColor4f (1.0, 1.0, 1.0, 1.0);
  glPushMatrix ();
  glTranslatef (position_[0], position_[1], position_[2]);
  glBegin (GL_TRIANGLE_STRIP);
  glTexCoord2f (1.0, 0.0); glVertex3f ( size_[0],  size_[1], 0.0);
  glTexCoord2f (0.0, 0.0); glVertex3f (-size_[0],  size_[1], 0.0);
  glTexCoord2f (1.0, 1.0); glVertex3f ( size_[0], -size_[1], 0.0);
  glTexCoord2f (0.0, 1.0); glVertex3f (-size_[0], -size_[1], 0.0);
  glEnd ();
  glPopMatrix ();

  float szx;//, szy;
  switch (type_)
  {
    case ENEMYAIRCRAFT_STRAIGHT:
      if (preFire_)
      {
        glBlendFunc (GL_SRC_ALPHA, GL_ONE);
        glBindTexture (GL_TEXTURE_2D, xtraTex_in);
        glColor4f (1.0, 1.0, 1.0, preFire_);
        szx = 0.55F*preFire_;
        glPushMatrix ();
        glTranslatef (position_[0], position_[1]-0.9F, position_[2]);
        glRotatef ((float)IRAND, 0.0, 0.0, 1.0);
        drawQuad (szx, szx+0.1F);
        glPopMatrix ();
        glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glColor4f (1.0, 1.0, 1.0, 1.0);
      }
      break;
    case ENEMYAIRCRAFT_OMNI:
      glColor4f (1.0, 0.0, 0.0, 1.0);
      glBindTexture (GL_TEXTURE_2D, xtraTex_in);
      glPushMatrix ();
      glTranslatef (position_[0], position_[1], position_[2]);
      glRotatef ((float)-(inherited::age_*8), 0.0, 0.0, 1.0);
      drawQuad (size_[0], size_[1]);
      glPopMatrix ();
      glColor4f (1.0, 1.0, 1.0, 1.0);
      break;
    case ENEMYAIRCRAFT_TANK:
      if (preFire_)
      {
        glBlendFunc (GL_SRC_ALPHA, GL_ONE);
        glBindTexture (GL_TEXTURE_2D, xtraTex_in);
        glColor4f (1.0, 1.0, 1.0, preFire_);
        glPushMatrix ();
        glTranslatef (position_[0], position_[1]-0.63F, position_[2]); //NOTE: offset is ~szy*0.3
        glRotatef ((float)IRAND, 0.0, 0.0, 1.0);
        szx = 0.4F+(0.6F*preFire_);
        drawQuad (szx, szx);
        glPopMatrix ();
        glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glColor4f (1.0, 1.0, 1.0, 1.0);
      } // end IF
      break;
    case ENEMYAIRCRAFT_BOSS_0:
      if (preFire_)
      {
        glBlendFunc (GL_SRC_ALPHA, GL_ONE);
        glBindTexture (GL_TEXTURE_2D, xtraTex_in);
        glColor4f (1.0, 1.0, 1.0, preFire_);
        szx = 0.4F+(0.6F*preFire_);
        glPushMatrix ();
        glTranslatef (position_[0]+1.1F, position_[1]-0.4F, position_[2]);
        glRotatef ((float)IRAND, 0.0, 0.0, 1.0);
        drawQuad (szx, szx);
        glPopMatrix ();
        glPushMatrix ();
        glTranslatef (position_[0]-1.1F, position_[1]-0.4F, position_[2]);
        glRotatef ((float)IRAND, 0.0, 0.0, 1.0);
        drawQuad (szx, szx);
        glPopMatrix ();
        glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glColor4f (1.0, 1.0, 1.0, 1.0);
      } // end IF
      break;
    case ENEMYAIRCRAFT_BOSS_1:
      if (preFire_)
      {
        glBlendFunc (GL_SRC_ALPHA, GL_ONE);
        glBindTexture (GL_TEXTURE_2D, xtraTex_in);
        glColor4f (1.0, 1.0, 1.0, preFire_);
        szx = 0.9F*preFire_;
        if (shootSwap_)
        {
          glPushMatrix ();
          glTranslatef (position_[0]-1.22F, position_[1]-1.22F, position_[2]);
          glRotatef ((float)IRAND, 0.0, 0.0, 1.0);
          drawQuad (szx, szx);
          drawQuad (szx+0.2F, szx+0.2F);
          glPopMatrix ();
        } // end IF
        else
        {
          glPushMatrix ();
          glTranslatef (position_[0]+0.55F, position_[1]-1.7F, position_[2]);
          glRotatef ((float)IRAND, 0.0, 0.0, 1.0);
          drawQuad (szx, szx);
          drawQuad (szx+0.3F, szx+0.3F);
          glPopMatrix ();
        } // end ELSE
        glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glColor4f (1.0, 1.0, 1.0, 1.0);
      } // end IF
      break;
    default:
      break;
  } // end SWITCH
}

bool
Splot_EnemyAircraft::checkHit (const Bullet_t& bullet_in)
{
  if (bullet_in.position[1] > (inherited::position_[1]-size_[1]))
    if ((bullet_in.position[1] < (inherited::position_[1]+size_[1])) &&
        (bullet_in.position[0] > (inherited::position_[0]-size_[0])) &&
        (bullet_in.position[0] < (inherited::position_[0]+size_[0])))
      return true;

  return false;
}

void
Splot_EnemyAircraft::reset (float position_in[3],
                            float randomMovementFactor_in)
{
  inherited::reset ();
  // *NOTE*: assignment not possible
  //inherited::position_ = position_in;
  ACE_OS::memcpy (&(inherited::position_), 0, sizeof (position_in));

  //size_[0] = size_[1] = 0;

  //damage_ = 0.0;
  //baseDamage_ = 0.0;

  //collisionMove_ = 0.0;
  secondaryMove_[0] = secondaryMove_[1] = 0.0;

  preFire_          = 0.0;

  over_             = NULL; //-- this aircraft will be inserted after that
                            //   aircraft in the fleet list

  ACE_OS::memset (&shootVec_, 0, sizeof (shootVec_));
  shootVec_[1]      = -0.2F;
  //shootPause_ = 0;
  shootInterval_    = 1;
  shootSwap_        = 0;

  randomMoveX_      = randomMovementFactor_in*FRAND;
  lastMove_[0]      = 0.0;
  lastMove_[1]      = 0.0;

  //speedAdjustment_ = 0.0;

  const State_t& state = SPLOT_STATE_SINGLETON::instance ()->get ();
  target_           = state.player;

//	float frand = FRAND*randFact;
//	switch(type)
//	{
//		case EnemyStraight:
//			damage = baseDamage = -110.0*game->gameSkill;
//			size[0] = 0.75;
//			size[1] = 1.02;
//			collisionMove = 0.5;
//			vel[1] = -0.046-frand*0.04;
//			break;
//		case EnemyOmni:
//			damage = baseDamage = -45.0;
//			size[0] = 0.7;
//			size[1] = 0.7;
//			collisionMove = 0.7;
//			vel[1] = -0.071-frand*0.04;
////			vel[1] = -0.021-frand*0.04;
//			break;
//		case EnemyRayGun:
//			damage = baseDamage = -1000.0*game->gameSkill;
//			size[0] = 1.2;
//			size[1] = 1.2;
//			collisionMove = 0.2;
//			vel[1] =  0.03;
//			randMoveX = 0.5+0.5*randMoveX;
//			break;
//		case EnemyTank:
//			damage = baseDamage = -2000.0*game->gameSkill;
//			size[0] = 1.9;
//			size[1] = 2.1;
//			collisionMove = 0.1;
//			vel[1] =  0.03;
//			break;
//		case EnemyGnat:
//			damage = baseDamage = -10.0;
//			size[0] = 0.45;
//			size[1] = 0.45;
//			collisionMove = 0.0;
//			randMoveX = 0.5+0.5*randMoveX;
//			vel[0] = 0.2;
//			vel[1] = 0.1;
//			break;
//		case EnemyBoss00:
//			damage = baseDamage = -10000.0*game->gameSkill;
////			damage = baseDamage = -8.0*game->gameSkill;
//			size[0] = 3.5;
//			size[1] = 2.275;
//			collisionMove = 0.05;
//			vel[1] = 0.02;
//			break;
//		case EnemyBoss01:
//			damage = baseDamage = -10000.0*game->gameSkill;
////			damage = baseDamage = -8.0*game->gameSkill;
//			size[0] = 2.6;
//			size[1] = 2.3;
//			collisionMove = 0.1;
//			vel[1] = 0.02;
//			age = 600;
//			break;
//		case NumEnemyTypes:
//			break;
//	}
}

//-- *TODO*: many of the firing rates are not adjusted by game->speedAdj
//           so they will be inaccurate
void
Splot_EnemyAircraft::update ()
{
  const State_t& state = SPLOT_STATE_SINGLETON::instance ()->get ();
  inherited::position_[0] += secondaryMove_[0]*state.speed_adj;
  inherited::position_[1] += secondaryMove_[1]*state.speed_adj;
  inherited::age_++;

  float s = (1.0F-state.speed_adj)+(state.speed_adj*0.7F);
  secondaryMove_[0] *= s;
  secondaryMove_[1] *= s;
  move ();

  shootInterval_--;

  //ACE_ASSERT(target_);
  ////float	*hpos = target->position_;
  ////float	*hpos = game->hero->getPos();
  //float a = target_->position_[0] - inherited::position_[0];
  //float b = target_->position_[1] - inherited::position_[1];
  //float bullet_speed = 0.35*state.speed_adj;
  //int 	omniSwap = 108;
  //int		tmpInt;
  //float dist;
  //float	v[3] = {0.0, -0.2, 0.0};
  //Splot_EnemyAircraft *tmpAircraft = 0;
//	float	p[3] = { pos[0], pos[1], pos[2] };
//	switch(type)
//	{
//		//------------------------------- Straight
//		case EnemyStraight:	
//			if(shootInterval < 10)
//				preFire = (10-shootInterval)/10.0;
//			else
//				preFire = 0.0;
//			if(!shootInterval)
//			{
//				calcShootInterval();
//				p[1] -= 0.9;
//				game->enemyAmmo->addAmmo(0, p, shootVec);
//			}
//			break;
//		//------------------------------- Omni
//		case EnemyOmni:
//			shootSwap = shootSwap%omniSwap;
//			if(shootSwap < 18)
//			{
//				if(!(shootSwap%6))
//				{
//					//ammoSpeed = 0.22;
//					ammoSpeed = 0.3*game->gameSkill*game->speedAdj;
//					dist = fabs(a) + fabs(b);
//					//dist = sqrt(a*a+b*b);
//					v[0] = a/dist;
//					v[1] = b/dist;
//					
//					shootVec[0] = ammoSpeed*v[0];
//					shootVec[1] = ammoSpeed*v[1]/*+vel[1]*/;
//				}
//				game->enemyAmmo->addAmmo(1, p, shootVec);
//			}
//			if(pos[1] < config->screenBoundY())
//				shootSwap++;
//			break;
//		//------------------------------- Ray Gun
//		case EnemyRayGun:	
//			if(fabs(a) < 1.5) //-- big center gun
//			{
//				v[1] = -0.6;
//				p[1] = pos[1]-0.5;
//				game->enemyAmmo->addAmmo(3, p, v);
//			}
//			break;
//		//------------------------------- Tank
//		case EnemyTank:		
//			p[1] = pos[1] - 1.7;
//			if(fabs(a) < 4.0)
//			{
//				if(shootSwap == 0 || shootSwap == 8 || shootSwap == 16 )
//				{
//					v[1] = -0.2;
//					p[0] = pos[0] + 1.5;
//					game->enemyAmmo->addAmmo(0, p, v);
//					p[0] = pos[0] - 1.5;
//					game->enemyAmmo->addAmmo(0, p, v);
//				}
//				shootSwap = ++shootSwap%100;
//			}
////				else
////					shootSwap = 0;
//
//			if(!((tmpInt = age/200)%2)) //-- omni shooters
//			{
//				tmpInt = age%200;
//				if(tmpInt < 100)
//				{
//					preFire = (float)tmpInt/100.0f;
//				}
//				else if(tmpInt < 170)
//				{
//					if(!(age%10))
//					{
//						p[1] = pos[1]-0.45;
//						b = hpos[1]-p[1];
//
//						p[0] = pos[0];
//						a = hpos[0]-p[0];
//						dist = fabs(a) + fabs(b);
//						shootVec[0] = 2.0*ammoSpeed*a/dist;
//						shootVec[1] = 2.0*ammoSpeed*b/dist;
//						secondaryMove[0] -= shootVec[0]*0.1;
//						secondaryMove[1] -= shootVec[1]*0.1;
//						game->enemyAmmo->addAmmo(2, p, shootVec);
//						preFire -= 0.4;
//						if(preFire < 0.0)
//							preFire = 0.0;	
//					}
//					else
//						preFire += 0.035;
//				}
//				else
//					preFire = 0.0;
//			}
//			break;
//		//------------------------------- Gnat
//		case EnemyGnat:	
//			if(!shootInterval)
//			{
//				calcShootInterval();
//				if(fabs(a) < 2.0 && b < 0.0) //-- 
//				{
//					v[1] = -0.39;
//					p[1] = pos[1]-0.5;
//					game->enemyAmmo->addAmmo(4, p, v);
//				}
//			}
//			break;
//		//------------------------------- Boss 0
//		case EnemyBoss00:	
//			if(fabs(a) < 1.6) //-- big center gun
//			{
//				v[1] = -0.6;
//				p[1] = pos[1]-1.7;
//				game->enemyAmmo->addAmmo(3, p, v);
//			}
//			if(!(age%5)) //-- side cannons
//			{
//				shootSwap = ++shootSwap%15;
//				if(shootSwap < 6)
//				{
//					v[1] = -0.2;
//					p[1] = pos[1]-1.9;
//					p[0] = pos[0]+2.0+((shootSwap%3)*0.4);
//					game->enemyAmmo->addAmmo(0, p, v);
//					p[0] = pos[0]-2.0-((shootSwap%3)*0.4);
//					game->enemyAmmo->addAmmo(0, p, v);
//				}
//			}
//			if(!((age-1)%7))
//			{
//				dist = fabs(a) + fabs(b);
//				shootVec[0] = ammoSpeed*a/dist;
//				shootVec[1] = ammoSpeed*b/dist;
//			}
//			if(!((age/200)%2)) //-- omni shooters
//			{
//				if(!((age/100)%2))
//				{
//					if(!((age/50)%2))
//					{
//						p[1] = pos[1]-0.45;
//						p[0] = pos[0]-1.1;
//						game->enemyAmmo->addAmmo(1, p, shootVec);
//						p[0] = pos[0]+1.1;
//						game->enemyAmmo->addAmmo(1, p, shootVec);
//					}
//					preFire = (age%100)/100.0f;
//				}
//				else if(!(age%10))
//				{
//					p[1] = pos[1]-0.45;
//					b = hpos[1]-p[1];
//
//					p[0] = pos[0]-1.1;
//					a = hpos[0]-p[0];
//					dist = fabs(a) + fabs(b);
//					shootVec[0] = 2.0*ammoSpeed*a/dist;
//					shootVec[1] = 2.0*ammoSpeed*b/dist;
//					game->enemyAmmo->addAmmo(2, p, shootVec);
//					p[0] = pos[0]+1.1;
//					a = hpos[0]-p[0];
//					dist = fabs(a) + fabs(b);
//					shootVec[0] = 2.0*ammoSpeed*a/dist;
//					shootVec[1] = 2.0*ammoSpeed*b/dist;
//					game->enemyAmmo->addAmmo(2, p, shootVec);
//					preFire -= 0.4;
//					if(preFire < 0.0)
//						preFire = 0.0;
//				}
//				else
//					preFire += 0.035;	
//			}
//			else
//				preFire = 0.0;
//			break;
//		//------------------------------- Boss 1
//		case EnemyBoss01:
//			if(fabs(a) < 5.0)
//			{	
//				shootVec[1] = -0.65;
//				preFire = (age%6)/6.0;
//				if( !(age%6) )
//				{
//					shootSwap = !shootSwap;
//					if(shootSwap)
//					{
//						p[0] = pos[0]+0.55;
//						p[1] = pos[1]-1.7;
//						game->enemyAmmo->addAmmo(0, p, shootVec);
//						p[1] += 0.5;
//						game->enemyAmmo->addAmmo(0, p, shootVec);
//					}
//					else
//					{
//						p[0] = pos[0]-1.22;
//						p[1] = pos[1]-1.22;
//						game->enemyAmmo->addAmmo(0, p, shootVec);
//						p[1] += 0.5;
//						game->enemyAmmo->addAmmo(0, p, shootVec);
//					}
//				}
//			}
//			else
//			{
//				if(preFire > 0.0)	preFire -= 0.05;
//				else 				preFire = 0.0;
//			}
//			//-- add Gnats
//			if( !((age/512)%2) )
//			{
//				if( !((age/64)%2) && !(age%5) )
//				{
//					p[0] += 1.7;
//					p[1] += 1.2;
//					tmpAircraft = game->itemAdd->dynamicEnemyAdd(EnemyGnat, p, game->gameFrame+2);
//					tmpAircraft->over = this;
//					tmpAircraft->setTarget(this);
//				}
//			}
//			break;
//		case NumEnemyTypes:
//			break;
//	}
}

void
Splot_EnemyAircraft::calcShootInterval ()
{
  shootInterval_ = 1;
}

//void
//Splot_EnemyAircraft::calcShootInterval ()
//{
//  const State_t& state = SPLOT_STATE_SINGLETON::instance ()->get ();
//  switch (type_)
//  {
//    case ENEMYAIRCRAFT_STRAIGHT:
//      shootInterval_ = (int)((30.0+FRAND*90.0)/state.speed_adj);
//      break;
//    case ENEMYAIRCRAFT_GNAT:
//      shootInterval_ = (int)((1.0+FRAND*5.0)/state.speed_adj);
//      break;
//    case ENEMYAIRCRAFT_OMNI:
//    case ENEMYAIRCRAFT_RAYGUN:
//    case ENEMYAIRCRAFT_TANK:
//    case ENEMYAIRCRAFT_BOSS_0:
//    case ENEMYAIRCRAFT_BOSS_1:
//      shootInterval_ = 1;
//      break;
//    default:
//      ACE_DEBUG ((LM_ERROR,
//                  ACE_TEXT ("invalid/unknown enemy aircraft type (was: %d), returning\n"),
//                  type_));
//      return;
//  } // end SWITCH
//}

void
Splot_EnemyAircraft::move ()
{
  float* position = NULL;
  if (target_)
    position = target_->position_;
  else
    position = inherited::position_;
  float diff[2] = { position[0]-inherited::position_[0],
                    position[1]-inherited::position_[1]};
  float tmp  = 0.0;
  float tmpd = 0.0;
  float tmps = 0.0;
  float randX;
  float v0, v1;
  float x, y, tmpX, tmpY, speed;
  float dist;
  float approachDist;
  const State_t& state = SPLOT_STATE_SINGLETON::instance ()->get ();
  const Configuration_t& configuration =
    SPLOT_CONFIGURATION_SINGLETON::instance ()->get ();
  switch (type_)
  {
    case ENEMYAIRCRAFT_STRAIGHT:
      inherited::position_[1] +=
        (state.speed_adj*(inherited::velocity_[1]*state.game_skill));
      break;
    case ENEMYAIRCRAFT_OMNI:
      lastMove_[0] = (0.9F*lastMove_[0])+(0.1F*(0.01F*diff[0]));
      inherited::position_[0] +=
        (state.speed_adj*(randomMoveX_*lastMove_[0]));
      inherited::position_[1] +=
        (state.speed_adj*(inherited::velocity_[1]*state.game_skill));
      break;
    case ENEMYAIRCRAFT_RAYGUN:
      if ((tmpd = fabs (diff[0])) < 3.0)
        tmps = ((3.0F-tmpd)/3.0F)*(0.1F*sin (inherited::age_*0.25F));
      if (fabs (diff[1]) < 7.0)
        diff[1] *= 0.1F;
      lastMove_[0] = (0.975F*lastMove_[0])+(0.002F*diff[0]);
      lastMove_[1] = (0.9F*lastMove_[1])+(0.001F*diff[1]);
      inherited::position_[0] +=
        state.speed_adj*(randomMoveX_*lastMove_[0]*(state.game_skill+0.1F)+tmps);
      inherited::position_[1] +=
        state.speed_adj*(lastMove_[1]+inherited::velocity_[1]*(state.game_skill+0.1F));
      break;
    case ENEMYAIRCRAFT_TANK:
      if (fabs (diff[0]) > 8.0)
        v1 = 0.04F;
      else
        v1 = 0.04F*(fabs (diff[0])/8.0F); 
      inherited::velocity_[1] = (0.99F*inherited::velocity_[1]+0.01F*v1);

      if (inherited::position_[1] < -3.0)
        inherited::velocity_[1] = -0.1F;
      else if (inherited::position_[1] < 0.0)
        inherited::velocity_[1] *= 0.99F;

      if (inherited::position_[0] < 0.0)
        inherited::position_[0] =
          state.speed_adj*(0.998F*inherited::position_[0]+0.002F*(-configuration.screen_bound[0]+2.85F));
      else
        inherited::position_[0] =
          state.speed_adj*(0.998F*inherited::position_[0]+0.002F*(configuration.screen_bound[0]-2.85F));
      switch ((inherited::age_/50)%8)
      {
        case 2:
          inherited::position_[1] += state.speed_adj*0.05F;
          break;
        default:
          inherited::position_[1] -= state.speed_adj*inherited::velocity_[1];
          break;
      } // end SWITCH
      break;
    case ENEMYAIRCRAFT_GNAT:
      if (target_ == state.player)
        randX = randomMoveX_;
      else
        randX = (0.75F+FRAND)*0.15F;
      tmps = 3.8F;
      dist = sqrt (diff[0]*diff[0]+diff[1]*diff[1])*randX; 
    //			dist = fabs(diff[0])+fabs(diff[1])*randX;
      tmpd = 0.4F+(0.6F*((dist+(0.2F*sin (inherited::age_*0.001F)))/tmps));
      speed = tmpd*0.25F*randX;
      tmp = (diff[0]/dist);
      x = speed*tmp;//*(1.0+tmp);
      tmp = (diff[1]/dist);
      y = speed*tmp;
      if (dist < tmps)
      {
        tmpX = x;
        tmpY = y;
        x = (tmpd*tmpX)+(-(1.0F-tmpd)*diff[1]/tmpd);
        y = (tmpd*tmpY)+( (1.0F-tmpd)*diff[0]/tmpd);
        y += 0.01F*sin (state.game_frame*0.001F);
      } // end IF
      else
      {
        tmpd = 0.97F;
        tmpX = x;
        tmpY = y;
        if (randX < 0.65)
        {
          x = (tmpd*tmpX)+( (1.0F-tmpd)*diff[1]/tmpd);
          y = (tmpd*tmpY)+(-(1.0F-tmpd)*diff[0]/tmpd);
        } // end IF
        else
        {
          x = (tmpd*tmpX)+(-(1.0F-tmpd)*diff[1]/tmpd);
          y = (tmpd*tmpY)+( (1.0F-tmpd)*diff[0]/tmpd);
        } // end ELSE
      } // end ELSE

      tmp = randX*0.2F;
      if ((inherited::age_/8)%2)
        v0 = (inherited::velocity_[0]*(0.85F-tmp))+((0.2F+tmp)*(randX-0.2F)*x);
      else
        v0 = inherited::velocity_[0];
      v1 = (inherited::velocity_[1]*(0.85F-tmp))+((0.2F+tmp)*(randX-0.2F)*y);

      if (inherited::age_ < 50)
      {
        float amt;
        if (inherited::age_ > 20)
          amt = (inherited::age_-20)/30.0F;
        else
          amt = 0.0;
        inherited::velocity_[0] = (1.0F-amt)*inherited::velocity_[0]+(amt*v0);
        inherited::velocity_[1] = (1.0F-amt)*inherited::velocity_[1]+(amt*v1);
      } // end IF
      else
      {
        inherited::velocity_[0] = v0;
        inherited::velocity_[1] = v1;
      } // end ELSE

      inherited::position_[0] += state.speed_adj*inherited::velocity_[0];
      inherited::position_[1] += state.speed_adj*inherited::velocity_[1];

      if (inherited::position_[1] < -10.0)
        inherited::position_[1] = -10.0;
      break;
    case ENEMYAIRCRAFT_BOSS_0:
      approachDist = 7.0F*(2.0F-state.game_skill);
      if (fabs (diff[1]) < (approachDist+0.0*sin (state.frame*0.05)))
        diff[1] = diff[1]*(diff[1]/approachDist);
      diff[0] += 5.0F*sin (inherited::age_*0.1F);
      lastMove_[0] = (0.98F*lastMove_[0])+(0.0005F*state.game_skill*diff[0]);
      lastMove_[1] = (0.90F*lastMove_[1])+(0.001F *state.game_skill*diff[1]);
      inherited::position_[0] += state.speed_adj*lastMove_[0];
      inherited::position_[1] += state.speed_adj*(lastMove_[1] + inherited::velocity_[1]);
      break;
    case ENEMYAIRCRAFT_BOSS_1:
      if (((inherited::age_+25)/512)%2)
        approachDist = 9.0F *(2.0F-state.game_skill);
      else
        approachDist = 12.0F*(2.0F-state.game_skill);
      if (fabs (diff[1]) < (approachDist+2.0F*sin (state.frame*0.05F)))
        diff[1] = diff[1]*(diff[1]/approachDist);
      diff[0] += 5.0F*sin (inherited::age_*0.1F);

      if ((inherited::age_/512)%2)
      {
        lastMove_[0] = (0.98F*lastMove_[0])+(0.001F*state.game_skill*diff[0]);
        lastMove_[1] = (0.9F *lastMove_[1])+(0.002F*state.game_skill*diff[1]);
      } // end IF
      else //-- release gnats
      {
        lastMove_[0] = (0.9F*lastMove_[0]) + (0.0003F*state.game_skill*diff[0]);
        lastMove_[1] = (0.9F*lastMove_[1]) + (0.001F *state.game_skill*diff[1]);
      } // end ELSE
      inherited::position_[0] += state.speed_adj*lastMove_[0];
      inherited::position_[1] +=
        state.speed_adj*(lastMove_[1]+inherited::velocity_[1]);
      break;
    default:
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("invalid/unknown enemy aircraft type (was: %d), continuing\n"),
                  type_));
      //inherited::position_[1] -= state.speed_adj*0.02;
      break;
  } // end SWITCH

  // x-bounds clamp
  if (inherited::position_[0] < -configuration.screen_bound[0])
    inherited::position_[0] = -configuration.screen_bound[0];
  if (inherited::position_[0] >  configuration.screen_bound[0])
    inherited::position_[0] = configuration.screen_bound[0];
}
