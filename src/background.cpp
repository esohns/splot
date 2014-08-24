#include "stdafx.h"

#include "background.h"

Splot_Background::Splot_Background ()
 : variation_ (0)
 , rootSeg_ (NULL)
{
  position_[0] = 0.0; position_[1] = 0.0; position_[2] = 0.0;
}

Splot_Background::~Splot_Background ()
{

}

void
Splot_Background::nextVariation ()
{
  setVariation (variation_ + 1);
}
