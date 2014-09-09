#include "stdafx.h"

#include "background_metal.h"

#include <cmath>

#include <string>

#include "ace/OS.h"
#include "ace/OS_Memory.h"

#include "defines.h"
#include "common.h"
#include "state.h"
#include "configuration.h"
#include "background_metal_segment.h"
#include "image.h"

// init statics
GLuint Splot_BackgroundMetal::texBase[MAX_METALBACKGROUNDVARIANT_TYPES];
GLuint Splot_BackgroundMetal::texBlip = 0;

Splot_BackgroundMetal::Splot_BackgroundMetal ()
 : inherited (BACKGROUND_METAL)
 , size_ (21.0)
{
  loadTextures ();

  float s[2] = {size_, size_};
  inherited::position_[1] = size_*2.0F;
  Splot_BackgroundSegment* segment = NULL;
  ACE_NEW (segment,
           Splot_BackgroundMetalSegment (inherited::position_, s, this));
  inherited::segments_.push_back (segment);

  inherited::position_[1] = 0.0;
  ACE_NEW (segment,
           Splot_BackgroundMetalSegment (inherited::position_, s, this));
  inherited::segments_.push_back (segment);
}

Splot_BackgroundMetal::~Splot_BackgroundMetal ()
{
  deleteTextures ();
}

bool
Splot_BackgroundMetal::init ()
{
#ifdef GL_CLAMP_TO_EDGE
  GLenum clamp = GL_CLAMP_TO_EDGE;
#else
  GLenum clamp = GL_REPEAT;
#endif

  // load base textures
  ACE_OS::memset (&texBase, 0, sizeof (texBase));
  std::string path_base = ACE_TEXT_ALWAYS_CHAR (SPLOT_IMAGE_DATA_DIR);
  path_base += ACE_DIRECTORY_SEPARATOR_STR;
  char buffer[PATH_MAX];
  ACE_OS::memset (buffer, 0, sizeof (buffer));
  std::string filename;
  for (int i = 0; i < MAX_METALBACKGROUNDVARIANT_TYPES; i++)
  {
    if (ACE_OS::sprintf (buffer, ACE_TEXT_ALWAYS_CHAR ("gndMetalBase0%d.png"),
                         i) < 0)
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to sprintf(): \"%m\", aborting\n")));

      return false;
    } // end IF
    filename = dataLoc (path_base+buffer);
    texBase[i] =
      Splot_Image::load (filename, IMG_NOMIPMAPS, IMG_ALPHA, clamp, GL_LINEAR, GL_LINEAR);
    if (!Splot_BackgroundMetal::texBase[i])
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to Splot_Image::load(\"%s\"), aborting\n"),
                  ACE_TEXT (filename.c_str ())));

      return false;
    } // end IF
  } // end FOR

  // load blip texture(s)
  if (ACE_OS::sprintf (buffer, ACE_TEXT_ALWAYS_CHAR ("gndMetalBlip.png")) < 0)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to sprintf(): \"%m\", aborting\n")));

    return false;
  } // end IF
  filename = dataLoc (path_base+buffer);
  texBlip =
    Splot_Image::load (filename, IMG_NOMIPMAPS, IMG_ALPHA, GL_REPEAT, GL_LINEAR, GL_LINEAR);
  if (!texBlip)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to Splot_Image::load(\"%s\"), aborting\n"),
                ACE_TEXT (filename.c_str ())));

    return false;
  } // end IF

  return true;
}

void
Splot_BackgroundMetal::fini ()
{
  glDeleteTextures (MAX_METALBACKGROUNDVARIANT_TYPES, &(texBase[0]));
  glDeleteTextures (1, &texBlip);
}

void
Splot_BackgroundMetal::loadTextures ()
{
#if defined (DEBUG_LAZY_LOADING) && (DEBUG_LAZY_LOADING == 1)
#ifdef GL_CLAMP_TO_EDGE
  GLenum clamp = GL_CLAMP_TO_EDGE;
#else
  GLenum clamp = GL_REPEAT;
#endif
  std::string path_base = ACE_TEXT_ALWAYS_CHAR (SPLOT_IMAGE_DATA_DIR);
  path_base += ACE_DIRECTORY_SEPARATOR_STR;
  char buffer[PATH_MAX];
  ACE_OS::sprintf (buffer, ACE_TEXT_ALWAYS_CHAR ("gndMetalBase0%d.png"),
                   inherited::variation_);
  std::string filename = path_base + buffer;
  inherited::tex_[BACKGROUND_BASE] =
    Splot_Image::load (dataLoc (filename), IMG_NOMIPMAPS, IMG_ALPHA, clamp, GL_LINEAR, GL_LINEAR);
  if (!inherited::tex_[BACKGROUND_BASE])
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to Splot_Image::load(\"%s\"), continuing\n"),
                ACE_TEXT (dataLoc (filename).c_str ())));

  const Configuration_t& configuration =
    SPLOT_CONFIGURATION_SINGLETON::instance ()->get ();
  float tbc[4] = {0.2F, 0.2F, 0.2F, 1.0};
  if (configuration.texture_border)
    glTexParameterfv (GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, tbc);
  filename = path_base + ACE_TEXT_ALWAYS_CHAR ("gndMetalBlip.png");
  tex_[BACKGROUND_BLIP] =
    Splot_Image::load (dataLoc (filename), IMG_NOMIPMAPS, IMG_ALPHA, GL_REPEAT, GL_LINEAR, GL_LINEAR);
  if (!tex_[BACKGROUND_BLIP])
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to Splot_Image::load(\"%s\"), continuing\n"),
                ACE_TEXT (dataLoc (filename).c_str ())));
#else
  inherited::tex_[BACKGROUNDTEXTURE_BASE] = Splot_BackgroundMetal::texBase[inherited::variation_];
  inherited::tex_[BACKGROUNDTEXTURE_BLIP] = Splot_BackgroundMetal::texBlip;
#endif
}

void
Splot_BackgroundMetal::deleteTextures ()
{
#if defined (DEBUG_LAZY_LOADING) && (DEBUG_LAZY_LOADING == 1)
  glDeleteTextures (1, &inherited::tex_[BACKGROUND_BASE]);
  inherited::tex_[BACKGROUND_BASE] = 0;
  glDeleteTextures (1, &inherited::tex_[BACKGROUND_BLIP]);
  inherited::tex_[BACKGROUND_BLIP] = 0;
#endif
}

void
Splot_BackgroundMetal::setVariation (int index_in)
{
  deleteTextures ();
  inherited::variation_ = index_in%MAX_METALBACKGROUNDVARIANT_TYPES;
  loadTextures ();
}

void
Splot_BackgroundMetal::drawGL ()
{
  State_t& state = SPLOT_STATE_SINGLETON::instance ()->get ();

  //-- set background color for low/med gfx
  float pulse = ::sin (state.game_frame*0.03F);
  if (pulse < 0.0) 
    pulse = 0.0;
  glClearColor (0.2F+pulse, 0.2F, 0.25F, 1.0);

  //-- draw ground segments

  segmentsIterator_t iterator = inherited::segments_.begin ();
  if (!state.game_pause ||
      state.game_mode == GAMEMODE_MENU)
  {
    while (iterator != inherited::segments_.end ())
    {
      (*iterator)->position_[1] += state.scroll_speed*state.speed_adj;
      iterator++;
    } // end WHILE
  } // end IF

  float s2 = size_*2.0F;
  float position[3] = {0.0, 0.0, 0.0};
  float s[2] = {size_, size_};
  Splot_BackgroundSegment* segment = NULL;
  iterator = inherited::segments_.begin ();
  while (iterator != inherited::segments_.end ())
  {
    (*iterator)->drawGL ();

    if ((*iterator)->position_[1] >= -s2)
    {
      iterator++;
      continue;
    } // end IF

    position[1] = (*iterator)->position_[1]+s2+s2;
    delete *iterator;
    iterator = inherited::segments_.erase (iterator);

    segment = NULL;
    ACE_NEW_NORETURN (segment,
                      Splot_BackgroundMetalSegment (position, s, this));
    if (!segment)
      ACE_DEBUG ((LM_CRITICAL,
                  ACE_TEXT ("failed to allocate memory, continuing\n")));
    else
      inherited::segments_.push_front (segment);
    break;
  } // end WHILE
}
