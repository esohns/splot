#include "stdafx.h"

#include "background.h"

#include "background_segment.h"

Splot_Background::Splot_Background ()
 : variation_ (0)
 //, position_ ()
 //, tex_ ()
 //, segments_ ()
{
  position_[0] = 0.0; position_[1] = 0.0; position_[2] = 0.0;
  for (int i = 0; i < MAX_BACKGROUND_TYPES; i++)
    tex_[i] = 0;
}

Splot_Background::~Splot_Background ()
{
  glDeleteTextures (MAX_BACKGROUND_TYPES, &(tex_[0]));

  for (segmentsIterator_t iterator = segments_.begin ();
       iterator != segments_.end ();
       iterator++)
    delete *iterator;
}

void
Splot_Background::nextVariation ()
{
  setVariation (variation_ + 1);
}
