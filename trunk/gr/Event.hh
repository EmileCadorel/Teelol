#ifndef DEF_INPUT
#define DEF_INPUT

#include <boost/signals2.hpp>
#include <SDL/SDL.h>

using boost::signals2::signal;


struct Input{
  char m_key[SDLK_LAST];
  int m_mouse_x, m_mouse_y;
  int m_mouse_xrel, m_mouse_yrel;
  char m_mousebuttons[8];
};


enum Mapp{
    LEFT, RIGHT, JUMP, QUIT , TAB , MAP_INTER, LEFT_CL , RIGHT_CL,  MAP_LAST
};

struct position_t{
  int m_x, m_y;
};





class Event {
public: 

  Event();

  void change_key(Mapp m, SDLKey k);
  void UpdateEvent();
  bool key_pressed(Mapp m);
  void reset_pressed(Mapp m);
  int & WheelChange();
  SDL_Event & getEvent();
  char& operator[](Mapp m);
  position_t operator()();



    signal<void()> key_left;
    signal<void()> key_right;
    signal<void()> key_jump;
    signal<void()> key_quit;
    signal<void()> key_tab;
    signal<void()> left_click;
    signal<void()> right_click;
    signal<void(int)> wheel;
    signal<void(int, int)> mouse_motion;

private:

    void launch_key_sig();
    void launch_mouse_sig();

  Input m_in;
  SDLKey m_mapping[MAP_INTER];
  int m_mouse_mapping[MAP_LAST];
  SDL_Event m_event;
  int m_wheel;
  int m_sensivity;
};


#endif
