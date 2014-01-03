#include "Rotable.hh"



void Rotable::set_angle(double angle){
  if( angle > 0)
    m_angle = angle;
  else 
    m_angle = angle+360;
}

void Rotable::set_image(Image_t img){
  m_surf = IMG_Load(c[img].c_str());
  m_sauv = IMG_Load(c[img].c_str());
  if(img == 8)
    m_sauv2 = IMG_Load(c[(Image_t)9].c_str());    
  if(img == 10)
    m_sauv2 = IMG_Load(c[(Image_t)11].c_str());    
  m_l = m_surf->w;
  m_h = m_surf->h;

}






void Rotable::rotate(double angle, int x, int y, int dist){
  m_angle += angle;
  SDL_Surface *s;
  if((m_angle < 270) && (m_angle >= 90))
    s = rotozoomSurface(m_sauv2 , -m_angle, 1.0, 1);
  else 
    s = rotozoomSurface(m_sauv, -m_angle, 1.0,1);
  m_surf = s;
  /*  m_x = x + dist * cos(m_angle*M_PI/180);
  m_y = y + dist * sin(m_angle*M_PI/180);
  */
  m_dist = dist;
  O.x = x;
  O.y = y;
  get_A();
  get_B();
  get_F();
  get_D();
  get_E();
  get_C();
  
  m_x = C.x;
  m_y = C.y;
  
}


void Rotable::zoom(double d){

 
}



void Rotable::recalibrate(){

  if(m_angle > 175 && m_angle < 185){
    m_x -= m_surf->w;
    m_y -= m_surf->h/2;
   
  }
  if(m_angle > 265 && m_angle < 275){
    m_x -= m_surf->w/2;
    m_y -= m_surf->h;
  }
  if((m_angle < 5 && m_angle > 0) || (m_angle > 355 && m_angle < 360)){
    m_y -= m_surf->h/2;
  }
  if(m_angle > 80 && m_angle < 95){
    m_x -= m_surf->w/2;
  }
  if(m_angle >= 95 && m_angle <= 175){
    m_x -= m_surf->w;
  }
  if(m_angle >= 185 && m_angle <= 265){
    m_x -= m_surf->w;
    m_y -= m_surf->h;
  }
  if(m_angle >= 275 && m_angle <= 355){
    m_y -= m_surf->h;
  }
    
    
}


int Rotable::get_xb(){

  if(m_angle > 175 && m_angle < 185){
    return m_x;
  }
  if(m_angle > 265 && m_angle < 275){
    return  m_x;
  }
  if((m_angle < 5 && m_angle > 0) || (m_angle > 355 && m_angle < 360)){
    return m_x+m_surf->w;
  }
  if(m_angle > 80 && m_angle < 95){
    return m_x + m_surf->w/2;
  }
  if(m_angle >= 95 && m_angle <= 175){
    return m_x;
  }
  if(m_angle >= 185 && m_angle <= 265){
    return m_x;

  }
  if(m_angle >= 275 && m_angle <= 355){
    return m_x+m_surf->w;
  }
  return m_x+m_surf->w;


}

int Rotable::get_yb(){
  
  if(m_angle > 175 && m_angle < 185){
    return m_y + m_surf->h/2;
  }
  if(m_angle > 265 && m_angle < 275){
    return  m_y;
  }
  if((m_angle < 5 && m_angle > 0) || (m_angle > 355 && m_angle < 360)){
    return m_y + m_surf->h/2;
  }
  if(m_angle > 80 && m_angle < 95){
    return m_y + m_surf->h;
  }
  if(m_angle >= 95 && m_angle <= 175){
    return m_y+m_surf->h;
  }
  if(m_angle >= 185 && m_angle <= 265){
    return  m_y;
  }
  if(m_angle >= 275 && m_angle <= 355){
    return m_y;
  }
  return m_y + m_surf->h;
  

}

int Rotable::get_yba(){
  if(m_angle > 175 && m_angle < 185){
    return m_y + m_surf->h/2;
  }
  if(m_angle > 265 && m_angle < 275){
    return  m_y - 5;
  }
  if((m_angle < 5 && m_angle > 0) || (m_angle > 355 && m_angle < 360)){
    return m_y + m_surf->h/2;
  }
  if(m_angle > 80 && m_angle < 95){
    return m_y + m_surf->h + 5;
  }
  if(m_angle >= 95 && m_angle <= 175){
    return m_y+m_surf->h+5;
  }
  if(m_angle >= 185 && m_angle <= 265){
    return  m_y - 5;
  }
  if(m_angle >= 275 && m_angle <= 355){
    return m_y - 5;
  }
  return m_y + m_surf->h + 5;
  
}

int Rotable::get_xba(){
  
  
  if(m_angle > 175 && m_angle < 185){
    return m_x - 5;
  }
  if(m_angle > 265 && m_angle < 275){
    return  m_x;
  }
  if((m_angle < 5 && m_angle > 0) || (m_angle > 355 && m_angle < 360)){
    return m_x+m_surf->w + 5;
  }
  if(m_angle > 80 && m_angle < 95){
    return m_x + m_surf->w/2;
  }
  if(m_angle >= 95 && m_angle <= 175){
    return m_x - 5;
  }
  if(m_angle >= 185 && m_angle <= 265){
    return m_x - 5;

  }
  if(m_angle >= 275 && m_angle <= 355){
    return m_x+m_surf->w + 5;
  }
  return m_x+m_surf->w + 5;

}

double Rotable::get_angle(){return m_angle;}

void Rotable::rotate_to(double angle, int x, int y, int dist){
  double _angle = m_angle - angle;
  SDL_Surface * s = rotozoomSurface(m_sauv, -_angle, 1.0,1);
  m_surf = s;
  m_x = x + dist * cos(_angle * M_PI/180);
  m_y = y + dist * sin(_angle*M_PI/180);

}




void Rotable::get_A(){

  A.x = m_dist*cos(m_angle*M_PI / 180) + O.x;
  A.y = m_dist*(sin(m_angle*M_PI/180))  + O.y;
  cout << (-sin(m_angle*M_PI/180))<< endl;
  cout << cos(m_angle*M_PI / 180) << endl;
  cout << m_angle << endl;


}

void Rotable::get_B(){
  B.x = A.x + (m_sauv->h/2)*(sin(m_angle*M_PI/180));
  B.y = A.y -  (m_sauv->h/2)*cos(m_angle*M_PI/180);

}

void Rotable::get_F(){
  F.x = A.x - (m_sauv->h/2)*sin(m_angle*M_PI/180);
  F.y = A.y + (m_sauv->h/2)*cos(m_angle*M_PI/180);
}

void Rotable::get_D(){
  D.x = B.x + (m_sauv->w)*cos(m_angle*M_PI/180);
  D.y = B.y + (m_sauv->w)*sin(m_angle*M_PI/180);

}

void Rotable::get_E(){
  E.x = D.x + (m_sauv->h)*(-sin(m_angle*M_PI/180));
  E.y = D.y +  (m_sauv->h)*cos(m_angle*M_PI/180);

}


void Rotable::get_C(){
  if(m_angle > 270 && m_angle <=360){
    C.y = D.y;
    C.x = B.x;
  }
  if(m_angle <= 270 && m_angle > 180){
    C.x = D.x;
    C.y = E.y;
  }
  if(m_angle <=180 && m_angle > 90){
    C.x = E.x;
    C.y = F.y;
  }
  if(m_angle > 0 && m_angle <= 90){
    C.x = F.x;
    C.y = B.y;
  }

}
