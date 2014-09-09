#ifndef BACKGROUNDSEASEGMENT_H
#define BACKGROUNDSEASEGMENT_H

#include "background_segment.h"

// forward declarations
class Splot_Background;

class Splot_BackgroundSeaSegment
 : public Splot_BackgroundSegment
{
 public:
  Splot_BackgroundSeaSegment (const float (&)[3], // position
                              const float (&)[2], // size
                              Splot_Background*); // parent
  virtual ~Splot_BackgroundSeaSegment ();

	virtual void	drawGL ();

 private:
  typedef Splot_BackgroundSegment inherited;

  float vert_[4][3];
};

#endif // BACKGROUNDSEASEGMENT_H
