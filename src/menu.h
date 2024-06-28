#ifndef MENU_H
#define MENU_H

#ifdef HAVE_CONFIG_H
#include "splot_config.h"
#endif

#ifdef USE_SDL
#include "SDL_opengl.h"
#elif defined (HAVE_APPLE_OPENGL_FRAMEWORK) || defined (HAVE_OPENGL_GL_H)
#include <OpenGL/gl.h>
#else
#include "GL/gl.h"
#endif

#include "ace/Default_Constants.h"

#include "defines.h"
#include "types.h"

enum MenuSelection_t
{
  MENU_INVALID = -1,
  MENU_NEWGAME = 0,
  MENU_GAME_LEVEL,
  MENU_SKILL_LEVEL,
  MENU_GRAPHICS,
  MENU_FULLSCREEN,
  MENU_SCREENSIZE,
  MENU_SOUND,
  MENU_MUSIC,
  MENU_MOVEMENTSPEED,
  MENU_QUIT,
  ///////////////////////////////////////
  MAX_MENU_TYPES
};

class Splot_Menu
{
 public:
  Splot_Menu ();
  ~Splot_Menu ();

  void startMenu ();
  void keyHit (Key_t);
  void mousePress (Button_t, int, int); // button / position

  void drawGL ();

  void loadTextures ();
  void deleteTextures ();

 protected:
  void activateItem ();
  void incItem ();
  void decItem ();

  void drawIndicator ();
  void drawElectric ();

  void drawTitle ();
  void drawTitleBack ();

  void createLists (bool); // thick ?

 private:
  MenuSelection_t currentSelection_;

  char menuText_[MAX_MENU_TYPES][BUFSIZ];
  char skillText_[SKILL_LEVEL_MAX_INT][BUFSIZ];

  GLuint listChrom_;
  GLuint listBSU_;

  GLuint texEnv_;
  GLuint texCsr_;
  GLuint texBack_;
  GLuint texElec_;
  GLuint texUpdwn_;

  float elecOff_[2];
  float elecStretch_;
  float textAngle_;
  int   textCount_;

  float txtHeight_;
  float butSize_[2];
  float butOffset_;

  bool  thickText_;

  float titleTilt_;

  float msgAlpha_;
  char  msgText_[BUFSIZ];
  int   msgIndex_;
  int   msgCount_;
  bool  msgHelpOverride_;

  static char hints[NUM_HELP_MESSAGE_LINES][BUFSIZ];
};

#endif // MENU_H
