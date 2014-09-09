#ifndef CONFIGURATION_H
#define CONFIGURATION_H

#include <string>

#include "ace/Global_Macros.h"
#include "ace/Singleton.h"
#include "ace/Synch.h"

#include "defines.h"
#include "types.h"

class Splot_Configuration
{
  // singleton requires access to the ctor/dtor
  friend class ACE_Singleton<Splot_Configuration,
                             ACE_Null_Mutex>;

 public:
  bool init (int,         // argc
             ACE_TCHAR**, // argv
             Mode_t&);    // return value: mode

  static std::string getFileName ();

  bool  save ();

  const Configuration_t& get () const;

  void setScreenSize(int m);
  void setScreenSize(int w, int h);
  int approxScreenSize ();

  void setFullScreen (bool fullScreen_in) { configuration_.full_screen = fullScreen_in; }
  void setGfxLevel (int level_in)
  {
    configuration_.graphics_level = level_in;
    if (configuration_.graphics_level < 0)
      configuration_.graphics_level = 0;
    if (configuration_.graphics_level > 2)
      configuration_.graphics_level = 2;
  }

  void setMovementSpeed (float speed_in) {
    configuration_.movement_speed = speed_in;
    if (configuration_.movement_speed < MOVEMENT_SPEED_MIN)
      configuration_.movement_speed = MOVEMENT_SPEED_MIN;
    if (configuration_.movement_speed > MOVEMENT_SPEED_MAX)
      configuration_.movement_speed = MOVEMENT_SPEED_MAX;
  }

  void setAudioEnabled (bool audioEnabled_in) { configuration_.audio_enabled = audioEnabled_in; };
  void setVolSound (float volume_in) {
    configuration_.volume_sound = volume_in;
    if (configuration_.volume_sound < 0.0)
      configuration_.volume_sound = 0.0;
    if (configuration_.volume_sound > 1.0)
      configuration_.volume_sound = 1.0;
  }
  void setVolMusic (float volume_in) {
    configuration_.volume_music = volume_in;
    if (configuration_.volume_music < 0.0)
      configuration_.volume_music = 0.0;
    if (configuration_.volume_music > 1.0)
      configuration_.volume_music = 1.0;
  }

  void setSkillBase (float skill_in) {
    configuration_.skill_base = skill_in;
    if (configuration_.skill_base > SKILL_BASE_MAX)
      configuration_.skill_base = SKILL_BASE_MAX;
    if (configuration_.skill_base < SKILL_BASE_MIN)
      configuration_.skill_base = SKILL_BASE_MIN;
  }
  int intSkill () { return (int)((configuration_.skill_base+0.05)*10.0); }

 private:
  Configuration_t configuration_;

  Splot_Configuration ();
  virtual ~Splot_Configuration ();
  ACE_UNIMPLEMENTED_FUNC (Splot_Configuration (const Splot_Configuration&));
  ACE_UNIMPLEMENTED_FUNC (Splot_Configuration& operator= (const Splot_Configuration&));

  bool load (const std::string&); // FQ filename
};

typedef ACE_Singleton<Splot_Configuration,
                      ACE_Null_Mutex> SPLOT_CONFIGURATION_SINGLETON;
//SPLOT_SINGLETON_DECLARE(ACE_Singleton, Splot_Configuration, ACE_Null_Mutex);

#endif // CONFIGURATION_H
