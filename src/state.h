#ifndef STATE_H
#define STATE_H

#include "ace/Global_Macros.h"
#include "ace/Singleton.h"
#include "ace/Synch.h"

#include "defines.h"
#include "types.h"

class Splot_State
{
  // singleton requires access to the ctor/dtor
  friend class ACE_Singleton<Splot_State,
                             ACE_Thread_Mutex>;

 public:
  static void initialize ();

  bool initialize (GameToolkitType_t, // toolkit
                   int,               // argc
                   ACE_TCHAR**);      // argv
  void run ();

  State_t& get () { return state_; };
  GameState_t& gameState () { return game_state_; };

  void initGame ();
  void finiGame ();
  void newGame ();
  void resetGame (bool = false); // player only ?
  void gotoNextLevel ();

  static int   randomI[256];
  static float randomF[256];
  static float randomS[256];
  static int   random_index;

  void loadTextures ();
  void deleteTextures ();

 private:
  State_t state_;
  GameState_t game_state_;

  Splot_State ();
  virtual ~Splot_State ();
  ACE_UNIMPLEMENTED_FUNC (Splot_State (const Splot_State&));
  ACE_UNIMPLEMENTED_FUNC (Splot_State& operator= (const Splot_State&));

  void initializeRandomness ();

#if defined (USE_STATIC_RANDOMNESS)
  static int randomData[256];
#endif
};

typedef ACE_Singleton<Splot_State,
                      ACE_Thread_Mutex> SPLOT_STATE_SINGLETON;
//SPLOT_SINGLETON_DECLARE(ACE_Singleton, Splot_State, ACE_Thread_Mutex);

#endif // STATE_H
