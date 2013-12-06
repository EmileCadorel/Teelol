#ifndef _Form
#define _Form

#include <SDL/SDL.h>
#include <SDL_SDL_image.h>

class Form{
public :
  //prend en entree un chemin vers une image
  void set_image(string img);
  int get_l();
  int get_h();
  int get_x();
  int get_y();

  ~Form();

private:

  SDL_Surface * m_surf;
  int m_x;
  int m_y;
  int m_h; 
  int m_l;
}



#endif