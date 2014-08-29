#ifndef BACKGROUNDSEGMENT_H
#define BACKGROUNDSEGMENT_H

class Splot_Background;

class Splot_BackgroundSegment
{
  friend class Splot_Background;

 public:
  Splot_BackgroundSegment (const float (&)[3], // position
                           const float (&)[2], // size
                           Splot_Background*); // parent
  virtual ~Splot_BackgroundSegment ();

  virtual void drawGL () = 0;

  float position_[3];
  float size_[2];
  float age_;

  Splot_Background* parent_;
};

#endif // BACKGROUNDSEGMENT_H
