#ifndef TYPES_H
#define TYPES_H

enum AudioType_t
{
 AUDIO_INVALID = -1,
 AUDIO_OPENAL = 0,
 AUDIO_SDL_MIXER,
 ////////////////////////////////////////
 AUDIO_MAX_TYPES
};

enum TextType_t
{
 TEXT_INVALD = -1,
 TEXT_GLC = 0,
 TEXT_FTGL,
 ////////////////////////////////////////
 TEXT_MAX_TYPES
};

struct Configuration_t
{
  int    screen_w;
  int    screen_h;
  float  screen_a;
  float  screen_FOV;
  float  screen_near;
  float  screen_far;
  float  screen_bound[2];
  float  z_trans;

  bool   full_screen;
  bool   blend_enabled;
  int    gfx_level;
  bool   true_color;
  bool   tex_border;
  bool   show_fps;
  float  view_gamma;
  TextType_t text_type;

  bool   auto_speed;
  float  movement_speed;

  bool   audio_enabled;
  bool   swap_stereo;
  bool   use_playList;
  bool   use_cdrom;
  int    cdrom_device;
  int    cdrom_count;
  float  vol_sound;
  float  vol_music;
  AudioType_t audio_type;

  bool   debug;

  float  skill_base;
};

enum Mode_t
{
  MODE_INVALID = -1,
  MODE_SHOW_VERSION = 0,
  MODE_GAME,
  ///////////////////////////////////////
  MODE_MAX
};

enum GameMode_t
{
  GAMEMODE_GAME_INVALID = -1,
  GAMEMODE_GAME = 0,
  GAMEMODE_MENU,
  GAMEMODE_LEVELOVER,
  GAMEMODE_HERODEAD,
  ///////////////////////////////////////
  GAMEMODE_MAX
};

#include "stdio.h"

// forward declarations
class HeroAircraft;
class HeroAmmo;
class EnemyFleet;
class EnemyAmmo;
class Explosions;
class PowerUps;
class Audio;
class Ground;
class MenuGL;
class MainGL;
class StatusDisplay;
class ScreenItemAdd;
class Text;
class MainToolkit;
class Splot_HighScore;

struct State_t
{
  Splot_HighScore* highscore;
  MainToolkit*     toolkit;

  bool             mouse_active;

  Text*            text;

  float            fps;
  int              frame;
  int              game_frame;
  float            game_speed;
  float            game_skill;
  int              game_level;
  int              max_level;
  float            speed_adj;

  float            scroll_speed;

  bool             game_pause;
  bool             game_quit;

  int              hero_death;
  int              hero_success;

  GameMode_t       game_mode;

  HeroAircraft*    hero;
  EnemyFleet*      enemy_fleet;
  HeroAmmo*        hero_ammo;
  EnemyAmmo*       enemy_ammo;
  ScreenItemAdd*   item_add;

  Explosions*      explosions;
  PowerUps*        power_ups;
  Audio*           audio;
  Ground*          ground;
  Ground*          ground_game;
  Ground*          ground_menu;
  MenuGL*          menu;
  MainGL*          main_GL;
  StatusDisplay*   status_display;

  float            cursor_pos[3];

  int              randI[256];
  float            randF[256];
  float            randS[256];
  int              rand_index;

  FILE*            event_file;

  int              tip_ship_past;
  int              tip_super_shield;
};

struct GameState_t
{
  float	damage;
  float	shields;
  float	score;
};

enum GameToolkit_t
{
  TOOLKIT_INVALID = -1,
  TOOLKIT_GLUT = 0,
  TOOLKIT_SDL,
  ///////////////////////////////////////
  TOOLKIT_MAX
};

#endif // TYPES_H
