#include "Ecran.hh"
#include "Event.hh"
#include "Character.hh"
#include "Bullet.hh"



void routine(){

  Ecran sc(500,500);

  Event e;
  Character r(I_TEE_P,50,20,10,10,&sc);
  Form f (20,200,10,300);
  Form f2 (40,221,10,300);
  Form f3 (75,200,10,300);
  f.set_screen(&sc);
  f2.set_screen(&sc);
  f3.set_screen(&sc);
  f.set_image(I_MUR);
  f2.set_image(I_MUR);
  f3.set_image(I_MUR);


  /* On ajoute le mur et la balle dans la liste de collisions */
  r << f;
  r << f2;
  r << f3;


  while(!e[QUIT]){
    e.UpdateEvent();
    if(e[LEFT])
      r.move_left();
    else if(e[RIGHT])
      r.move_right();
    else r.stop_x();
    if(e[JUMP]) {
      r.jump();
      e.reset_pressed(JUMP);
    }
    r.get_weapon()->set_angle(-atan2(e().m_x-(r.get_x()+ r.get_l()/2), e().m_y-(r.get_y() + r.get_h()/2))*180/M_PI+90);
    r.get_weapon()->rotate(0, r.get_x() + r.get_l()/2, r.get_y() + r.get_h()/2, 10);
    if(e[LEFT_CL]){
      r.shoot();
    }
    if(!e[LEFT_CL])
      sc.clean();
    r.pass_row();
    r.show();
    r.get_weapon()->show();
    f.show();
    f2.show();
    f3.show();
    sc.Flip();
    SDL_Delay(50);
  }

}



int main(){
  routine();
  return 0;
}
