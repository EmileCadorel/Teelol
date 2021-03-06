#include "Button.hh"

Button::Button(std::string text, int x, int y, int h, int l):m_text(text), m_x(x), m_y(y), m_h(h), m_l(l){
  TTF_Init();
  m_clicked = false;
  TTF_Font * f = TTF_OpenFont("../const/Font.ttf",20);
  SDL_Color couleur = {0,0,0};
  m_form = TTF_RenderText_Blended(f, m_text.c_str(), couleur);
  m_fond = SDL_CreateRGBSurface(SDL_HWSURFACE, l, h, 32,0,0,0,0);
  SDL_FillRect(m_fond, NULL, SDL_MapRGB(m_fond->format, 153,255,255));
}


void Button::show(Ecran * e){
  SDL_Rect rect;
  rect.x = m_x;
  rect.y = m_y;
  e->put(m_fond, rect);
  rect.x = m_x + m_fond->w/2 - m_form->w/2;
  rect.y = m_y + m_fond->h/2 - m_form->h/2;
  e->put(m_form, rect);
}

void Button::pass_row(Event &e){
  if(e[LEFT_CL] && is_inside(e().m_x, e().m_y)){
    SDL_FillRect(m_fond, NULL, SDL_MapRGB(m_fond->format, 102,51,0));
    if(!m_clicked)
	clicked();
    else 
	pressed();
    m_clicked = true;
    m_hover = false;
  }
  else if(is_inside(e().m_x, e().m_y)){
    SDL_FillRect(m_fond, NULL, SDL_MapRGB(m_fond->format, 51,51,51));
    if(!m_hover)
	hover();
    m_clicked = false;
    m_hover = true;
  }
  else{
      SDL_FillRect(m_fond, NULL, SDL_MapRGB(m_fond->format, 153,255,255));
      m_clicked = false;
      m_hover = false;
  }
}

bool Button::getClicked(){
  return m_clicked;
}

bool Button::is_inside(int x, int y){
  if (x >= m_x && x <= m_x + m_l && y >= m_y && y <= m_y + m_h)
    return true;
  else
    return false;
}


int & Button::x() {
    return m_x;
}

int & Button::y() {
    return m_y;
}


int & Button::h() {
    return m_h;
}

int & Button::l() {
    return m_l;
}

Button::~Button(){
}



