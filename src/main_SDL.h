#ifndef MAINSDL_H
#define MAINSDL_H

#ifdef HAVE_CONFIG_H
#include "splot-config.h"
#endif

#ifdef USE_SDL
#include "SDL.h"

#include "main.h"

class Splot_MainSDL
 : public Splot_Main
{
 public:
  Splot_MainSDL (int = 0,             // argc
                 ACE_TCHAR** = NULL); // argv
  virtual ~Splot_MainSDL ();

  bool run ();
  bool checkErrors ();

  bool setVideoMode ();

  void grabMouse (bool,         // grab cursor ? : release
                  bool = true); // warp ?

  bool process (SDL_Event*);
  void joystickMove ();
  void keyMove ();

 private:
  typedef Splot_Main inherited;

  void activation (bool,  // window (minimized/restored)
                   bool,  // mouse focus
                   bool,  // input focus
                   bool); // window/focus gained ? : lost
  void keyDown (SDL_Event*);
  void keyDownGame (SDL_Event*);
  void keyUp (SDL_Event*);
  void mouseMotion (SDL_Event*);
  void mouseButtonDown (SDL_Event*);
  void mouseButtonUp (SDL_Event*);
  void joystickMotion (SDL_Event*);
  void joystickButtonDown (SDL_Event*);
  void joystickButtonUp (SDL_Event*);

  //void saveEvent (SDL_Event*);
  //SDL_Event* getEvent (FILE*);

 private:
#if SDL_VERSION_ATLEAST (2,0,0)
  SDL_Window*   window_;
  SDL_GLContext context_;
#endif
#ifdef WITH_SDL_JOYSTICK
  SDL_Joystick* joystick_;
#endif
  int           fire_;
  bool          mouseToggle_;
  bool          joystickToggle_;
  int           last_[2];
  int           mid_[2];
  float         joy_[2];
  float         jNow_[2];
  int           adjCount_;
  float         key_speed_[2];
};
#endif // USE_SDL

#endif // MAINSDL_H
