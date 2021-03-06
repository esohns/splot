#ifndef BACKGROUNDMETALSEGMENT_H
#define BACKGROUNDMETALSEGMENT_H

#include "background_segment.h"

//class Ground;
//class GroundMetal;
//
////#define EXPERIMENTAL

class Splot_BackgroundMetalSegment
 : public Splot_BackgroundSegment
{
  friend class Splot_Background;
  friend class Splot_BackgroundMetal;

 public:
  Splot_BackgroundMetalSegment (const float (&)[3], // position
                                const float (&)[2], // size
                                Splot_Background*); // parent
  virtual ~Splot_BackgroundMetalSegment ();

  virtual void drawGL ();

  void drawBlip (float rep, float S, float tilt, bool blipMirrorT);
  void drawSurface (float* c0_clr,
                    float* c1_clr,
                    float* r0_clr,
                    float* r1_clr,
                    float* r2_clr);

#ifdef EXPERIMENTAL
  void drawMultiTex (float rep, float S, float tilt, bool blipMirrorT,
                     float* c0_clr,
                     float* c1_clr,
                     float* r0_clr,
                     float* r1_clr,
                     float* r2_clr);
#endif // EXPERIMENTAL

 private:
  typedef Splot_BackgroundSegment inherited;

  float c0_clr_[4];
  float c1_clr_[4];
  float r0_clr_[4];
  float r1_clr_[4];
  float r2_clr_[4];
};

#endif // BACKGROUNDMETALSEGMENT_H
