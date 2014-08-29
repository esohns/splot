#ifndef DEFINES_H
#define DEFINES_H

#ifdef HAVE_CONFIG_H
#include "splot-config.h"
#endif

#define SPLOT_DATA_DIR_ENV_SYMBOL           "SPLOT_DATA"
#define SPLOT_FONT_DIR_ENV_SYMBOL           "SPLOT_FONT"

#define CONFIGURATION_DEFAULT_SCREEN_WIDTH   1024
#define CONFIGURATION_DEFAULT_SCREEN_HEIGHT  786
#define CONFIGURATION_DEFAULT_SCREEN_ASPECT  1.3333333F
#define CONFIGURATION_DEFAULT_SCREEN_FOV     30.0F
#define CONFIGURATION_DEFAULT_SCREEN_ZNEAR   10.0
#define CONFIGURATION_DEFAULT_SCREEN_ZFAR    70.0
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
#define SKILL_LEVEL_MAX_INT                  MAX_LEVEL

#define STATE_DEFAULT_FPS                    50.0

#define STATE_DEFAULT_GAME_SPEED             0.5
#define STATE_DEFAULT_GAME_SKILL             1.0
#define STATE_DEFAULT_SPEED_ADJUSTMENT       1.0
#define STATE_DEFAULT_SCROLL_SPEED           -0.045F
#define STATE_DEFAULT_TOOLKIT                TOOLKIT_SDL

#define STATE_DEFAULT_PLAYER_SHIPS           4
#define MAX_PLAYER_SHIPS                     9

#define SPLOT_IMAGE_DATA_DIR                 "png"
#define SPLOT_AUDIO_DATA_DIR                 "wav"

#if defined (USE_DYNAMIC_RANDOMNESS)
#define SRAND (2.0*(0.5-(ACE_OS::rand ()/(double)RAND_MAX)))
#define FRAND (ACE_OS::rand ()/(double)RAND_MAX)
#define IRAND (ACE_OS::rand ())
#else
#define SRAND (Splot_State::randomS[(++Splot_State::random_index)%256])
#define FRAND (Splot_State::randomF[(++Splot_State::random_index)%256])
#define IRAND (Splot_State::randomI[(++Splot_State::random_index)%256])
#endif

#define AMMO_REFILL                          150
#define HERO_Z                               25.0F

#define MIN_SCREEN_SIZE                      0 // 320x200
#define MAX_SCREEN_SIZE                      8 // 1280x1024

#define NUM_ENEMY_AMMO_TYPES                 5
#define NUM_PLAYER_AMMO_TYPES                3
#define MAX_PLAYER_ITEMS                     1

#define PLAYER_DEFAULT_DAMAGE                -500.0F
#define PLAYER_DEFAULT_SHIELDS               500.0F

#define DEATH_SPIKES                         7
#define DEATH_TIME                           50

#define SUPER_BOMB_DAMAGE                    5000.0F
#define MAX_COLLISION_DAMAGE                 35.0F

#define SCORE_STEP                           50000.0F
#define SCORE_END_OF_LEVEL                   5000.0F
#define SCORE_KILL_DEFAULT                   75.0F
#define SCORE_KILL_OMNI                      25.0F
#define SCORE_KILL_RAYGUN                    1000.0F
#define SCORE_KILL_TANK                      1500.0F
#define SCORE_KILL_GNAT                      10.0F
#define SCORE_KILL_BOSS_0                    SCORE_END_OF_LEVEL
#define SCORE_KILL_BOSS_1                    SCORE_END_OF_LEVEL
#define SCORE_POWERUP_SHIELD_SUPER           2500.0F
#define SCORE_POWERUP_SHIELD                 1000.0F
#define SCORE_POWERUP_REPAIR                 1000.0F
#define SCORE_POWERUP_AMMUNITION             100.0F

#ifdef _APPLE_
#define CONFIG_EXT                           ".cfg"
#define CONFIG_FILE                          PACKAGE".cfg"
#define CONFIG_SCORE_FILE                    PACKAGE"-score.cfg"
#define CONFIG_MUSIC_FILE                    PACKAGE"-music.cfg"
#else
#define CONFIG_EXT
#define CONFIG_FILE                          "."PACKAGE
#define CONFIG_SCORE_FILE                    "."PACKAGE"-score"
#define CONFIG_MUSIC_FILE                    "."PACKAGE"-music"
#endif // _APPLE_

#define HI_SCORE_HIST                        5

#define NUM_EXPLO                            2
#define NUM_EXPLO_POP                        6
#define MAX_MUSIC                            32

#define NUM_HELP_MESSAGE_LINES               7

#endif // DEFINES_H
