#ifndef MAINSDL_H
#define MAINSDL_H

#ifdef USE_SDL
#include "SDL.h"

#include "main.h"

//class Global;

class Splot_MainSDL
 : public Splot_Main
{
 public:
  Splot_MainSDL (int = 0,        // argc
                 char** = NULL); // argv
  virtual ~Splot_MainSDL ();

  bool run ();
  bool checkErrors ();

  bool setVideoMode ();

  void grabMouse (bool, bool = true);

  bool process (SDL_Event*);
  void joystickMove ();
  void keyMove ();

 private:
  typedef Splot_Main inherited;

  void activation (bool shown, bool mouse, bool input, bool gain);
  void keyDown (SDL_Event*);
  void keyDownGame (SDL_Event*);
  void keyUp (SDL_Event*);
  void mouseMotion (SDL_Event*);
  void mouseButtonDown (SDL_Event*);
  void mouseButtonUp (SDL_Event*);
  void joystickMotion (SDL_Event*);
  void joystickButtonDown (SDL_Event*);
  void joystickButtonUp (SDL_Event*);

  void saveEvent (SDL_Event*);
  SDL_Event* getEvent (FILE*);

 private:
#if SDL_VERSION_ATLEAST (2,0,0)
  SDL_Window* window;
  SDL_GLContext context;
#endif
  SDL_Joystick* joystick_;
  int           fire_;
  bool          mouseToggle_;
  int           last_[2];
  int           mid_[2];
  float         joy_[2];
  float         jNow_[2];

  int           adjCount_;
  float         key_speed_[2];
};
#endif // USE_SDL

#endif // MAINSDL_H
