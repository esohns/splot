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
                             ACE_Thread_Mutex>;

 public:
  bool init (int,         // argc
             ACE_TCHAR**, // argv
             Mode_t&);    // return value: mode

  static std::string getFileName ();
  //static std::string getOldFileName ();
  bool  save ();

  const Configuration_t& get () const;

  void	setScreenSize(int m);
  void	setScreenSize(int w, int h);
  int	approxScreenSize();

  void setFullScreen (bool set_in) { configuration_.full_screen = set_in; }
  void setBlend (bool set_in) { configuration_.blend_enabled = set_in; }
  void setGfxLevel(int level_in)
  {
    configuration_.gfx_level = level_in;
    if (configuration_.gfx_level < 0)
      configuration_.gfx_level = 0;
    if (configuration_.gfx_level > 2)
      configuration_.gfx_level = 2;
  }
  void setTrueColor (bool set_in) { configuration_.true_color = set_in; }
  void setTexBorder (bool set_in) { configuration_.tex_border = set_in; }
  void setShowFPS (bool set_in) { configuration_.show_fps = set_in; }
  void setViewGamma (float gamma_in) { configuration_.view_gamma = gamma_in; }

  void setAutoSpeed (bool set_in) { configuration_.auto_speed = set_in; }
  void setMovementSpeed (float speed_in) {
    configuration_.movement_speed = speed_in;
    if (configuration_.movement_speed < MOVEMENT_SPEED_MIN)
      configuration_.movement_speed = MOVEMENT_SPEED_MIN;
    if (configuration_.movement_speed > MOVEMENT_SPEED_MAX)
      configuration_.movement_speed = MOVEMENT_SPEED_MAX;
  }

  void setAudio (bool set_in) { configuration_.audio_enabled = set_in; }
  void setSwapStereo (bool set_in) { configuration_.swap_stereo = set_in; }
  void setUsePlayList (bool set_in) { configuration_.use_playList = set_in; }
  void setUseCDROM (bool set_in) { configuration_.use_cdrom = set_in; }
  void setCDROMDevice (int device_in) { configuration_.cdrom_device = device_in; }
  void setCDROMCount (int count_in) { configuration_.cdrom_count = count_in; }
  void setVolSound (float volume_in) {
    configuration_.vol_sound = volume_in;
    if (configuration_.vol_sound < 0.0)
      configuration_.vol_sound = 0.0;
    if (configuration_.vol_sound > 1.0)
      configuration_.vol_sound = 1.0;
  }
  void setVolMusic (float volume_in) {
    configuration_.vol_music = volume_in;
    if (configuration_.vol_music < 0.0)
      configuration_.vol_music = 0.0;
    if (configuration_.vol_music > 1.0)
      configuration_.vol_music = 1.0;
  }

  void setDebug (bool set_in) { configuration_.debug = set_in; }

  void setSkillBase (float skill_in) {
    configuration_.skill_base = skill_in;
    if (configuration_.skill_base > SKILL_BASE_MAX)
      configuration_.skill_base = SKILL_BASE_MAX;
    if (configuration_.skill_base < SKILL_BASE_MIN)
      configuration_.skill_base = SKILL_BASE_MIN;
  }
  int intSkill() { return (int)((configuration_.skill_base+0.05)*10.0); }

 private:
  Configuration_t configuration_;

  Splot_Configuration ();
  virtual ~Splot_Configuration ();
  ACE_UNIMPLEMENTED_FUNC (Splot_Configuration (const Splot_Configuration&));
  ACE_UNIMPLEMENTED_FUNC (Splot_Configuration& operator= (const Splot_Configuration&));

  //void readValues (FILE* file);
  bool load (const std::string&); // FQ filename
};

typedef ACE_Singleton<Splot_Configuration,
                      ACE_Thread_Mutex> SPLOT_CONFIGURATION_SINGLETON;
//SPLOT_SINGLETON_DECLARE(ACE_Singleton, Splot_Configuration, ACE_Thread_Mutex);

#endif // CONFIGURATION_H
