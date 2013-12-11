#include "Character.hh"
#include "Collision.hh"

Character::Character(string img, int x, int y, int l, int h, Ecran * e) : Form(x, y, h, l) {
  m_x = x;
  m_y = y; 
  m_l = l; 
  m_h = h;
  m_e = e;
  set_image(img);
  m_img = img;
  m_wrong_img = false;
  m_ground = true;
  m_speed.m_x = 0;
  m_speed.m_y = 0;
}

void Character::move_left(){
  m_speed.m_x = -5; 
}

void Character::move_right(){
  m_speed.m_x = 5;
}

void Character::stop_x(){
  m_speed.m_x = 0;
}

void Character::stop_y(){
  m_speed.m_y = 0;
  m_ground = true;
}

void Character::jump(){
  m_ground = false;
  m_speed.m_y = -5; 
}

void Character::take_dmg(){
  set_image("Dmg"+m_img);
  m_wait = 10;//attend 10 tour avant de remettre l'image à la base 
  m_wrong_img = true;
}

void Character::take_life(){
  set_image("Health"+m_img);
  m_wait = 10;
  m_wrong_img = true;
}

void Character::pass_row(){
  cout<<"ici :"<<m_x<<" "<<m_y<<" "<<m_speed.m_x<<" "<< m_speed.m_y<<endl;
  /*
    if(m_wait > 0)
    m_wait--;
    if(m_wait == 0 and m_wrong_img){
    set_image(m_img);
    m_wrong_img = false;
    }*/
  m_x += m_speed.m_x;
  m_ground = (collide() == SOUTH);
  
  direction_t d = collide();
  switch(d) {
    case NORTH:
      cout<< "north"<<endl;
      break;
    case SOUTH:
      cout<<"south"<<endl;
      break;
    case EAST:
      cout << "east" << endl;
      break;
    case WEST:
      cout << "west" << endl;
      break;
  }

  if(!m_ground){
    cout<<"pas de sol"<<endl;
    if(m_speed.m_y < 20)
      m_speed.m_y++;
    m_y += m_speed.m_y;
  } else {
    //m_y -= 5;
  }
}

void Character::die(){
  set_image("Die"+m_img);
}

void Character::spawn(int x, int y){
  m_x = x;
  m_y = y;
  set_image(m_img);
}


void Character::radical_move(int x, int y){
  set_image("Spew"+m_img);
  m_wait = 10;
  m_wrong_img = true;
} 


void Character::set_over_ground(){
  m_ground = false;
}


void Character::add_obstacle(Form f){
  m_obstacle.push_back(f);
}



direction_t Character::collide(){
  Collision c(this, m_obstacle);
  return c.get_direction();
}

