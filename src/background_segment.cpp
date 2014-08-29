#include "stdafx.h"

#include "background_segment.h"

//#include "Splot_BackgroundSegment.h"
//
//#include "Global.h"
//#include "Ground.h"

Splot_BackgroundSegment::Splot_BackgroundSegment (const float (&position_in)[3],
                                                  const float (&size_in)[2],
                                                  Splot_Background* parent_in)
 : //position_ ()
 //, size_ ()
 /*,*/ age_ (0.0)
 , parent_ (parent_in)
{
  position_[0] = position_in[0];
  position_[1] = position_in[1];
  position_[2] = position_in[2];

  size_[0] = size_in[0];
  size_[1] = size_in[1];
}

Splot_BackgroundSegment::~Splot_BackgroundSegment ()
{

}
