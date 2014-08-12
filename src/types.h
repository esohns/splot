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
  MAX_MODE_TYPES
};

enum GameMode_t
{
  GAMEMODE_GAME_INVALID = -1,
  GAMEMODE_GAME = 0,
  GAMEMODE_GAME_OVER,
  GAMEMODE_LEVEL_END,
  GAMEMODE_MENU,
  ///////////////////////////////////////
  MAX_GAMEMODE_TYPES
};

#include "stdio.h"

// forward declarations
class Splot_PlayerAircraft;
class Splot_PlayerBullets;
class Splot_Enemies;
class EnemyAmmo;
class Explosions;
class Splot_PowerUps;
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

  Splot_PlayerAircraft* player;
  Splot_Enemies*        enemies;
  Splot_PlayerBullets*  player_bullets;
  EnemyAmmo*            enemy_ammo;
  ScreenItemAdd*        item_add;

  Explosions*           explosions;
  Splot_PowerUps*       power_ups;
  Audio*                audio;
  Ground*               ground;
  Ground*               ground_game;
  Ground*               ground_menu;
  MenuGL*               menu;
  MainGL*               main_GL;
  StatusDisplay*        status_display;

  float            cursor_pos[3];

  // *TODO*: save randomness/state with static savegames
  // *TODO*: keep randomness/state here to support re-entrant code
  //int              randomI[256];
  //float            randomF[256];
  //float            randomS[256];
  //int              random_index;

  FILE*            event_file;

  int              tip_ship_past;
  int              tip_super_shield;
};

#include "defines.h"

struct GameState_t
{
  float damage;
  float shields;

  float score;
  float score_step;
  float score_target;

  float death_stereo;

  int ships;
  int super_bomb_exploding;

  float ammo_stock[NUM_PLAYER_AMMO_TYPES];
  float gun_pause[NUM_PLAYER_AMMO_TYPES];
  bool gun_swap;
  bool gun_trigger;

  int current_item_index;
  float use_item_armed;
};

enum GameToolkit_t
{
  TOOLKIT_INVALID = -1,
#if defined (USE_GLUT)
  TOOLKIT_GLUT = 0,
#elif defined (USE_SDL)
  TOOLKIT_SDL,
#else
#error "USE_SDL or USE_GLUT must be defined"
#endif
  ///////////////////////////////////////
  TOOLKIT_MAX
};

struct bullet_t
{
  float position[3];
  float translation_step[3];
  float damage;

  static int count;
};

#include <vector>

typedef std::vector<bullet_t> bullets_t;
typedef bullets_t::iterator bullets_iterator_t;
typedef bullets_t::const_iterator bullets_const_iterator_t;

#endif // TYPES_H
