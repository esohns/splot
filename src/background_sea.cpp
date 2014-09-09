#include "stdafx.h"

#include "background_sea.h"

#include <cmath>

#include <string>

#include "ace/OS.h"
#include "ace/OS_Memory.h"

#include "defines.h"
#include "common.h"
#include "state.h"
#include "configuration.h"
#include "background_sea_segment.h"
#include "image.h"

// init statics
GLuint Splot_BackgroundSea::texBase = 0;
//float Splot_BackgroundSea::vert[4][3] = {{ 20.5,  15.5, 0.0},
//                                         {-20.5,  15.5, 0.0},
//                                         { 20.5, -15.5, 0.0},
//                                         {-20.5, -15.5, 0.0}};

Splot_BackgroundSea::Splot_BackgroundSea ()
 : inherited (BACKGROUND_SEA)
 , size_ (21.0)
{
  loadTextures ();

  float s[2] = {size_, size_};
  inherited::position_[1] = size_*2.0F;
  Splot_BackgroundSegment* segment = NULL;
  ACE_NEW (segment,
           Splot_BackgroundSeaSegment (inherited::position_, s, this));
  inherited::segments_.push_back (segment);

  inherited::position_[1] = 0.0;
  ACE_NEW (segment,
           Splot_BackgroundSeaSegment (inherited::position_, s, this));
  inherited::segments_.push_back (segment);
}

Splot_BackgroundSea::~Splot_BackgroundSea ()
{
  deleteTextures ();
}

bool
Splot_BackgroundSea::init ()
{
  std::string path_base = ACE_TEXT_ALWAYS_CHAR (SPLOT_IMAGE_DATA_DIR);
  path_base += ACE_DIRECTORY_SEPARATOR_STR;
  char buffer[PATH_MAX];
  ACE_OS::memset (buffer, 0, sizeof (buffer));
  if (ACE_OS::sprintf (buffer, ACE_TEXT_ALWAYS_CHAR ("gndBaseSea.png")) < 0)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to sprintf(): \"%m\", aborting\n")));

    return false;
  } // end IF
  std::string filename = dataLoc (path_base+buffer);
  texBase =
    Splot_Image::load (filename, IMG_NOMIPMAPS, IMG_ALPHA, GL_CLAMP, GL_LINEAR, GL_LINEAR);
  if (!Splot_BackgroundSea::texBase)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to Splot_Image::load(\"%s\"), aborting\n"),
                ACE_TEXT (filename.c_str ())));

    return false;
  } // end IF

  return true;
}

void
Splot_BackgroundSea::fini ()
{
  glDeleteTextures (1, &texBase);
}

void
Splot_BackgroundSea::loadTextures ()
{
#if defined (DEBUG_LAZY_LOADING) && (DEBUG_LAZY_LOADING == 1)
  std::string path_base = ACE_TEXT_ALWAYS_CHAR (SPLOT_IMAGE_DATA_DIR);
  path_base += ACE_DIRECTORY_SEPARATOR_STR;
  char buffer[PATH_MAX];
  ACE_OS::memset (buffer, 0, sizeof (buffer));
  if (ACE_OS::sprintf (buffer, ACE_TEXT_ALWAYS_CHAR ("gndBaseSea.png")) < 0)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to sprintf(): \"%m\", returning\n")));

    return;
  } // end IF
  std::string filename = dataLoc (path_base+buffer);
  texBase =
    Splot_Image::load (filename, IMG_NOMIPMAPS, IMG_ALPHA, GL_CLAMP, GL_LINEAR, GL_LINEAR);
  if (!Splot_BackgroundSea::texBase)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to Splot_Image::load(\"%s\"), returning\n"),
                ACE_TEXT (filename.c_str ())));

    return;
  } // end IF
#else
  inherited::tex_[BACKGROUNDTEXTURE_BASE] = Splot_BackgroundSea::texBase;
#endif
}

void
Splot_BackgroundSea::deleteTextures ()
{
#if defined (DEBUG_LAZY_LOADING) && (DEBUG_LAZY_LOADING == 1)
  glDeleteTextures (1, &inherited::tex_[BACKGROUNDTEXTURE_BASE]);
  inherited::tex_[BACKGROUNDTEXTURE_BASE] = 0;
#endif
}

void
Splot_BackgroundSea::setVariation (int index_in)
{
  ACE_UNUSED_ARG (index_in);
}

void
Splot_BackgroundSea::drawGL ()
{
  State_t& state = SPLOT_STATE_SINGLETON::instance ()->get ();

  glClearColor (0.15F, 0.12F, 0.1F, 1.0F);
  //	glClearColor(0.27451, 0.235294, 0.392157, 1.0); // web page background color

  //-- draw ground segments

  segmentsIterator_t iterator = inherited::segments_.begin ();
  if (!state.game_pause ||
      state.game_mode == GAMEMODE_MENU)
  {
    while (iterator != inherited::segments_.end ())
    {
      (*iterator)->position_[1] += state.scroll_speed;
      iterator++;
    } // end WHILE
  } // end IF

  glColor4f (0.9F, 0.9F, 0.9F, 0.7F);
  float	s2 = size_*2.0F;
  float position[3] = {0.0, s2, 0.0};
  float s[2] = {size_, size_};
  Splot_BackgroundSeaSegment* segment = NULL;
  iterator = inherited::segments_.begin ();
  while (iterator != inherited::segments_.end ())
  {
    (*iterator)->drawGL ();

    (*iterator)->position_[1] += state.scroll_speed;
    if ((*iterator)->position_[1] >= -s2)
    {
      iterator++;
      continue;
    } // end IF

    delete *iterator;
    iterator = inherited::segments_.erase (iterator);

    segment = NULL;
    ACE_NEW_NORETURN (segment,
                      Splot_BackgroundSeaSegment (position, s, this));
    if (!segment)
      ACE_DEBUG ((LM_CRITICAL,
                  ACE_TEXT ("failed to allocate memory, continuing\n")));
    else
      inherited::segments_.push_front (segment);
    break;
  } // end WHILE
}
