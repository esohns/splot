#include "stdafx.h"

#include "status_display.h"

//#ifdef HAVE_CONFIG_H
//#include "splot-config.h"
//#endif
//
//#include "gettext.h"
//
//#include "Splot_StatusDisplay.h"
//
//#include <cstdio>
//#include <cstdlib>
//#include <cstring>
//#include <cmath>
//
#include <string>

#include "ace/OS.h"
#include "ace/Log_Msg.h"

//#include "compatibility.h"
//
//#if defined(HAVE_APPLE_OPENGL_FRAMEWORK) || defined(HAVE_OPENGL_GL_H)
//#include <OpenGL/gl.h>
//#else
//#include <GL/gl.h>
//#endif
//
#include "defines.h"
#include "common.h"
#include "configuration.h"
#include "state.h"
#include "explosion.h"
#include "image.h"
#include "text.h"
#include "player_aircraft.h"

float Splot_StatusDisplay::statPosAmmo[3] = {-10.5, 8.00, 25.0};
float Splot_StatusDisplay::statPosShld[3] = {-10.4F, -7.80F, 25.0F};
float Splot_StatusDisplay::statClrWarn[4] = {1.1F, 0.6F, 0.1F, 1.1F};
float Splot_StatusDisplay::statClrZero[4] = {0.0, 0.0, 0.0, 0.0};
float Splot_StatusDisplay::statClrAmmo[NUM_PLAYER_AMMO_TYPES][4] =
{
  {1.0F, 0.7F, 0.5F, 0.6F},
  {0.0, 1.0, 0.5F, 0.7F},
  {0.3F, 0.0, 1.0, 0.7F}
};

Splot_StatusDisplay::Splot_StatusDisplay ()
 : ammoAlpha_ (0.0)
 , damageAlpha_ (0.0)
 , shieldAlpha_ (0.0)
 , texStat_ (0)
 , texShld_ (0)
 , texTop_ (0)
 , texPlayerSuper_ (0)
 , texPlayerShield_ (0)
 //, texPlayerAmmoFlash_ ()
 , texUseFocus_ (0)
 //, texUseItem_ ()
 , blink_ (true)
 , tipShipShow_ (0)
 , tipSuperShow_ (0)
 , enemyWarn_ (0.0)
{
  for (int i = 0; i < NUM_PLAYER_AMMO_TYPES; i++)
    texPlayerAmmoFlash_[i] = 0;
  for (int i = 0; i < MAX_PLAYER_ITEMS; i++)
    texUseItem_[i] = 0;

  loadTextures ();
}

Splot_StatusDisplay::~Splot_StatusDisplay ()
{
  deleteTextures ();
}

void
Splot_StatusDisplay::loadTextures ()
{
#ifdef GL_CLAMP_TO_EDGE
  GLenum clamp = GL_CLAMP_TO_EDGE;
#else
  GLenum clamp = GL_CLAMP;
#endif

  std::string path_base = ACE_TEXT_ALWAYS_CHAR (SPLOT_IMAGE_DATA_DIR);
  path_base += ACE_DIRECTORY_SEPARATOR_STR;
  std::string filename = path_base + ACE_TEXT_ALWAYS_CHAR ("statBar.png");
  texStat_         = Splot_Image::load (dataLoc (filename), IMG_NOMIPMAPS, IMG_BLEND1, GL_REPEAT, GL_NEAREST, GL_NEAREST);
  filename = path_base + ACE_TEXT_ALWAYS_CHAR ("shields.png");
  texShld_         = Splot_Image::load (dataLoc (filename), IMG_SIMPLEMIPMAPS, IMG_BLEND1, clamp, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR);
  filename = path_base + ACE_TEXT_ALWAYS_CHAR ("stat-top.png");
  texTop_          = Splot_Image::load (dataLoc (filename), IMG_NOMIPMAPS, IMG_BLEND1, clamp, GL_LINEAR, GL_NEAREST);
  filename = path_base + ACE_TEXT_ALWAYS_CHAR ("heroSuper.png");
  texPlayerSuper_  = Splot_Image::load (dataLoc (filename), IMG_NOMIPMAPS, IMG_ALPHA, clamp, GL_LINEAR, GL_LINEAR);
  filename = path_base + ACE_TEXT_ALWAYS_CHAR ("heroShields.png");
  texPlayerShield_ = Splot_Image::load (dataLoc (filename), IMG_NOMIPMAPS, IMG_BLEND2, clamp, GL_LINEAR, GL_LINEAR);
  char buffer[PATH_MAX];
  for (int i = 0; i < NUM_PLAYER_AMMO_TYPES; i++)
  {
    ACE_OS::sprintf (buffer, ACE_TEXT_ALWAYS_CHAR ("heroAmmoFlash%02d.png"), i);
    filename = path_base + buffer;
    texPlayerAmmoFlash_[i] = Splot_Image::load (dataLoc (filename), IMG_NOMIPMAPS, IMG_ALPHA, clamp, GL_LINEAR, GL_LINEAR);
  } // end FOR
  filename = path_base + ACE_TEXT_ALWAYS_CHAR ("useFocus.png");
  texUseFocus_ = Splot_Image::load (dataLoc (filename), IMG_NOMIPMAPS, IMG_ALPHA, clamp, GL_LINEAR, GL_LINEAR);
  for (int i = 0; i < MAX_PLAYER_ITEMS; i++)
  {
    ACE_OS::sprintf (buffer, ACE_TEXT_ALWAYS_CHAR ("useItem%02d.png"), i);
    texUseItem_[i] = Splot_Image::load (dataLoc (filename), IMG_NOMIPMAPS, IMG_ALPHA, clamp, GL_LINEAR, GL_LINEAR);
  } // end FOR
}

void
Splot_StatusDisplay::deleteTextures ()
{
  glDeleteTextures (1, &texStat_);
  glDeleteTextures (1, &texShld_);
  glDeleteTextures (1, &texPlayerSuper_);
  glDeleteTextures (1, &texPlayerShield_);
  glDeleteTextures (NUM_PLAYER_AMMO_TYPES, &(texPlayerAmmoFlash_[0]));
  glDeleteTextures (MAX_PLAYER_ITEMS, &(texUseItem_[0]));
}

void
Splot_StatusDisplay::darkenGL ()
{
  //-- sidebars
  glBindTexture (GL_TEXTURE_2D, texShld_);
  glBegin (GL_QUADS);
  glColor4f (0.25F, 0.2F, 0.2F, 0.6F);
  glTexCoord2f (0.0,  0.0); glVertex3f ( -9.2F,  8.5F, 25.0F);
  glTexCoord2f (1.0,  0.0); glVertex3f (-11.5F,  8.5F, 25.0F);
  glColor4f (0.25F, 0.25F, 0.35F, 0.6F);
  glTexCoord2f (1.0,  1.7F); glVertex3f (-11.5F, -8.5F, 25.0F);
  glTexCoord2f (0.0,  1.7F); glVertex3f ( -9.2F, -8.5F, 25.0F);

  glColor4f (0.25F, 0.2F, 0.2F, 0.6F);
  glTexCoord2f (1.0, 0.0); glVertex3f ( 11.5F,  8.5F, 25.0F);
  glTexCoord2f (0.0, 0.0); glVertex3f (  9.2F,  8.5F, 25.0F);
  glColor4f (0.25F, 0.25F, 0.35F, 0.6F);
  glTexCoord2f (0.0, 1.7F); glVertex3f (  9.2F, -8.5F, 25.0F);
  glTexCoord2f (1.0, 1.7F); glVertex3f ( 11.5F, -8.5F, 25.0F);
  glEnd ();
}

void
Splot_StatusDisplay::drawGL (Splot_PlayerAircraft* player_in)
{
  if (!player_in)
    return;

  State_t& state = SPLOT_STATE_SINGLETON::instance ()->get ();
  if (!(state.frame%15))
    blink_ = !blink_;

  ammoAlpha_ *= 0.96F;

  GameState_t& game_state = SPLOT_STATE_SINGLETON::instance ()->gameState ();
  float shields = game_state.shields;
  float superShields = 0.0;
  if (shields > PLAYER_DEFAULT_SHIELDS)
  {
    superShields = PLAYER_DEFAULT_SHIELDS-(shields-PLAYER_DEFAULT_SHIELDS);
  //		superShields = (shields-PLAYER_DEFAULT_SHIELDS);
    shields = PLAYER_DEFAULT_SHIELDS;
  } // end IF

  //-- draw score
  static char buffer[BUFSIZ];
  glColor4f (1.0, 1.0, 1.0, 0.4F);
  glPushMatrix ();
  ACE_OS::sprintf (buffer, ACE_TEXT_ALWAYS_CHAR ("%07d"),
                   (int)game_state.score);
  glTranslatef (-9.0F, -8.2F, 25.0F);
  glScalef (0.025F, 0.02F, 1.0);
  state.text->Render (buffer);
  glPopMatrix ();

  //-- draw fps
  const Configuration_t configuration =
    SPLOT_CONFIGURATION_SINGLETON::instance ()->get ();
  if (configuration.show_fps)
  {
    glPushMatrix ();
    glTranslatef (7.75, 8.0, 25.0);
    glScalef (0.018F, 0.015F, 1.0);
    ACE_OS::sprintf (buffer, ACE_TEXT_ALWAYS_CHAR ("%3.1f"),
                     state.fps);
    state.text->Render (buffer);
    glPopMatrix ();
  } // end IF

  //-- draw remaining player ships
  glPushMatrix ();
  glColor4f (0.6F, 0.6F, 0.7F, 1.0);
  glBindTexture (GL_TEXTURE_2D, state.player->texAircraft_);
  glTranslatef (10.2F, 7.4F, 25.0F);
  float size[2] = {
    state.player->getSize (0)*0.5F,
    state.player->getSize (1)*0.5F
  };
  for (int i = 0; i < game_state.ships; i++)
  {
    drawQuad (size[0], size[1]);
    glTranslatef (0.0, -size[1]*2.0F, 0.0);
  } // end FOR
  glPopMatrix ();

  //-- draw usable items
  float a;
  if (configuration.gfx_level > 1)
  {
    glPushMatrix ();
    glColor4f (1.0, 1.0, 1.0, 1.0);
    glTranslatef (8.5F, -7.7F, 25.0F);
    size[0] = 0.4F;
    size[1] = 0.5F;
    for (int i = 0; i < MAX_PLAYER_ITEMS; i++)
    {
      if (i == game_state.current_item_index)
      {
        a = game_state.use_item_armed*0.8F;
        glColor4f (0.4F+a, 0.4F, 0.4F, 0.4F+a);
        glBindTexture (GL_TEXTURE_2D, texUseFocus_);
        drawQuad (size[1], size[1]);
        glColor4f (1.0, 1.0, 1.0, 1.0);
      } // end IF
      glBindTexture (GL_TEXTURE_2D, texUseItem_[i]);
      drawQuad (size[0], size[0]);
      glTranslatef (-size[1]*2.0F, 0.0, 0.0);
    } // end FOR
    glPopMatrix ();
  } // end IF

  //-- draw 'enemy-got-past' Warning
  if (enemyWarn_ &&
      game_state.ships >= 0)
  {
    glPushMatrix ();
    glColor4f (1.0, 0.0, 0.0, enemyWarn_+0.15F*sin (state.game_frame*0.7F));
    glTranslatef (0.0, -8.75, 25.0);
    glBindTexture (GL_TEXTURE_2D, texPlayerAmmoFlash_[0]);
    drawQuad (12.0, 3.0);
    glPopMatrix ();
    enemyWarn_ = 0.0;
  } // end IF

  //-- draw AMMO
  glPushMatrix ();
  glTranslatef (Splot_StatusDisplay::statPosAmmo[0],
                Splot_StatusDisplay::statPosAmmo[1],
                Splot_StatusDisplay::statPosAmmo[2]);

  //--draw ammo reserves
  glBindTexture (GL_TEXTURE_2D, texStat_);
  glBegin (GL_QUADS);
  float x = 0.0, y, y3;
  bool  statClrWarnAmmo = false;
  float w = 0.1F;
  for (int i = 0; i < NUM_PLAYER_AMMO_TYPES; i++)
  {
    glColor4fv (Splot_StatusDisplay::statClrAmmo[i]);
    if (game_state.ammo_stock[i] > 0.0)
    {
      x = i*0.3F;
      y = game_state.ammo_stock[i]*0.02F;
      y3= y*2.65F;
      if (blink_ || game_state.ammo_stock[i] > 50.0)
        glColor4fv (Splot_StatusDisplay::statClrAmmo[i]);
      else
      {
        statClrWarnAmmo = true;
        glColor4fv (Splot_StatusDisplay::statClrWarn);
      } // end ELSE

      glTexCoord2f (1.0, 0.0); glVertex3f (x+w, -y3, 0.0);
      glTexCoord2f (1.0,   y); glVertex3f (x+w, 0.0, 0.0);
      glTexCoord2f (0.0,   y); glVertex3f (x-w, 0.0, 0.0);
      glTexCoord2f (0.0, 0.0); glVertex3f (x-w, -y3, 0.0);
    } // end IF
  } // end FOR
  glEnd ();

  glBindTexture (GL_TEXTURE_2D, texTop_);
  if (statClrWarnAmmo)
    glColor4f (Splot_StatusDisplay::statClrWarn[0],
               Splot_StatusDisplay::statClrWarn[1],
               Splot_StatusDisplay::statClrWarn[2],
               0.5F+ammoAlpha_);
  else
    glColor4f (0.5F, 0.5F, 0.5F+ammoAlpha_, 0.2F+ammoAlpha_);
  glBegin (GL_QUADS);
  glTexCoord2f (1.0, 1.0); glVertex3f ( 1.25F, -1.85F, 0.0);
  glTexCoord2f (1.0, 0.0); glVertex3f ( 1.25F,  0.47F, 0.0);
  glTexCoord2f (0.0, 0.0); glVertex3f (-0.75F,  0.47F, 0.0);
  glTexCoord2f (0.0, 1.0); glVertex3f (-0.75F, -2.85F, 0.0);
  glEnd ();
  x += w*1.5F;

  glPopMatrix ();

  //--draw Shields
  damageAlpha_ *= 0.94F;
  shieldAlpha_ *= 0.94F;
  float dc = damageAlpha_*0.5F;
  float sc = shieldAlpha_*0.5F;
  float sl, sls, dl, dls;
  float szx = 0.5;
  float szy = 6.0;
  static float rot = 0;
  rot += 2.0F*state.speed_adj;
  float rot2 = 2.0F*((int)rot%180);

  sl = sls = (shields          /PLAYER_DEFAULT_SHIELDS)-1.0F;
  dl = dls = (game_state.damage/PLAYER_DEFAULT_DAMAGE) -1.0F;
  if (superShields)
    sls = dls = ((shields+superShields)/PLAYER_DEFAULT_SHIELDS)-1.0F;

  //------ draw Engine
  float c1f, c2f;
  float c1[4] = {0.85F, 0.65F, 1.0  , 0.7F};
  float c2[4] = {1.0F , 0.2F , 0.25F, 0.7F};
  float esz;
  if (player_in->isVisible () &&
      configuration.gfx_level >= 1)
  {
    c1f = 1.0F+dl;
    c2f = -dl;
    //		glColor4f(0.9, 0.7, 1.0, 0.7);
    glColor4f (c1[0]*c1f+c2[0]*c2f,
               c1[1]*c1f+c2[1]*c2f,
               c1[2]*c1f+c2[2]*c2f,
               c1[3]*c1f+c2[3]*c2f);
    glBindTexture (GL_TEXTURE_2D, texPlayerAmmoFlash_[0]);
    glPushMatrix ();
    glTranslatef (player_in->position_[0],
                  player_in->position_[1]-0.625F,
                  player_in->position_[2]);
    esz = 1.0F+c2f;
    drawQuad (1.3F, 0.5F*esz);
    glTranslatef (0.0, -0.18F, 0.0);
    glRotatef ((float)IRAND, 0.0, 0.0, 1.0);
    drawQuad (0.85F*esz, 0.6F*esz);
    glPopMatrix ();
  } // end IF

  //	if(shields > 0)
  //	{
  //		glPushMatrix();
  //		float sz = hero->getSize(1)*1.5;
  //		glColor4f(0.5, 0.5, 1.0, 0.2);
  //		glBindTexture(GL_TEXTURE_2D, heroShieldTex);
  //		glTranslatef(hero->pos[0], hero->pos[1]-0.05, hero->pos[2]);
  //		glRotatef(IRAND, 0.0, 0.0, 1.0);
  //		drawQuad(sz, sz);
  //		glPopMatrix();
  //	}

  //------ draw Super Shields
  float sz;
  float p[3] = {0.0, 0.0, player_in->position_[2]};
  //float v0;
  float v[3] = {0.0, 0.0, 0.0};
  float c3 = 1.0F-sls*sls;
  float c[4] = {1.0, 1.0, 0.7F, c3};
  if (superShields)
  {
    glPushMatrix ();
    sz = player_in->getSize (1)*1.3F;
    glColor4f (1.0, 1.0, 1.0, 1.0F-sls*sls);
    glBindTexture (GL_TEXTURE_2D, texPlayerSuper_);
    glTranslatef (player_in->position_[0],
                  player_in->position_[1],
                  player_in->position_[2]);
    glRotatef ((float)IRAND, 0.0, 0.0, 1.0);
    drawQuad (sz, sz);
    glPopMatrix ();

    //------ add a bit of Glitter
    if (configuration.gfx_level > 1 &&
        !state.game_pause)
    {
      v[0] = 0.01F*SRAND;
      switch (state.game_frame%2)
      {
        case 0:
          v[1] = -0.3F+FRAND*0.05F;
          p[0] = player_in->position_[0];
          p[1] = player_in->position_[1]-0.8F;
          state.explosions->addEffect (EXPLOSION_EFFECT_GLITTER, p, v, c, 0, 0.4F+0.4F*FRAND);
          v[1] = -0.25F+FRAND*0.05F;
          p[0] = player_in->position_[0]+0.95F;
          p[1] = player_in->position_[1]+0.1F;
          state.explosions->addEffect (EXPLOSION_EFFECT_GLITTER, p, v, c, 0, 0.4F+0.4F*FRAND);
          p[0] = player_in->position_[0]-0.95F;
          p[1] = player_in->position_[1]+0.1F;
          state.explosions->addEffect (EXPLOSION_EFFECT_GLITTER, p, v, c, 0, 0.4F+0.4F*FRAND);
          break;
        case 1:
          v[1] = -0.25F+FRAND*0.05F;
          p[0] = player_in->position_[0]+0.95F;
          p[1] = player_in->position_[1]+0.1F;
          state.explosions->addEffect (EXPLOSION_EFFECT_GLITTER, p, v, c, 0, 0.4F+0.4F*FRAND);
          p[0] = player_in->position_[0]-0.95F;
          p[1] = player_in->position_[1]+0.1F;
          state.explosions->addEffect (EXPLOSION_EFFECT_GLITTER, p, v, c, 0, 0.4F+0.4F*FRAND);
          break;
      } // end SWITCH
    } // end IF
  } // end IF

  //---------- Draw ammo flash
  if (configuration.gfx_level > 1)
  {
    glPushMatrix ();
    glTranslatef (player_in->position_[0],
                  player_in->position_[1],
                  player_in->position_[2]);
    if (player_in->gunFlash0_[0])
    {
      glBindTexture (GL_TEXTURE_2D, texPlayerAmmoFlash_[0]);
      szx = player_in->gunFlash0_[0];
      szy = 0.46f*szx;
      glColor4f (0.75f, 0.75f, 0.75f, szx);
      glTranslatef ( 0.275F,  0.25F, 0.0);
      drawQuad (szy, szy);
      glTranslatef (-0.55F,   0.0, 0.0);
      drawQuad (szy, szy);
      glTranslatef ( 0.275F, -0.25F, 0.0);

      if (player_in->gunFlash1_[0])
      {
        glTranslatef ( 0.45F, -0.1F, 0.0);
        drawQuad (szy, szy);
        glTranslatef (-0.9F,   0.0 , 0.0);
        drawQuad (szy, szy);
        glTranslatef ( 0.45F,  0.1F, 0.0);
      } // end IF
    } // end IF
    if (player_in->gunFlash0_[1])
    {
      glBindTexture (GL_TEXTURE_2D, texPlayerAmmoFlash_[1]);
      szx = player_in->gunFlash0_[1];
      szy = 0.8f*szx;
      glColor4f (1.0f, 1.0f, 1.0f, szx);
      glTranslatef (0.0,  0.7F, 0.0);
      drawQuad (szy, szy);
      glTranslatef (0.0, -0.7F, 0.0);
    } // end IF
    glBindTexture (GL_TEXTURE_2D, texPlayerAmmoFlash_[2]);
    if (player_in->gunFlash0_[2])
    {
      szx = player_in->gunFlash0_[2];
      szy = 0.65F*szx;
      glColor4f (1.0f, 1.0f, 1.0f, szx);
      glTranslatef (-0.65F, -0.375F, 0.0);
      drawQuad (szy, szy);
      glTranslatef ( 0.65F,  0.375F, 0.0);
    } // end IF
    if (player_in->gunFlash1_[2])
    {
      szx = player_in->gunFlash1_[2];
      szy = 0.65f*szx;
      glColor4f (1.0f, 1.0f, 1.0f, szx);
      glTranslatef ( 0.65F, -0.375F, 0.0);
      drawQuad (szy, szy);
      glTranslatef (-0.65F,  0.375F, 0.0);
    } // end IF
    glPopMatrix ();
  } // end IF

  //-- shield indicator
  glBindTexture (GL_TEXTURE_2D, texShld_);
  glColor4f (0.2F, 0.2F, 0.2F, 0.5F);
  glBegin (GL_QUADS);
  szx = 0.6F;
  szy = 6.0F;
  glTexCoord2f ( 1.0, 1.0);
  glVertex3f (Splot_StatusDisplay::statPosShld[0]+szx,
              Splot_StatusDisplay::statPosShld[1]+szy,
              Splot_StatusDisplay::statPosShld[2]);
  glTexCoord2f (-2.5F, 1.0);
  glVertex3f (Splot_StatusDisplay::statPosShld[0]-2.0F,
              Splot_StatusDisplay::statPosShld[1]+szy,
              Splot_StatusDisplay::statPosShld[2]);
  glTexCoord2f (-2.5F, 0.0);
  glVertex3f (Splot_StatusDisplay::statPosShld[0]-2.0F,
              Splot_StatusDisplay::statPosShld[1],
              Splot_StatusDisplay::statPosShld[2]);
  glTexCoord2f ( 1.0, 0.0);
  glVertex3f (Splot_StatusDisplay::statPosShld[0]+szx,
              Splot_StatusDisplay::statPosShld[1],
              Splot_StatusDisplay::statPosShld[2]);

  glTexCoord2f ( 3.5F, 1.0);
  glVertex3f (-Splot_StatusDisplay::statPosShld[0]+2.0F,
               Splot_StatusDisplay::statPosShld[1]+szy,
               Splot_StatusDisplay::statPosShld[2]);
  glTexCoord2f ( 0.0, 1.0);
  glVertex3f (-Splot_StatusDisplay::statPosShld[0]-szx,
               Splot_StatusDisplay::statPosShld[1]+szy,
               Splot_StatusDisplay::statPosShld[2]);
  glTexCoord2f ( 0.0, 0.0);
  glVertex3f (-Splot_StatusDisplay::statPosShld[0]-szx,
               Splot_StatusDisplay::statPosShld[1],
               Splot_StatusDisplay::statPosShld[2]);
  glTexCoord2f ( 3.5F, 0.0);
  glVertex3f (-Splot_StatusDisplay::statPosShld[0]+2.0F,
               Splot_StatusDisplay::statPosShld[1],
               Splot_StatusDisplay::statPosShld[2]);
  glEnd ();

  if (configuration.gfx_level > 0)
  {
    //-- Shields 
    if ((sl < -0.7 &&
         blink_    &&
         shields > 0.0) ||
        superShields)
      glColor4fv (Splot_StatusDisplay::statClrWarn);
    else
      glColor4f (0.7F+dc, 0.6F+dc, 0.8F+dc, 0.5F+damageAlpha_);
    //			glColor4f(0.0+sc, 0.35+sc, 1.0+sc, 0.5+shieldAlpha_);
    glPushMatrix ();
    glTranslatef (Splot_StatusDisplay::statPosShld[0],
                  Splot_StatusDisplay::statPosShld[1],
                  Splot_StatusDisplay::statPosShld[2]);
    //		glScalef(1.0, 1.0, 1.5);
    glRotatef (-rot, 0.0, 1.0, 0.0);
    glBegin (GL_QUADS);
    szx = 0.5;
    glTexCoord2f (1.0,     sls);  glVertex3f ( szx, szy, szx);
    glTexCoord2f (0.0,     sls);  glVertex3f (-szx, szy, szx);
    glTexCoord2f (0.0, 1.0F+sls); glVertex3f (-szx, 0.0, szx);
    glTexCoord2f (1.0, 1.0F+sls); glVertex3f ( szx, 0.0, szx);

    glTexCoord2f (0.0,     sls);  glVertex3f ( szx, szy, -szx);
    glTexCoord2f (0.0, 1.0F+sls); glVertex3f ( szx, 0.0, -szx);
    glTexCoord2f (1.0, 1.0F+sls); glVertex3f (-szx, 0.0, -szx);
    glTexCoord2f (1.0,     sls);  glVertex3f (-szx, szy, -szx);

    glTexCoord2f (1.0,     sls);  glVertex3f ( szx, szy,  szx);
    glTexCoord2f (1.0, 1.0F+sls); glVertex3f ( szx, 0.0,  szx);
    glTexCoord2f (0.0, 1.0F+sls); glVertex3f ( szx, 0.0, -szx);
    glTexCoord2f (0.0,     sls);  glVertex3f ( szx, szy, -szx);

    glTexCoord2f (1.0,     sls);  glVertex3f (-szx, szy, -szx);
    glTexCoord2f (1.0, 1.0F+sls); glVertex3f (-szx, 0.0, -szx);
    glTexCoord2f (0.0, 1.0F+sls); glVertex3f (-szx, 0.0,  szx);
    glTexCoord2f (0.0,     sls);  glVertex3f (-szx, szy,  szx);

    if (shields)
    {
      glTexCoord2f (1.0, 1.0);
      glColor4f (0.3F+sc, 0.4F+sc, 1.0F+sc, 0.5F);
      glVertex3f ( szx, 0.0,  szx);
      glVertex3f ( szx, 0.0, -szx);
      glVertex3f (-szx, 0.0, -szx);
      glVertex3f (-szx, 0.0,  szx);
    } // end IF
    glEnd ();

    glRotatef (rot2, 0.0, 1.0, 0.0);
    //		glColor4f(0.4+sc, 0.5+sc, 1.0+sc, 0.6+shieldAlpha_);
    glColor4f (0.1F+sc, 0.15F+sc, 0.9F+sc, 0.6F+shieldAlpha_);
    glBegin (GL_QUADS);
    szx = 0.4F;
    glTexCoord2f (1.0,     sl);  glVertex3f ( szx, szy,  szx);
    glTexCoord2f (0.0,     sl);  glVertex3f (-szx, szy,  szx);
    glTexCoord2f (0.0, 1.0F+sl); glVertex3f (-szx, 0.0,  szx);
    glTexCoord2f (1.0, 1.0F+sl); glVertex3f ( szx, 0.0,  szx);

    glTexCoord2f (0.0,     sl);  glVertex3f ( szx, szy, -szx);
    glTexCoord2f (0.0, 1.0F+sl); glVertex3f ( szx, 0.0, -szx);
    glTexCoord2f (1.0, 1.0F+sl); glVertex3f (-szx, 0.0, -szx);
    glTexCoord2f (1.0,     sl);  glVertex3f (-szx, szy, -szx);

    glTexCoord2f (1.0,     sl);  glVertex3f ( szx, szy,  szx);
    glTexCoord2f (1.0, 1.0F+sl); glVertex3f ( szx, 0.0,  szx);
    glTexCoord2f (0.0, 1.0F+sl); glVertex3f ( szx, 0.0, -szx);
    glTexCoord2f (0.0,     sl);  glVertex3f ( szx, szy, -szx);

    glTexCoord2f (0.0,     sl);  glVertex3f (-szx, szy,  szx);
    glTexCoord2f (1.0,     sl);  glVertex3f (-szx, szy, -szx);
    glTexCoord2f (1.0, 1.0F+sl); glVertex3f (-szx, 0.0, -szx);
    glTexCoord2f (0.0, 1.0F+sl); glVertex3f (-szx, 0.0,  szx);
    glEnd ();
    glPopMatrix ();

    //-- Life
    if ((dl < -0.7 &&
         blink_) ||
        superShields)
    {
      glColor4fv (Splot_StatusDisplay::statClrWarn);
      if (configuration.tex_border)
        glTexParameterfv (GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, Splot_StatusDisplay::statClrWarn);
    } // end IF
    else
    {
      glColor4f (0.9F+dc, 0.6F+dc, 0.7F+dc, 0.5F+damageAlpha_);
      if (configuration.tex_border)
        glTexParameterfv (GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, Splot_StatusDisplay::statClrZero);
    } // end ELSE
    glPushMatrix ();
    glTranslatef (-Splot_StatusDisplay::statPosShld[0],
                   Splot_StatusDisplay::statPosShld[1],
                   Splot_StatusDisplay::statPosShld[2]);
    glRotatef (rot, 0.0, 1.0, 0.0);

    glBegin (GL_QUADS);
    szx = 0.5;
    glTexCoord2f (1.0,     dls);  glVertex3f ( szx, szy, szx);
    glTexCoord2f (0.0,     dls);  glVertex3f (-szx, szy, szx);
    glTexCoord2f (0.0, 1.0F+dls); glVertex3f (-szx, 0.0, szx);
    glTexCoord2f (1.0, 1.0F+dls); glVertex3f ( szx, 0.0, szx);

    glTexCoord2f (0.0,     dls);  glVertex3f ( szx, szy, -szx);
    glTexCoord2f (0.0, 1.0F+dls); glVertex3f ( szx, 0.0, -szx);
    glTexCoord2f (1.0, 1.0F+dls); glVertex3f (-szx, 0.0, -szx);
    glTexCoord2f (1.0,     dls);  glVertex3f (-szx, szy, -szx);

    glTexCoord2f (1.0,     dls);  glVertex3f ( szx, szy,  szx);
    glTexCoord2f (1.0, 1.0F+dls); glVertex3f ( szx, 0.0,  szx);
    glTexCoord2f (0.0, 1.0F+dls); glVertex3f ( szx, 0.0, -szx);
    glTexCoord2f (0.0,     dls);  glVertex3f ( szx, szy, -szx);

    glTexCoord2f (0.0,     dls);  glVertex3f (-szx, szy,  szx);
    glTexCoord2f (1.0,     dls);  glVertex3f (-szx, szy, -szx);
    glTexCoord2f (1.0, 1.0F+dls); glVertex3f (-szx, 0.0, -szx);
    glTexCoord2f (0.0, 1.0F+dls); glVertex3f (-szx, 0.0,  szx);

    if (game_state.damage)
    {
      glTexCoord2f (1.0, 1.0); 
      glColor4f (1.0F+dc, dc, dc, 0.5F);
      glVertex3f ( szx, 0.0,  szx);
      glVertex3f ( szx, 0.0, -szx);
      glVertex3f (-szx, 0.0, -szx);
      glVertex3f (-szx, 0.0,  szx);
    } // end IF
    glEnd ();

    glRotatef (-rot2, 0.0, 1.0, 0.0);
    glColor4f (1.0F+dc, dc, dc, 0.6F+damageAlpha_);
    glBegin (GL_QUADS);
    szx = 0.4F;
    glTexCoord2f (1.0,     dl);  glVertex3f ( szx, szy, szx);
    glTexCoord2f (0.0,     dl);  glVertex3f (-szx, szy, szx);
    glTexCoord2f (0.0, 1.0F+dl); glVertex3f (-szx, 0.0, szx);
    glTexCoord2f (1.0, 1.0F+dl); glVertex3f ( szx, 0.0, szx);

    glTexCoord2f (0.0,     dl);  glVertex3f ( szx, szy, -szx);
    glTexCoord2f (0.0, 1.0F+dl); glVertex3f ( szx, 0.0, -szx);
    glTexCoord2f (1.0, 1.0F+dl); glVertex3f (-szx, 0.0, -szx);
    glTexCoord2f (1.0,     dl);  glVertex3f (-szx, szy, -szx);

    glTexCoord2f (1.0,     dl);  glVertex3f ( szx, szy,  szx);
    glTexCoord2f (1.0, 1.0F+dl); glVertex3f ( szx, 0.0,  szx);
    glTexCoord2f (0.0, 1.0F+dl); glVertex3f ( szx, 0.0, -szx);
    glTexCoord2f (0.0,     dl);  glVertex3f ( szx, szy, -szx);

    glTexCoord2f (0.0,     dl);  glVertex3f (-szx, szy,  szx);
    glTexCoord2f (1.0,     dl);  glVertex3f (-szx, szy, -szx);
    glTexCoord2f (1.0, 1.0F+dl); glVertex3f (-szx, 0.0, -szx);
    glTexCoord2f (0.0, 1.0F+dl); glVertex3f (-szx, 0.0,  szx);
    glEnd ();
    glPopMatrix ();
  } // end IF
  else
  {
    szx = 0.8F;
    if ((sl < -0.7 &&
         blink_    &&
         shields > 0.0) ||
        superShields)
      glColor4fv (Splot_StatusDisplay::statClrWarn);
    else
      glColor4f (sc, 0.35F+sc, 1.0F+sc, 0.7F+shieldAlpha_);
    //-- Shields
    glBegin (GL_QUADS);
    glTexCoord2f (1.0, sl);
    glVertex3f (Splot_StatusDisplay::statPosShld[0],
                Splot_StatusDisplay::statPosShld[1]+szy,
                Splot_StatusDisplay::statPosShld[2]);
    glTexCoord2f (0.0,     sl);
    glVertex3f (Splot_StatusDisplay::statPosShld[0]-szx,
                Splot_StatusDisplay::statPosShld[1]+szy,
                Splot_StatusDisplay::statPosShld[2]);
    glTexCoord2f (0.0, 1.0F+sl);
    glVertex3f (Splot_StatusDisplay::statPosShld[0]-szx,
                Splot_StatusDisplay::statPosShld[1],
                Splot_StatusDisplay::statPosShld[2]);
    glTexCoord2f (1.0, 1.0F+sl);
    glVertex3f (Splot_StatusDisplay::statPosShld[0],
                Splot_StatusDisplay::statPosShld[1],
                Splot_StatusDisplay::statPosShld[2]);

    //-- Life
    if (dl < -0.7 && blink_)
      glColor4fv (Splot_StatusDisplay::statClrWarn);
    else
      glColor4f (1.0F+dc, dc, dc, 0.7F+damageAlpha_);
    glTexCoord2f (1.0,     dl);
    glVertex3f (-Splot_StatusDisplay::statPosShld[0],
                 Splot_StatusDisplay::statPosShld[1]+szy,
                 Splot_StatusDisplay::statPosShld[2]);
    glTexCoord2f (1.0, 1.0F+dl);
    glVertex3f (-Splot_StatusDisplay::statPosShld[0],
                 Splot_StatusDisplay::statPosShld[1],
                 Splot_StatusDisplay::statPosShld[2]);
    glTexCoord2f (0.0, 1.0F+dl);
    glVertex3f (-Splot_StatusDisplay::statPosShld[0]+szx,
                 Splot_StatusDisplay::statPosShld[1],
                 Splot_StatusDisplay::statPosShld[2]);
    glTexCoord2f (0.0,     dl);
    glVertex3f (-Splot_StatusDisplay::statPosShld[0]+szx,
                 Splot_StatusDisplay::statPosShld[1]+szy,
                 Splot_StatusDisplay::statPosShld[2]);
    glEnd ();
  } // end ELSE

  //-- print message when paused...
  float off[2];
  if (state.game_pause)
  {
    off[0] = 2.0F*sin (state.frame*0.01F);
    off[1] = 1.0F*cos (state.frame*0.011F);
    glPushMatrix ();
    glTranslatef (-14.5F, -3.0F, 0.0);
    glScalef (0.21F, 0.21F, 1.0);
    glPushMatrix ();
    glColor4f (1.0, 1.0, 1.0, 0.1F*fabs (sin (state.frame*0.05F)));
    state.text->Render (ACE_TEXT_ALWAYS_CHAR ("p a u s e d"));
    glPopMatrix ();
    glColor4f (1.0, 1.0, 1.0, 0.1F*fabs (sin (state.frame*0.03F)) );
    glTranslatef (off[0], off[1], 0.0);
    state.text->Render (ACE_TEXT_ALWAYS_CHAR ("p a u s e d"));
    glPopMatrix ();
  } // end IF
  if (state.tip_ship_past == 1 &&
      state.game_level == 1)
  {
    state.tip_ship_past++;
    tipShipShow_ = 200;
  } // end IF
  if (state.tip_super_shield == 1 &&
      state.game_level == 1)
  {
    state.tip_super_shield++;
    tipSuperShow_ = 200;
  } // end IF
  if (tipShipShow_ > 0)
  {
    tipShipShow_--;
    glPushMatrix ();
    glTranslatef (-16.0F, 13.0F, 0.0);
    glScalef (0.035F, 0.035F, 1.0);
    glColor4f (1.0, 1.0, 1.0, tipShipShow_/300.0F);
    state.text->Render (ACE_TEXT_ALWAYS_CHAR ("do not let -any- ships past you! each one costs you a life!"));
    glPopMatrix ();
  } // end IF
  if (tipSuperShow_ > 0)
  {
    tipSuperShow_--;
    glPushMatrix ();
    glTranslatef (-16.0F, 13.0F, 0.0);
    glScalef (0.035F, 0.035F, 1.0);
    glColor4f (1.0, 1.0, 1.0, tipSuperShow_/300.0F);
    state.text->Render (ACE_TEXT_ALWAYS_CHAR ("let super shields pass by for an extra life!"));
    glPopMatrix ();
  } // end IF
}
