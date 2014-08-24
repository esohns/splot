#include "stdafx.h"

#include "menu.h"

#include <cstdlib>
//#include <cstring>
#include <cmath>
//
//#include "compatibility.h"
//
//#if defined(HAVE_APPLE_OPENGL_FRAMEWORK) || defined(HAVE_OPENGL_GL_H)
//#include <OpenGL/gl.h>
//#else
//#include <GL/gl.h>
//#endif
//

#include "ace/OS.h"
#include "ace/Log_Msg.h"

#include "gettext.h"

//#ifdef HAVE_CONFIG_H
//#include "splot-config.h"
//#endif

#include "common.h"
#include "defines.h"
#include "state.h"
#include "configuration.h"
#include "highscore.h"
#include "background.h"
#include "audio.h"
#include "configuration.h"
#include "image.h"
#include "textGeometry.h"

// init statics
char Splot_Menu::msgHelpText_[NUM_HELP_MESSAGE_LINES][BUFSIZ] = {
ACE_TEXT_ALWAYS_CHAR ("  d o   n o t   a l l o w  -a n y-   e n e m i e s   g e t   p a s t   y o u !"),
ACE_TEXT_ALWAYS_CHAR ("  e v e r y   e n e m y   t h a t   g e t s   b y   c o s t s   y o u   a   l i f e !"),
ACE_TEXT_ALWAYS_CHAR ("  a l l o w   p o w e r - u p s   t o   p a s s   b y   f o r   b i g   p o i n t s !"),
ACE_TEXT_ALWAYS_CHAR ("  c r a s h   i n t o   e n e m i e s   t o   d e s t r o y   t h e m !"),
ACE_TEXT_ALWAYS_CHAR ("  r i g h t   c l i c k   t w i c e   t o   s e l f - d e s t r u c t !"),
ACE_TEXT_ALWAYS_CHAR ("  s e l f - d e s t r u c t   t o   p r e s e r v e   y o u r   a m m u n i t i o n !"),
ACE_TEXT_ALWAYS_CHAR ("  d o w n l o a d   S P L O T   a t   http://github.com/esohns/splot/"),
};

Splot_Menu::Splot_Menu ()
 : currentSelection_ (MENU_NEWGAME)
 , elecStretch_ (10.0F)
 //, elecOff_ ({0.0, 0.0})
 , textAngle_ (0.0)
 , txtHeight_ (0.5F)
 , titleTilt_ (-10.0F)
 //, butSize_ ({0.5*4.0, 0.5}) // butSize_[1]*4.0
 , butOffset_ (3.05F)
 , thickText_ (true)
 , msgAlpha_ (0.0)
 , msgIndex_ (0)
 , msgCount_ (0)
 , msgHelpOverride_ (false)
{
  elecOff_[0] = 0.0;
  elecOff_[1] = 0.0;

  butSize_[0] = 0.5F*4.0F; // butSize_[1]*4.0
  butSize_[1] = 0.5F;

  ACE_OS::strcpy (menuText_[MENU_NEWGAME], ACE_TEXT_ALWAYS_CHAR ("n e w    g a m e"));
  ACE_OS::strcpy (menuText_[MENU_GAME_LEVEL], ACE_TEXT_ALWAYS_CHAR ("l e v e l"));
  ACE_OS::strcpy (menuText_[MENU_SKILL_LEVEL], ACE_TEXT_ALWAYS_CHAR ("s k i l l"));
  ACE_OS::strcpy (menuText_[MENU_GRAPHICS], ACE_TEXT_ALWAYS_CHAR ("g f x    d e t a i l"));
  ACE_OS::strcpy (menuText_[MENU_FULLSCREEN], ACE_TEXT_ALWAYS_CHAR ("f u l l s c r e e n"));
  ACE_OS::strcpy (menuText_[MENU_SCREENSIZE], ACE_TEXT_ALWAYS_CHAR ("s c r e e n    s i z e"));
  ACE_OS::strcpy (menuText_[MENU_SOUND], ACE_TEXT_ALWAYS_CHAR ("s o u n d    f x    v o l u m e"));
  ACE_OS::strcpy (menuText_[MENU_MUSIC], ACE_TEXT_ALWAYS_CHAR ("m u s i c    v o l u m e"));
  ACE_OS::strcpy (menuText_[MENU_SPEED], ACE_TEXT_ALWAYS_CHAR ("m o v e m e n t   s p e e d"));
  ACE_OS::strcpy (menuText_[MENU_QUIT], ACE_TEXT_ALWAYS_CHAR ("q u i t"));

  //ACE_OS::strcpy (skillText_[0], ACE_TEXT_ALWAYS_CHAR ("-"));
  //ACE_OS::strcpy (skillText_[1], ACE_TEXT_ALWAYS_CHAR ("-"));
  ACE_OS::strcpy (skillText_[2], ACE_TEXT_ALWAYS_CHAR ("fish in a barrel"));
  ACE_OS::strcpy (skillText_[3], ACE_TEXT_ALWAYS_CHAR ("wimp"));
  ACE_OS::strcpy (skillText_[4], ACE_TEXT_ALWAYS_CHAR ("easy"));
  ACE_OS::strcpy (skillText_[5], ACE_TEXT_ALWAYS_CHAR ("normal"));
  ACE_OS::strcpy (skillText_[6], ACE_TEXT_ALWAYS_CHAR ("experienced"));
  ACE_OS::strcpy (skillText_[7], ACE_TEXT_ALWAYS_CHAR ("fun"));
  ACE_OS::strcpy (skillText_[8], ACE_TEXT_ALWAYS_CHAR ("insane"));
  ACE_OS::strcpy (skillText_[9], ACE_TEXT_ALWAYS_CHAR ("impossible"));

  loadTextures ();

  ACE_OS::strcpy (msgText_, ACE_TEXT_ALWAYS_CHAR (""));

  startMenu ();
}

Splot_Menu::~Splot_Menu ()
{
  deleteTextures ();
}

void
Splot_Menu::loadTextures ()
{
  std::string path_base = ACE_TEXT_ALWAYS_CHAR (SPLOT_IMAGE_DATA_DIR);
  path_base += ACE_DIRECTORY_SEPARATOR_STR;
  std::string filename = path_base + ACE_TEXT_ALWAYS_CHAR ("electric.png");
  texElec_   = Splot_Image::load (dataLoc (filename.c_str ()), IMG_NOMIPMAPS, IMG_BLEND3, GL_CLAMP, GL_LINEAR, GL_LINEAR);
  filename = path_base + ACE_TEXT_ALWAYS_CHAR ("menu_back.png");
  texBack_   = Splot_Image::load (dataLoc (filename.c_str ()), IMG_NOMIPMAPS, IMG_SOLID, GL_REPEAT, GL_LINEAR, GL_LINEAR);
  //filename = path_base + ACE_TEXT_ALWAYS_CHAR ("cursor.png");
  filename = path_base + ACE_TEXT_ALWAYS_CHAR ("heroAmmoFlash00.png");
  texCsr_    = Splot_Image::load (dataLoc (filename.c_str ()));
  filename = path_base + ACE_TEXT_ALWAYS_CHAR ("menu_updown.png");
  texUpdwn_  = Splot_Image::load (dataLoc (filename.c_str ()));

  //-- Environment map
  glTexGeni (GL_S, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP);
  glTexGeni (GL_T, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP);
  filename = path_base + ACE_TEXT_ALWAYS_CHAR ("reflect.png");
  texEnv_    = Splot_Image::load (dataLoc (filename.c_str ()), IMG_BUILDMIPMAPS, IMG_SOLID, GL_CLAMP, GL_LINEAR_MIPMAP_NEAREST, GL_LINEAR);

  listChrom_ = glGenLists (1);
  listBSU_ = glGenLists (1);

  const Configuration_t& configuration =
    SPLOT_CONFIGURATION_SINGLETON::instance ()->get ();
  if (!thickText_ &&
      configuration.debug)
    ACE_DEBUG ((LM_INFO,
                ACE_TEXT ("using thin lists to improve framerate...\n")));
  createLists (thickText_);
}

void
Splot_Menu::createLists (bool thick_in)
{
  glNewList (listChrom_, GL_COMPILE);
  textGeometryChromium (thickText_);
  glEndList ();

  glNewList (listBSU_, GL_COMPILE);
  textGeometryBSU (thickText_);
  glEndList ();
}

void
Splot_Menu::deleteTextures ()
{
  glDeleteTextures (1, &texElec_);
  glDeleteTextures (1, &texBack_);
  glDeleteTextures (1, &texEnv_);
  glDeleteTextures (1, &texCsr_);
  glDeleteTextures (1, &texUpdwn_);

  glDeleteLists (listChrom_, 1);
  glDeleteLists (listBSU_, 1);
}

void
Splot_Menu::startMenu ()
{
////	elecOffX	= elecStretch*0.4;
  //elecOffX	= 0.0;
  textAngle_ = 90.0;
  textCount_ = 500;
  //createLists( (thickText = true) );

  State_t& state = SPLOT_STATE_SINGLETON::instance ()->get ();
  state.cursor_pos[0] = 0.0;
  state.cursor_pos[1] = 0.0;
}

void
Splot_Menu::drawGL ()
{
  const Configuration_t& configuration =
    SPLOT_CONFIGURATION_SINGLETON::instance ()->get ();
  State_t& state = SPLOT_STATE_SINGLETON::instance ()->get ();

  if (--textCount_ < 0)
  {
    textCount_ = 500;
  //		textAngle_ = 360.0;
  } // end IF
  if (textAngle_ > 0.0) 
    textAngle_ -=  5.0;
  else
    textAngle_  =  0.0;

  //glClearColor(0.27451, 0.235294, 0.392157, 1.0);
  //-- Clear buffers
  glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  //-- Place camera
  glLoadIdentity ();
  glTranslatef (0.0, 0.0, configuration.z_trans);

  glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  glColor4f (1.0, 1.0, 1.0, 1.0);

  //-- Draw background
  state.ground->drawGL ();

  //-- Update audio
  state.audio->update ();

  int i;
  float szx  =  9.0;
  float szy  =  4.5;
  float top  =  1.0;
  float left = -8.0;
  float inc  = -txtHeight_*2.5F;

  //----- Draw credits texture --------------------------------
  glPushMatrix ();
  // NOTE: corners of back tex is white, alpha 1 and
  // we are in modulate blend...
  glBindTexture (GL_TEXTURE_2D, texBack_);
  glTexCoord2f (1.0, 0.0);

  //-- darken
  glBegin (GL_QUADS);
  glColor4f (0.0, 0.0, 0.0, 0.8F);
  glVertex3f ( szx,  szy+0.25F-3.0F, 10.0F);
  glVertex3f (-szx,  szy+0.25F-3.0F, 10.0F);
  glColor4f (0.0, 0.0, 0.0, 0.4F);
  glVertex3f (-szx, -13.0, 10.0);
  glVertex3f ( szx, -13.0, 10.0);
  glEnd ();

  glBegin (GL_QUADS);
  glColor4f (0.0, 0.0, 0.0, msgAlpha_);
  glVertex3f ( 16.0F, -10.7F, 10.0F);
  glVertex3f (-16.0F, -10.7F, 10.0F);
  glColor4f (0.0, 0.0, 0.0, msgAlpha_);
  glVertex3f (-16.0F, -11.9F, 10.0F);
  glVertex3f ( 16.0F, -11.9F, 10.0F);
  glEnd ();

  szx = 12.0;
  szy = txtHeight_*0.5F;
  glPushMatrix ();
  glTranslatef (left, top+(inc*currentSelection_), 10.0);
  glBegin (GL_QUADS);
  glColor4f (0.5, 0.5, 1.0, 1.0);
  glVertex3f (-szx,  szy*3.0F, 0.0);
  glVertex3f (-szx, -szy, 0.0);
  glColor4f (0.2F, 0.2F, 1.0F, 0.0);
  glVertex3f (szx, -szy, 0.0);
  glVertex3f (szx,  szy*3.0F, 0.0);
  glEnd ();
  drawIndicator ();

  glBlendFunc (GL_SRC_ALPHA, GL_ONE);
  drawElectric ();
  glPopMatrix ();
  glColor4f (1.0F, 1.0F, 1.0F, 0.9F);
  float sc = 0.035F;
  for (i = 0; i < MAX_MENU_TYPES; i++)
  {
    glPushMatrix ();
    glTranslatef (left, top+(inc*i), 10.0);
    glRotatef (textAngle_, 1.0, 0.0, 0.0);
    glScalef (sc, sc*0.75F, 1.0F);
    state.text->Render (menuText_[i]);
    glPopMatrix ();
  } // end FOR

  //---- highscores
  {
    float f = (float)-state.frame;
    float r = cos (f*0.02F);
    glPushMatrix ();
    //			glColor4f(1.0+r, r, r, 0.6+0.2*r);
    glColor4f (1.0F, 1.0F, 1.0F, 0.6F+0.2F*r);
    glTranslatef (-18.75, -8.5, 0.0);
    glScalef (sc, sc*0.75F, 1.0F);
    state.text->Render (ACE_TEXT_ALWAYS_CHAR ("high scores"));
    glTranslatef (-100.0, -30.0, 0.0);
    char buffer[16];
    float trans;
    time_t nowTime = ACE_OS::time (NULL);
    int l = SPLOT_CONFIGURATION_SINGLETON::instance ()->intSkill ();
    int recentHiScore = -1;
    time_t mostRecent = 0;
    time_t highscore_date = 0;
    for (i = 0; i < HI_SCORE_HIST; i++)
    {
      highscore_date = state.highscore->getDate (l, i);
      if (highscore_date > (nowTime-300) && // highlight score for 5 minutes (300)
          highscore_date > mostRecent)
      {
        recentHiScore = i;
        mostRecent = highscore_date;
      } // end IF
    } // end FOR
    for (i = 0; i < HI_SCORE_HIST; i++)
    {
      f += 30.0;
      r = cos (f*0.02F);
      if (i == recentHiScore)
        glColor4f (1.5F, 0.5F, 0.5F, 0.6F+0.1F*r);
      else
        glColor4f (1.0F, 1.0F, 1.0F, 0.2F+0.2F*r);
      //				glColor4f(0.5+r*0.5, 0.5, 0.25-r*0.25, 0.2+0.2*r);
      ACE_OS::sprintf (buffer, "%d", (int)state.highscore->getScore (l, i));
      trans = 80.0+state.text->Advance (ACE_TEXT_ALWAYS_CHAR ("high scores"))-state.text->Advance (buffer);
      glTranslatef (trans, 0.0, 0.0);
      state.text->Render (buffer);
      glTranslatef (-trans, -30.0, 0.0);
    } // end FOR
    glPopMatrix ();
  }

  //---- credits
  {
    int   n = 0;
    float alpha;
    static float c = 0.0;
    if (c > 75.0)
      c = 0.0;
    c += 0.2F;
    glPushMatrix ();
    if (c > 25) alpha = 0.4F*(75.0F-c)/50.0F;
    else alpha = 0.4F;
    glColor4f (1.0, 1.0, 1.0, alpha);
    sc = 0.03F;
    glTranslatef (14.0, -11.5, 0.0);
    glScalef (sc, sc, 1.0);
    glTranslatef (-c*1.5F, c, 0.0);
    if (c < 3) n = (int)c;
    else n = 3;
    if (n > 0) state.text->Render (ACE_TEXT_ALWAYS_CHAR ("the"), n);
    glTranslatef (c, -38+c, 0.0);
    if (c < 10) n = (int)(c-3);
    else n = 7;
    if (n > 0) state.text->Render (ACE_TEXT_ALWAYS_CHAR ("reptile"), n);
    glTranslatef (c, -38+c, 0.0);
    if (c < 16) n = (int)c-10;
    else n = 6;
    if (n > 0) state.text->Render (ACE_TEXT_ALWAYS_CHAR ("labour"), n);
    glTranslatef (c, -38+c, 0.0);
    if (c < 23) n = (int)(c-16);
    else n = 7;
    if (n > 0) state.text->Render (ACE_TEXT_ALWAYS_CHAR ("project"), n);
    // font height is 23
    glPopMatrix ();
  }

  //-------- draw help message
  if (msgAlpha_ > 0.0)
  {
    if (msgHelpOverride_)
      glColor4f (1.3F, msgAlpha_, msgAlpha_, msgAlpha_);
    else
      glColor4f (0.5F, 0.5F, 0.9F, (0.2F+msgAlpha_));
    sc = 0.042F;
    glTranslatef (-19.5, -14.0, 0.0);
    glScalef (sc, sc*0.75F, 1.0F);
    size_t len = mbstowcs (NULL, msgText_, 0);
    unsigned int ti = (unsigned int)(112.0*msgAlpha_);
    if (ti > len) ti = len;
    if (ti) state.text->Render (msgText_, ti);
    msgAlpha_ -= 0.004F;
    glColor4f (1.0, 1.0, 1.0, 1.0);
  } // end IF
  glPopMatrix ();

  glBlendFunc (GL_SRC_ALPHA, GL_ONE);
  glPushMatrix ();
  glTranslatef (0.0, 4.75, 25.0);
  glColor4f (1.0, 1.0, 1.0, 1.0);
  glDepthMask (GL_FALSE); //XXX Hack to make Voodoo3 XF4 work
  drawTitleBack ();
  glDepthMask (GL_TRUE); //XXX Hack to make Voodoo3 XF4 work
  drawTitle ();
  glPopMatrix ();

  //	//-- draw cursor...
  //	{
  //		float x = Global::cursorPos[0]*16.60;
  //		float y = Global::cursorPos[1]*12.45;
  //		float z = 10.0;
  //		float sz;
  //		glBindTexture(GL_TEXTURE_2D, csrTex);
  //		sz = 0.2;
  //		glBegin(GL_QUADS);
  //		glTexCoord2f(1.0, 1.0); glVertex3f( x+sz, y+sz, z);
  //		glTexCoord2f(0.0, 1.0); glVertex3f( x-sz, y+sz, z);
  //		glTexCoord2f(0.0, 0.0); glVertex3f( x-sz, y-sz, z);
  //		glTexCoord2f(1.0, 0.0); glVertex3f( x+sz, y-sz, z);
  //		glEnd();
  //	}

  if (thickText_ && state.fps < 30)
  {
    if (configuration.debug)
      ACE_DEBUG ((LM_INFO,
                  ACE_TEXT ("using thin lists to improve framerate...\n")));
    thickText_ = false;
    createLists (thickText_);
  } // end IF
  if (!thickText_ && state.fps > 40)
  {
    if (configuration.debug)
      ACE_DEBUG ((LM_INFO,
                  ACE_TEXT ("reverting to thick lists...\n")));
    thickText_ = true;
    createLists (thickText_);
  } // end IF

  //---------- Help messages
  if (msgHelpOverride_ && msgAlpha_ < 0.05)
  {
    msgHelpOverride_ = false;
    msgCount_ = 0;
  } // end IF
  if (!msgHelpOverride_)
  {
    int interval = (msgCount_++)%500;
    if (!interval)
    {
      ACE_OS::strcpy (msgText_, gettext (msgHelpText_[msgIndex_%NUM_HELP_MESSAGE_LINES]));
      msgIndex_++;
    } // end IF
    if (interval < 150)
      msgAlpha_ = interval/150.0F;
  } // end IF
}

//----------------------------------------------------------
void Splot_Menu::drawIndicator()
{
	Config	*config = Config::instance();
	Global	*game = Global::getInstance();
	char	buf[64];
	float	szx = 10.0;
	float	szy = txtHeight;
	float	level = 0.0;
	float	sc = 0.025F;
	int		tmp;
	switch(curSel)
	{
		case GameLevel: 
			level = game->gameLevel/9.0;
			sprintf(buf, "%d", game->gameLevel);	 
			break;
		case SkillLevel: 
			level = config->gameSkillBase();
			tmp = (int)((level+0.05)*10.0);
			sprintf(buf, "%s", skillString(tmp));
			break;
		case Graphics: 
			level = config->gfxLevel()/2.0;
			switch(config->gfxLevel())
			{
				case 0: sprintf(buf, _("low")); break;
				case 1: sprintf(buf, _("med")); break;
				case 2: sprintf(buf, _("high")); break;
			}
			break;
		case ScreenSize: 
			level = (float)config->approxScreenSize()/(float)MAX_SCREEN_SIZE;
			sprintf(buf, _("%dx%d"), config->screenW(), config->screenH());
			break;
		case FullScreen: 
			level = (float)config->fullScreen(); 
			if(config->fullScreen()) sprintf(buf, _("true"));
			else sprintf(buf, _("false"));
			break;
		case Sound: 
			level = config->volSound(); 
			sprintf(buf, "%d", (int)((level+0.05)*10.0));	 
			break;
		case Music: 
			level = config->volMusic(); 
			sprintf(buf, "%d", (int)((level+0.05)*10.0));	 
			break;
		case MovementSpeed:
			level = config->movementSpeed()*10.0;
			sprintf(buf, "%d", (int)((level+0.005)*100.0));	 
			break;
		default: 
			level = -5.0;
			break;
	}
	glPushMatrix();
		glTranslatef(0.0, -txtHeight, 0.0);
		glBegin(GL_QUADS);
			glColor4f(1.0F, 1.0F, 1.0F, 0.3F);
			glVertex3f(szx+szy, szy, 0.0);
			glVertex3f(   -szx, szy, 0.0);
			glColor4f(1.0F, 1.0F, 1.0F, 0.15F);
			glVertex3f(   -szx, 0.0, 0.0);
			glVertex3f(    szx, 0.0, 0.0);
		glEnd();
		
		//-- draw level indicator and/or text
		glPushMatrix();
		if(level > -1.0)
		{
			glBegin(GL_QUADS);
				glColor4f(1.0, 0.0, 0.0, 1.0);
				glVertex3f(szy+szx*level, szy, 0.0);
				glVertex3f(          0.0, szy, 0.0);
				glColor4f(0.0, 0.0, 0.0, 0.0);
				glVertex3f(          0.0, 0.0, 0.0);
				glVertex3f(    szx*level, 0.0, 0.0);
			glEnd();
			
			//-- draw +/- buttons ---
			float	bx = butWidth;
			float	by = butHeight;
			float	bo = butOffset;
			glBindTexture(GL_TEXTURE_2D, updwnTex);
			glBegin(GL_QUADS);
				glColor4f(1.0F, 1.0F, 1.0F, 0.6F);
				glTexCoord2f (1.0, 0.0); glVertex3f (  bx-bo,  by, 0.0);
				glTexCoord2f (0.0, 0.0); glVertex3f (0.0F-bo,  by, 0.0);
				glTexCoord2f (0.0, 1.0); glVertex3f (0.0F-bo, 0.0, 0.0);
				glTexCoord2f (1.0, 1.0); glVertex3f (  bx-bo, 0.0, 0.0);
			glEnd();
			
			glColor4f(1.0, 1.0, 1.0, 0.5);
			glTranslatef(11.0, 0.0, 0.0);
			glScalef(sc, sc, 1.0);
			game->text->Render(buf);
		}
		glPopMatrix();
		
	glPopMatrix();
	
}
	
//----------------------------------------------------------
void Splot_Menu::drawElectric()
{
	float es = elecStretch;
	elecOffY = FRAND*0.7;
	if( (elecOffX+=0.175) > es)
		elecOffX = -5.0;
	float szx = 30.0;
	float szy = txtHeight;
	glBindTexture(GL_TEXTURE_2D, elecTex);
	glPushMatrix();
	glTranslatef(0.0, txtHeight*0.5, 0.0);
	glBegin(GL_QUADS);
		glColor4f(1.0, 1.0, 1.0, 1.0);
		glTexCoord2f(   elecOffX, elecOffY+0.3); glVertex3f( -8.0,  szy, 0.0);
		glTexCoord2f(   elecOffX,     elecOffY); glVertex3f( -8.0, -szy, 0.0);
		glColor4f(0.0, 0.0, 0.1F, 1.0F);
		glTexCoord2f(elecOffX-es,     elecOffY); glVertex3f(  szx, -szy, 0.0);
		glTexCoord2f(elecOffX-es, elecOffY+0.3); glVertex3f(  szx,  szy, 0.0);
	glEnd();
	glPopMatrix();
}
	
//----------------------------------------------------------
void Splot_Menu::drawTitleBack()
{
	float	clr_c[4] = { 1.0, 1.0, 1.0, 0.0 };
	float	clr_w[4] = { 1.0, 1.0, 1.0, 1.0 };
	float	w = 9.0;
	float	a = 2.0;
	float	h = 1.4F;
	float	z = 0.5;
		
	float	as = a/(w+a);
	float	at = a/(h+a);
	
	glBindTexture(GL_TEXTURE_2D, backTex);
	glMatrixMode(GL_TEXTURE);
	glPushMatrix();
	static float amt = 0.0;
	glTranslatef(amt*0.1F, amt*0.5F, 0.0);
	glRotatef(-amt*100.0F, 0.0, 1.0F, 1.0F);
	amt -= 0.01F;
	//-- Top right
	glBegin(GL_TRIANGLE_STRIP);
		glTexCoord2f(1.0   , 1.0-at); glColor4fv(clr_c);	glVertex3f(  w+a,  h  , 0.0);
		glTexCoord2f(1.0   , 1.0   ); glColor4fv(clr_c);	glVertex3f(  w+a,  h+a, 0.0);
		glTexCoord2f(1.0-as, 1.0-at); glColor4fv(clr_w);	glVertex3f(  w  ,  h  , 0.0);
		glTexCoord2f(1.0-as, 1.0   ); glColor4fv(clr_c);	glVertex3f(  w  ,  h+a, 0.0);
		glTexCoord2f(0.0   , 1.0-at); glColor4fv(clr_w);	glVertex3f(  0.0,  h  , 0.0);
		glTexCoord2f(0.0   , 1.0   ); glColor4fv(clr_c);	glVertex3f(  0.0,  h+a, 0.0);
	glEnd();
	glBegin(GL_TRIANGLE_STRIP);
		glTexCoord2f(1.0   , 0.0   ); glColor4fv(clr_c);	glVertex3f(  w+a, 0.0, z);
		glTexCoord2f(1.0   , 1.0-at); glColor4fv(clr_c);	glVertex3f(  w+a,	h, 0.0);
		glTexCoord2f(1.0-as, 0.0   ); glColor4fv(clr_w);	glVertex3f(  w  , 0.0, z);
		glTexCoord2f(1.0-as, 1.0-at); glColor4fv(clr_w);	glVertex3f(  w  ,	h, 0.0);
		glTexCoord2f(0.0   , 0.0   ); glColor4fv(clr_w);	glVertex3f(  0.0, 0.0, z);
		glTexCoord2f(0.0   , 1.0-at); glColor4fv(clr_w);	glVertex3f(  0.0,	h, 0.0);
	glEnd();
	//-- Top left
	glBegin(GL_TRIANGLE_STRIP);
		glTexCoord2f(0.0   , 1.0-at); glColor4fv(clr_w);	glVertex3f(  0.0,  h  , 0.0);
		glTexCoord2f(0.0   , 1.0   ); glColor4fv(clr_c);	glVertex3f(  0.0,  h+a, 0.0);
		glTexCoord2f(1.0-as, 1.0-at); glColor4fv(clr_w);	glVertex3f( -w  ,  h  , 0.0);
		glTexCoord2f(1.0-as, 1.0   ); glColor4fv(clr_c);	glVertex3f( -w  ,  h+a, 0.0);
		glTexCoord2f(1.0   , 1.0-at); glColor4fv(clr_c);	glVertex3f( -w-a,  h  , 0.0);
		glTexCoord2f(1.0   , 1.0   ); glColor4fv(clr_c);	glVertex3f( -w-a,  h+a, 0.0);
	glEnd();		
	glBegin(GL_TRIANGLE_STRIP);
		glTexCoord2f(0.0   , 0.0   ); glColor4fv(clr_w);	glVertex3f(  0.0, 0.0, z);
		glTexCoord2f(0.0   , 1.0-at); glColor4fv(clr_w);	glVertex3f(  0.0,	h, 0.0);
		glTexCoord2f(1.0-as, 0.0   ); glColor4fv(clr_w);	glVertex3f( -w  , 0.0, z);
		glTexCoord2f(1.0-as, 1.0-at); glColor4fv(clr_w);	glVertex3f( -w  ,	h, 0.0);
		glTexCoord2f(1.0   , 0.0   ); glColor4fv(clr_c);	glVertex3f( -w-a, 0.0, z);
		glTexCoord2f(1.0   , 1.0-at); glColor4fv(clr_c);	glVertex3f( -w-a,	h, 0.0);
	glEnd();
	//-- bottom right
	glBegin(GL_TRIANGLE_STRIP);
		glTexCoord2f(1.0   , 1.0-at); glColor4fv(clr_c);	glVertex3f(  w+a,  -h, 0.0);
		glTexCoord2f(1.0   , 0.0   ); glColor4fv(clr_c);	glVertex3f(  w+a, 0.0, z);
		glTexCoord2f(1.0-as, 1.0-at); glColor4fv(clr_w);	glVertex3f(  w  ,  -h, 0.0);
		glTexCoord2f(1.0-as, 0.0   ); glColor4fv(clr_w);	glVertex3f(  w  , 0.0, z);
		glTexCoord2f(0.0   , 1.0-at); glColor4fv(clr_w);	glVertex3f(  0.0,  -h, 0.0);
		glTexCoord2f(0.0   , 0.0   ); glColor4fv(clr_w);	glVertex3f(  0.0, 0.0, z);
	glEnd();
	glBegin(GL_TRIANGLE_STRIP);
		glTexCoord2f(1.0   , 1.0   ); glColor4fv(clr_c);	glVertex3f(  w+a, -h-a, 0.0);
		glTexCoord2f(1.0   , 1.0-at); glColor4fv(clr_c);	glVertex3f(  w+a, -h  , 0.0);
		glTexCoord2f(1.0-as, 1.0   ); glColor4fv(clr_c);	glVertex3f(  w  , -h-a, 0.0);
		glTexCoord2f(1.0-as, 1.0-at); glColor4fv(clr_w);	glVertex3f(  w  , -h  , 0.0);
		glTexCoord2f(0.0   , 1.0   ); glColor4fv(clr_c);	glVertex3f(  0.0, -h-a, 0.0);
		glTexCoord2f(0.0   , 1.0-at); glColor4fv(clr_w);	glVertex3f(  0.0, -h  , 0.0);
	glEnd();
	//bottom left
	glBegin(GL_TRIANGLE_STRIP);
		glTexCoord2f(0.0   , 1.0-at); glColor4fv(clr_w);	glVertex3f(  0.0,  -h, 0.0);
		glTexCoord2f(0.0   , 0.0   ); glColor4fv(clr_w);	glVertex3f(  0.0, 0.0, z);
		glTexCoord2f(1.0-as, 1.0-at); glColor4fv(clr_w);	glVertex3f( -w  ,  -h, 0.0);
		glTexCoord2f(1.0-as, 0.0   ); glColor4fv(clr_w);	glVertex3f( -w  , 0.0, z);
		glTexCoord2f(1.0   , 1.0-at); glColor4fv(clr_c);	glVertex3f( -w-a,  -h, 0.0);
		glTexCoord2f(1.0   , 0.0   ); glColor4fv(clr_c);	glVertex3f( -w-a, 0.0, z);
	glEnd();
	glBegin(GL_TRIANGLE_STRIP);
		glTexCoord2f(0.0   , 1.0   ); glColor4fv(clr_c);	glVertex3f(  0.0, -h-a, 0.0);
		glTexCoord2f(0.0   , 1.0-at); glColor4fv(clr_w);	glVertex3f(  0.0, -h  , 0.0);
		glTexCoord2f(1.0-as, 1.0   ); glColor4fv(clr_c);	glVertex3f( -w  , -h-a, 0.0);
		glTexCoord2f(1.0-as, 1.0-at); glColor4fv(clr_w);	glVertex3f( -w  , -h  , 0.0);
		glTexCoord2f(1.0   , 1.0   ); glColor4fv(clr_c);	glVertex3f( -w-a, -h-a, 0.0);
		glTexCoord2f(1.0   , 1.0-at); glColor4fv(clr_c);	glVertex3f( -w-a, -h  , 0.0);
	glEnd();
	
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);	
	
}

//----------------------------------------------------------
void Splot_Menu::drawTitle()
{
	static int tiltCount = 600;
	static float ta0  = -60.0;
	static float ta1  = -90.0;
	float &tilt = titleTilt;
	if(ta0 < 90.0)	ta0 += 0.7;
	else if(!thickText) ta0 += 180.0;
	else ta0 += 5.0;
	if(ta0 > 270.0)	ta0 = ta0-360.0;
		
	if(ta1 < 90.0)	ta1 += 0.55;
	else if(!thickText) ta1 += 180.0;
	else ta1 += 8.0;
	if(ta1 > 270.0)	ta1 = ta1-360.0;
	
	if(thickText)
	{
		tiltCount--;
		if(tiltCount == 0)
			tilt = 360.0+tilt;
		else if(tiltCount < 0)
		{
			tilt -= 1.0;
			if(tilt < -10.0)
			{
				tiltCount = 1500;
				tilt = -10.0;
			}
		}
		else
			tilt -= 0.01;
	}

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glColor4f(1.0, 1.0, 1.0, 1.0);
	glPushMatrix();
		glEnable(GL_TEXTURE_GEN_S);
		glEnable(GL_TEXTURE_GEN_T);
		glBindTexture(GL_TEXTURE_2D, envTex);
		glPushMatrix();
			glTranslatef(0.0,  1.0, 0.0);
			glRotatef( tilt, 1.0, 0.0, 0.0);
			glRotatef(   ta0, 0.0, 1.0, 0.0);
			glCallList(listChrom);
		glPopMatrix();
		glPushMatrix();
			glTranslatef(0.0, -1.0, 0.0);
			glRotatef( tilt, 1.0, 0.0, 0.0);
			glRotatef(   ta1, 0.0, 1.0, 0.0);
			glCallList(listBSU);
		glPopMatrix();
		glDisable(GL_TEXTURE_GEN_S);
		glDisable(GL_TEXTURE_GEN_T);
	glPopMatrix();
	glDisable(GL_DEPTH_TEST);
}

//----------------------------------------------------------
void Splot_Menu::keyHit(MainToolkit::Key key)
{
	switch(key)
	{
		case MainToolkit::KeyUp:
			if(curSel == (MenuSelection)0)
				curSel = (MenuSelection)(NumSelections-1);
			else
				curSel = (MenuSelection)(curSel - 1);
//			curSel = (MenuSelection)(curSel - 1);
//			if(curSel < (MenuSelection)0)
//				curSel = (MenuSelection)(NumSelections-1);
			elecOffX = 0.0;
			break;
		case MainToolkit::KeyDown:
			curSel = (MenuSelection)((curSel+1)%NumSelections);
			elecOffX = 0.0;
			break;
		case MainToolkit::KeyLeft:
			decItem();
			break;
		case MainToolkit::KeyRight:
			incItem();
			break;
		case MainToolkit::KeyEnter:
			activateItem();
			break;
		default:
			break;
	}
}

//----------------------------------------------------------
void Splot_Menu::activateItem()
{
	Config	*config = Config::instance();
	Global *game = Global::getInstance();
	switch(curSel)
	{
		case NewGame:
			game->gameMode = Global::Game;
			game->newGame();
			game->toolkit->grabMouse(true);
			game->audio->setMusicMode(Audio::MusicGame);
			break;
		case SkillLevel:
			break;
		case GameLevel:
			break;
		case Graphics:
			break;
		case ScreenSize:
			break;
		case FullScreen:
			config->setFullScreen(!config->fullScreen());
			game->deleteTextures();
			if( !game->toolkit->setVideoMode() )
			{
				mssgHelpOverride = true;
				mssgAlpha = 1.1;
				if( config->fullScreen() )
					sprintf(mssgText, _("---- error setting full screen mode ----"));
				else
					sprintf(mssgText, _("---- error setting window mode ----"));
				config->setFullScreen(!config->fullScreen());
			}
			game->loadTextures();
			break;
		case Sound:
			break;
		case Music:
			break;
		case MovementSpeed:
			break;
		case Quit:
			game->game_quit = true;
			break;
		case NumSelections:
			break;
	}
}

//----------------------------------------------------------
void Splot_Menu::incItem()
{
	Config	*config = Config::instance();
	HiScore *hiScore = HiScore::getInstance();
	float	pos[3] = { 0.0, 0.0, 25.0 };
	switch(curSel)
	{
		case NewGame:
			activateItem();
			break;
		case SkillLevel:
			config->setGameSkillBase(config->gameSkillBase()+0.1);
			if( config->debug() ) hiScore->print(config->intSkill());
			game->newGame();
			break;
		case GameLevel:
			game->gameLevel++;
			if(game->gameLevel > config->maxLevel()) 
			{
				mssgHelpOverride = true;
				mssgAlpha = 1.1;
				sprintf(mssgText, _("---- you must complete level %d before you can select level %d ----"), config->maxLevel(), game->gameLevel);
				game->gameLevel = config->maxLevel();
			}
			else
				game->newGame();
			break;
		case Graphics:
			config->setGfxLevel(config->gfxLevel()+1);
			break;
		case ScreenSize:
			config->setScreenSize(config->approxScreenSize()+1);
			game->deleteTextures();
			if( !game->toolkit->setVideoMode() )
			{
				mssgHelpOverride = true;
				mssgAlpha = 1.1;
				sprintf(mssgText, _("---- error setting screen size ----"));
				config->setScreenSize(config->approxScreenSize()-1);
			}
			game->loadTextures();
			break;
		case FullScreen:
			if(!config->fullScreen())
			{
				config->setFullScreen(true);
				game->deleteTextures();
				if( !game->toolkit->setVideoMode() )
				{
					mssgHelpOverride = true;
					mssgAlpha = 1.1;
					sprintf(mssgText, _("---- error setting full screen mode ----"));
					config->setFullScreen(false);
				}
				game->loadTextures();
			}
			break;
		case Sound:
			config->setVolSound(config->volSound()+0.05);
			game->audio->setSoundVolume(config->volSound());
			game->audio->playSound(Audio::Explosion, pos);
			break;
		case Music:
			config->setVolMusic(config->volMusic()+0.05);
			game->audio->setMusicVolume(config->volMusic());
			break;
		case MovementSpeed:
			config->setMovementSpeed(config->movementSpeed()+0.005);
			break;
		case Quit:
			activateItem();
			break;
		case NumSelections:
			break;
	}
}

//----------------------------------------------------------
void Splot_Menu::decItem()
{
	Config	*config = Config::instance();
	HiScore *hiScore = HiScore::getInstance();
	float	pos[3] = { 0.0, 0.0, 25.0 };
	switch(curSel)
	{
		case NewGame:
			break;
		case SkillLevel:
			config->setGameSkillBase(config->gameSkillBase()-0.1);
			if( config->debug() ) hiScore->print(config->intSkill());
			game->newGame();
			break;
		case GameLevel:
			game->gameLevel -= 1;
			if(game->gameLevel < 1) 
				game->gameLevel = 1;
			game->newGame();
			break;
		case Graphics:
			config->setGfxLevel(config->gfxLevel()-1);
			break;
		case ScreenSize:
			config->setScreenSize(config->approxScreenSize()-1);
			game->deleteTextures();
			if( !game->toolkit->setVideoMode() )
			{
				mssgHelpOverride = true;
				mssgAlpha = 1.1;
				sprintf(mssgText, _("---- error setting screen size ----"));
				config->setScreenSize(config->approxScreenSize()+1);
			}
			game->loadTextures();
			break;
		case FullScreen:
			if(config->fullScreen())
			{
				config->setFullScreen(false);
				game->deleteTextures();
				if( !game->toolkit->setVideoMode() )
				{
					mssgHelpOverride = true;
					mssgAlpha = 1.1;
					sprintf(mssgText, _("---- error setting full screen mode ----"));
					config->setFullScreen(true);
				}
				game->loadTextures();
			}
			break;
		case Sound:
			config->setVolSound(config->volSound()-0.05);
			game->audio->setSoundVolume(config->volSound());
			game->audio->playSound(Audio::Explosion, pos);
			break;
		case Music:
			config->setVolMusic(config->volMusic()-0.05);
			game->audio->setMusicVolume(config->volMusic());
			break;
		case MovementSpeed:
			config->setMovementSpeed(config->movementSpeed()-0.005);
			break;
		case Quit:
			break;
		case NumSelections:
			break;
	}
}

/**
 * horrible, hacky, but quick to implement....
 */
//----------------------------------------------------------
void Splot_Menu::mousePress(MainToolkit::Button but, int xi, int yi)
{
	if(but == MainToolkit::Left)
	{
		float x,y;
		Config *config = Config::instance();
		x = -2.0*(0.5-(((float)xi)/config->screenW()))* 16.60;
		y =  2.0*(0.5-(((float)yi)/config->screenH()))* 12.45;

		float	p = -y+(1.0+txtHeight*1.5);
		float	s = txtHeight*2.5;
		int		mSel = -1;
		if(p > 0.0)
		{
			// reset electric 
			elecOffX = 0.0;

			p = p/s;
			mSel = (int)floor(p);
			if( mSel >= 0 && mSel < (int)NumSelections)
			{
				if(mSel != (int)curSel)
				{
					curSel = (MenuSelection)mSel;
					elecOffX = 0.0;
					mSel = -1;
				}
			}
		}
		float l  = -8.0 - butOffset;
		float hw = butWidth*0.5;
		if(mSel >= 0)
		{
			if(x > l && x < l+hw )
				decItem();
			else if (x > l+hw && x < l+butWidth)
				incItem();
			else
				activateItem();
		}
	}
}

