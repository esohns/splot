#include "stdafx.h"

#include "background_metal.h"

//#ifdef HAVE_CONFIG_H
//#include "splot-config.h"
//#endif
//
//#include "Splot_BackgroundMetal.h"
//
//#include <cmath>
//

#include <string>

#include "ace/OS_Memory.h"

#include "defines.h"
#include "common.h"
#include "state.h"
#include "configuration.h"
#include "background_metal_segment.h"
#include "image.h"

Splot_BackgroundMetal::Splot_BackgroundMetal ()
 : inherited ()
 , size_ (21.0)
{
  loadTextures ();

  float s[2] = {size_, size_};
  Splot_BackgroundSegment* segment = NULL;
  ACE_NEW (segment, 
           Splot_BackgroundMetalSegment (inherited::position_, s, this));
  inherited::segments_.push_back (segment);

  inherited::position_[1] = size_*2.0F;
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

void
Splot_BackgroundMetal::loadTextures ()
{
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
  if (configuration.tex_border)
    glTexParameterfv (GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, tbc);
  filename = path_base + ACE_TEXT_ALWAYS_CHAR ("gndMetalBlip.png");
  tex_[BACKGROUND_BLIP] =
    Splot_Image::load (dataLoc (filename), IMG_NOMIPMAPS, IMG_ALPHA, GL_REPEAT, GL_LINEAR, GL_LINEAR);
  ACE_DEBUG ((LM_ERROR,
              ACE_TEXT ("failed to Splot_Image::load(\"%s\"), continuing\n"),
              ACE_TEXT (dataLoc (filename).c_str ())));
}

void
Splot_BackgroundMetal::deleteTextures ()
{
  glDeleteTextures (1, &inherited::tex_[BACKGROUND_BASE]);
  inherited::tex_[BACKGROUND_BASE] = 0;
  glDeleteTextures (1, &inherited::tex_[BACKGROUND_BLIP]);
  inherited::tex_[BACKGROUND_BLIP] = 0;
}

void
Splot_BackgroundMetal::setVariation (int index_in)
{
  deleteTextures ();
  inherited::variation_ = index_in%3;
  loadTextures ();
}

void
Splot_BackgroundMetal::drawGL ()
{
  State_t& state = SPLOT_STATE_SINGLETON::instance ()->get ();

  //-- Set background color for low and med gfx
  float pulse = sin (state.game_frame*0.03F);
  if (pulse < 0.0) 
    pulse = 0.0;
  glClearColor (0.2F+pulse, 0.2F, 0.25F, 1.0);

  //-- draw ground segments
  segmentsIterator_t iterator = inherited::segments_.begin ();
  if (!state.game_pause ||
      state.game_mode == GAMEMODE_MENU)
  {
    iterator++;
    while (iterator != inherited::segments_.end ())
    {
      (*iterator)->position_[1] += state.scroll_speed*state.speed_adj;
      iterator++;
    } // end WHILE
  } // end IF

  float s2 = size_*2.0F;
  float p[3] = {0.0, 0.0, 0.0};
  float s[2] = {size_, size_};
  iterator = inherited::segments_.begin ();
  iterator++;
  segmentsIterator_t first = iterator;
  Splot_BackgroundSegment* segment = NULL;
  while (iterator != inherited::segments_.end ())
  {
    (*iterator)->drawGL ();

    if ((*iterator)->position_[1] >= -s2)
    {
      iterator++;
      continue;
    } // end IF

    p[1] = (*iterator)->position_[1]+s2+s2;
    iterator = inherited::segments_.erase (iterator);

    ACE_NEW (segment,
             Splot_BackgroundMetalSegment (p, s, this));
    inherited::segments_.insert (first, segment);
    break;
  } // end WHILE
}
