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
  bool init ();
  void run ();

  const State_t& get () const { return state_; };
  const GameState_t& gameState() const { return game_state_; };

  void newGame ();
  void gotoNextLevel ();
  void generateRandom (bool = true); // true randomness ?
  bool setToolkit (GameToolkit_t, // toolkit
                   int,           // argc
                   char**);       // argv

 private:
  State_t state_;
  GameState_t game_state_;

  Splot_State ();
  virtual ~Splot_State ();
  ACE_UNIMPLEMENTED_FUNC (Splot_State (const Splot_State&));
  ACE_UNIMPLEMENTED_FUNC (Splot_State& operator= (const Splot_State&));

  void createGame ();
  void deleteGame ();

  void deleteTextures ();
  void loadTextures ();
};

typedef ACE_Singleton<Splot_State,
                      ACE_Thread_Mutex> SPLOT_STATE_SINGLETON;
//SPLOT_SINGLETON_DECLARE(ACE_Singleton, Splot_State, ACE_Thread_Mutex);

#endif // STATE_H
