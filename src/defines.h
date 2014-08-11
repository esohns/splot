#ifndef DEFINES_H
#define DEFINES_H

#ifdef HAVE_CONFIG_H
#include "splot-config.h"
#endif

#define CONFIGURATION_DEFAULT_SCREEN_W       1024
#define CONFIGURATION_DEFAULT_SCREEN_H       786
#define CONFIGURATION_DEFAULT_SCREEN_A       1.3333333F
#define CONFIGURATION_DEFAULT_SCREEN_FOV     30.0F
#define CONFIGURATION_DEFAULT_SCREEN_NEAR    10.0
#define CONFIGURATION_DEFAULT_SCREEN_FAR     70.0
#define CONFIGURATION_DEFAULT_SCREEN_BOUND_0 11.0
#define CONFIGURATION_DEFAULT_SCREEN_BOUND_1 9.0
#define CONFIGURATION_DEFAULT_Z_TRANS        -56.5

#define CONFIGURATION_DEFAULT_FULLSCREEN     false
#define CONFIGURATION_DEFAULT_BLEND_ENABLED  true
#define CONFIGURATION_DEFAULT_TRUE_COLOR     false
#define CONFIGURATION_DEFAULT_TEX_BORDER     true
#define CONFIGURATION_DEFAULT_SHOW_FPS       true
#define CONFIGURATION_DEFAULT_VIEW_GAMMA     1.1F
#define CONFIGURATION_DEFAULT_TEXT_TYPE      TEXT_GLC

#define CONFIGURATION_DEFAULT_AUTO_SPEED     false
#define CONFIGURATION_DEFAULT_MOVEMENT_SPEED 0.03F

#define CONFIGURATION_DEFAULT_AUDIO_ENABLED  true
#define CONFIGURATION_DEFAULT_SWAP_STEREO    false
#define CONFIGURATION_DEFAULT_USE_PLAYLIST   false
#define CONFIGURATION_DEFAULT_USE_CDROM      false
#define CONFIGURATION_DEFAULT_VOL_SOUND      0.9F
#define CONFIGURATION_DEFAULT_VOL_MUSIC      0.5
#define CONFIGURATION_DEFAULT_AUDIO_TYPE     AUDIO_OPENAL

#define CONFIGURATION_DEFAULT_DEBUG          false

#define CONFIGURATION_DEFAULT_SKILL_BASE     0.5

#define CONFIGURATION_INI_FILE_ROOT_SECTION  PACKAGE

#define OPTIONS_LONG_OPTION_NOAUDIO          "noaudio"
#define OPTIONS_LONG_OPTION_NOBLEND          "noblend"
#define OPTIONS_LONG_OPTION_FULLSCREEN       "fullscreen"
#define OPTIONS_LONG_OPTION_VIDEO_MODE       "vidmode"
#define OPTIONS_LONG_OPTION_NOTEXBORDER      "notexborder"
#define OPTIONS_LONG_OPTION_VERSION          "version"
#define OPTIONS_LONG_OPTION_WINDOWED         "window"

#define MAX_LEVEL                            10

#define MAX_PLAYER_NAME_LENGTH               100

#define MOVEMENT_SPEED_MIN                   0.01F
#define MOVEMENT_SPEED_MAX                   0.1F

#define SKILL_BASE_MIN                       0.2F
#define SKILL_BASE_MAX                       0.9F
#define SKILL_LEVEL_INCREMENT                0.05
#define SKILL_MAX                            1.9

#define STATE_DEFAULT_FPS                    50.0

#define STATE_DEFAULT_GAME_SPEED             0.5
#define STATE_DEFAULT_GAME_SKILL             1.0
#define STATE_DEFAULT_SPEED_ADJUSTMENT       1.0
#define STATE_DEFAULT_SCROLL_SPEED           -0.045F
#define STATE_DEFAULT_TOOLKIT                TOOLKIT_SDL

#define STATE_DEFAULT_HERO_LIVES             4

//#define SRAND (2.0*(0.5-(rand()/(double)RAND_MAX)))
//#define FRAND (rand()/(double)RAND_MAX)
//#define IRAND (rand())
//#define SRAND (Global::randS[(++Global::randIndex)%256])
//#define FRAND (Global::randF[(++Global::randIndex)%256])
//#define IRAND (Global::randI[(++Global::randIndex)%256])

#define AMMO_REFILL 150
#define HERO_Z      25.0f

#define MIN_SCREEN_SIZE 0 // 320x200
#define MAX_SCREEN_SIZE 8 // 1280x1024

#define NUM_ENEMY_AMMO_TYPES 5
#define NUM_HERO_AMMO_TYPES 3
#define NUM_HERO_ITEMS 1

#define HERO_DAMAGE -500.0
#define HERO_SHIELDS 500.0 

#ifdef macintosh
#define CONFIG_EXT        ".cfg"
#define CONFIG_FILE       PACKAGE".cfg"
#define CONFIG_SCORE_FILE PACKAGE"-score.cfg"
#define CONFIG_MUSIC_FILE PACKAGE"-music.cfg"
#else
#define CONFIG_EXT
#define CONFIG_FILE       "."PACKAGE
#define CONFIG_SCORE_FILE "."PACKAGE"-score"
#define CONFIG_MUSIC_FILE "."PACKAGE"-music"
#endif // macintosh

#define HI_SCORE_HIST 5

#endif // DEFINES_H
