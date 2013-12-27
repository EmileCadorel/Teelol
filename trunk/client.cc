#include <vector>

#include "proto.hh"
#include "player.hh"
#include "gr/Event.hh"
#include "gr/Ecran.hh"

namespace Teelol {

  using namespace std;
  using namespace netez;
  
  enum state_t {
      STARTING,
      STARTED
  };

  struct session_on_client: public session<my_proto>{
    ezmutex mutex;
    vector<Player*> players;
    vector<Character> f;
    vector<Bullet> b;
    Player * player;
    Ecran  *sc;
    state_t state;

    session_on_client(socket &io): session<my_proto>(io){
      sc = new Ecran(400,400);
      player = new Player("nameless" , I_TEE_P, 0, 0, 10, 10, sc);
      state  = STARTING;
      cout<<"ici"<<endl;
      proto.moveOk.sig_recv.connect(EZMETHOD(this, do_moveOk));
      proto.moved.sig_recv.connect(EZMETHOD(this, do_moved));
      proto.err.sig_recv.connect(EZMETHOD(this, do_err));
      proto.ok.sig_recv.connect(EZMETHOD(this, do_ok));
      proto.joined.sig_recv.connect(EZMETHOD(this, do_joined));
      proto.left.sig_recv.connect(EZMETHOD(this, do_left));
      proto.okNick.sig_recv.connect(EZMETHOD(this, do_okNick));
      proto.addObstacle.sig_recv.connect(EZMETHOD(this, do_addObstacle));
      proto.rotated.sig_recv.connect(EZMETHOD(this, do_rotated));
      proto.showMissile.sig_recv.connect(EZMETHOD(this, do_showMissile));
      //sig_begin.connect(EZMETHOD(this,on_begin));
      sig_end.connect(EZMETHOD(this, on_end));
    }

    /* void on_begin() {
      string n;
      cout << "nick : ";
      cin >> n;

      proto.nick(n);

      }*/

    void on_end() {
      proto.quit();
    }
    
    void do_moveOk(int x ,int y){
      player->set_position(x, y);
    }

    void do_moved(int x, int y, string nick) {
      for(int i = 0; i < players.size(); i++) {
        if(players[i]->get_nick() == nick) {
          players[i]->set_position(x, y);
          break;
        }
      } 
    }

    void do_err(string err) {
      cout << "System : " << err << endl;
    }
    
    void do_okNick(string n) {
      if(state == STARTING) {

        state = STARTED;
      }
      player->set_nick(n);
      cout << "System : nick changed !" << endl;
    }
    
    void do_ok() {
      cout << "System : success" << endl;
    }

    void do_joined(string nick) {
      players.push_back(new Player(nick, I_TEE_P, 0, 0, 10, 10, sc));
    }

    void do_left(string nick) {
      for(int i = 0; i < players.size(); i++) {
        if(players[i]->get_nick() == nick) {
          delete players[i];
          players[i] = players[players.size() - 1];
          players.pop_back();
          break;
        }
      }
    }

    void do_addObstacle(int x, int y, int h, int l){
      
      f.push_back(Character(I_MUR,x,y,h,l,sc));
    }
    
    void do_rotated(int angle, string nick){
       for(int i = 0; i < players.size(); i++) {
        if(players[i]->get_nick() == nick) {
          players[i]->get_weapon()->set_angle(angle);
	  int x = players[i]->get_x() + players[i]->get_l()/2;
	  int y = players[i]->get_y() + players[i]->get_h()/2;
	  players[i]->get_weapon()->rotate(0,x,y,0);
          break;
        }
      } 
    }
    
    void do_showMissile(int x, int y){
      ezlock hold(mutex);
      b.push_back(Bullet(x,y,0,0,0,0,0,I_GRENADE_C));
      b[b.size() - 1].set_screen(sc);
    }

    
    void affiche(){
      ezlock hold(mutex);
      sc->clean();
      for(int i = 0 ; i < f.size() ; i++){
	f[i].show();
      }
      for(int i = b.size()-1; i >= 0 ; i--){
	b[i].show();
	b.pop_back();
      }
      for(int i = 0 ; i < players.size() ; i++){
	players[i]->show();
	players[i]->get_weapon()->show();
      }
      player->show();
      player->get_weapon()->show();
      sc->Flip();
    }
    
    void rotationArme(int _x, int _y){
      int angle = -atan2(_x-player->get_x(), _y-player->get_y())*180/M_PI+90;
      int x = player->get_x()+ player->get_l()/2;;
      int y = player->get_y() + player->get_h()/2; 
      player->get_weapon()->set_angle(angle);
      player->get_weapon()->rotate(0, x, y,0);
      proto.rotate(angle);
    }
    
    void shoot(){
      int x1 = boost::lexical_cast<int>(player->get_weapon()->get_xb());
      int y1 = boost::lexical_cast<int>(player->get_weapon()->get_yb());
      int x2 = boost::lexical_cast<int>(player->get_weapon()->get_xba());
      int y2 = boost::lexical_cast<int>(player->get_weapon()->get_yba());

      proto.shoot(x1,y1,x2,y2);
    }

  };
};




void * routine(void * arg){
  
  Event e;
  Teelol::session_on_client * c = (Teelol::session_on_client*)arg;
  string pseudo;
  cin>>pseudo;
  c->proto.nick(pseudo);
  cout<<"message envoye"<<endl;
  while(!e[QUIT]){
    e.UpdateEvent();
    if(e[LEFT]){
      c->proto.move("left");
    }
    if(e[RIGHT]){
      c->proto.move("right");
    }
    if(!e[RIGHT] && !e[LEFT]){
      c->proto.move("stopx");
    }
    if(e[JUMP]){ c->proto.move("jump"); e[JUMP] = 0;}
    if(e[LEFT_CL]){ c->shoot();}
    c->rotationArme(e().m_x, e().m_y);
    SDL_Delay(50);
    c->sc->clean();
    c->affiche();
  }
  delete c->sc;
  c->proto.quit();
}


int main(int argc, char ** argv){
  netez::client<Teelol::session_on_client> client(argc,argv);
  cout<<"ici"<<endl;
  pthread_t th;
  pthread_create(&th, NULL, routine, (void*)&client.session);
  pthread_join(th,NULL);
  client.join();
  
  
}
