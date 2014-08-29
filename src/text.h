#ifndef TEXT_H
#define TEXT_H

class Splot_Text
{
 public:
  Splot_Text ();
  virtual ~Splot_Text ();

  virtual void Render (const char*, const int = -1) = 0;
  virtual float Advance (const char*, const int = -1) = 0;
  virtual float LineHeight (const char* = " ", const int = -1) = 0;
};

#endif // TEXT_H
