#include <vector>

#include "proto.hh"
#include "player.hh"
#include "gr/Event.hh"
#include "gr/Ecran.hh"
#include "gr/Item.hh"

bool tentative_connexion = true;

namespace Teelol {

  using namespace std;
  using namespace netez;
  
  enum state_t {
      STARTING,
      STARTED,
      QUIT
  };

  struct session_on_client: public session<my_proto>{
    ezmutex mutex;
    vector<Player*> players;
    vector<Form> obstacle;
    map<string,Bullet*> map_bullet;
    vector<Bullet> vec_explode;
    map<int, Item*> map_item;
    Player * player;
    Ecran  *sc;
    state_t state;    

    session_on_client(socket &io): session<my_proto>(io){

      sc = new Ecran(400,400);
      player = new Player("nameless" , I_TEE_P, 0, 0, 10, 10, sc);
      state  = STARTING;
     
      proto.moveOk.sig_recv.connect(EZMETHOD(this, do_moveOk));
      proto.moved.sig_recv.connect(EZMETHOD(this, do_moved));
      proto.err.sig_recv.connect(EZMETHOD(this, do_err));
      proto.ok.sig_recv.connect(EZMETHOD(this, do_ok));
      proto.joined.sig_recv.connect(EZMETHOD(this, do_joined));
      proto.left.sig_recv.connect(EZMETHOD(this, do_left));
      proto.okNick.sig_recv.connect(EZMETHOD(this, do_okNick));
      proto.addObstacle.sig_recv.connect(EZMETHOD(this, do_addObstacle));
      proto.rotated.sig_recv.connect(EZMETHOD(this, do_rotated));
      proto.addBullet.sig_recv.connect(EZMETHOD(this, do_addBullet));
      proto.nbAmmo.sig_recv.connect(EZMETHOD(this, do_nbAmmo));
      proto.hurt.sig_recv.connect(EZMETHOD(this, do_hurt));
      proto.hurted.sig_recv.connect(EZMETHOD(this, do_hurted));
      proto.health.sig_recv.connect(EZMETHOD(this, do_health));
      proto.addItem.sig_recv.connect(EZMETHOD(this, do_addItem));
      proto.hideItem.sig_recv.connect(EZMETHOD(this, do_hideItem));
      proto.showItem.sig_recv.connect(EZMETHOD(this, do_showItem));
      proto.winPoint.sig_recv.connect(EZMETHOD(this, do_winPoint));
      proto.loosePoint.sig_recv.connect(EZMETHOD(this, do_loosePoint));
      proto.explode.sig_recv.connect(EZMETHOD(this, do_explode));

      sig_end.connect(EZMETHOD(this, on_end));
    }

    void on_end() {
      proto.quit();
      state = QUIT;
    }
    
    //le serveur envoi que le joueur est en x,y
    void do_moveOk(int x ,int y){
      player->set_position(x, y);
    }

    //le joueur nick est en x,y
    void do_moved(int x, int y, string nick) {
      for(int i = 0; i < players.size(); i++) {
        if(players[i]->get_nick() == nick) {
          players[i]->set_position(x, y);
          break;
        }
      } 
    }
    
    //erreur envoyer du serveur
    void do_err(string err) {
      if(state == STARTING) {
	tentative_connexion = false;
      }
      cout << "System : " << err << endl;	
    }
    
    //le nick est accepte
    void do_okNick(string n) {
      if(state == STARTING) {
        cout << "System : nick changed !" << endl;
        state = STARTED;
	tentative_connexion = false;
      }
      player->set_nick(n);
      
    }
    
    //reponse positive du serveur
    void do_ok() {
      cout << "System : success" << endl;
    }

    //un nouveau joueur est present
    void do_joined(string nick) {
      players.push_back(new Player(nick, I_TEE_A, 0, 0, 10, 10, sc));
    }

    //le joueur nick est partis
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

    //reception d'un obstacle depuis le serveur
    void do_addObstacle(int img, int x, int y, int h, int l){
      obstacle.push_back(Form(x,y,h,l));
      obstacle[obstacle.size()-1].set_screen(sc);
      obstacle[obstacle.size()-1].set_image((Image_t)img);
    }
    
    //le joueur nick a tourner son arme de l'angle angle
    void do_rotated(int angle, string nick){
       for(int i = 0; i < players.size(); i++) {
        if(players[i]->get_nick() == nick) {
          players[i]->get_weapon()->set_angle(angle);
	  int x = players[i]->get_x() + players[i]->get_l()/2;
	  int y = players[i]->get_y() + players[i]->get_h()/2;
	  players[i]->get_weapon()->rotate(0,x,y,10);
          break;
        }
      } 
    }
    
    //un missile est tiré en x,y
    void do_addBullet(string s, int x, int y, int x_s, int y_s){
      ezlock hold(mutex);
      int id = 0;
      map_bullet[s] = new Bullet(x,y,0,0,0,x_s,y_s,I_GRENADE_C,id);
      map_bullet[s]->set_screen(sc);
    }

    //un missile a explosé
    void do_explode(string s) {
      ezlock hold(mutex);
      int id = 0;
      vec_explode.push_back(Bullet(map_bullet[s]->get_x(), map_bullet[s]->get_y(), 0, 0, 0, 0, 0, I_CART_EX, id));
      vec_explode[vec_explode.size()-1].set_screen(sc);
      delete map_bullet[s];
      map_bullet.erase(s);
    }

    //le client a nb munitions
    void do_nbAmmo(int nb){
      player->get_ammo()->set_nb(nb);
    }

    //le joueur a pris dmg degat
    void do_hurt(int dmg){

      if(dmg > 0){
	player->set_image(I_TEE_DMG);
	player->loose_life(dmg);
      }
      else if(player->get_wrong_img())
	player->set_image(I_TEE_P);
    }

    //le joueur _nick a pris des degat
    void do_hurted(string _nick){
      for(int i = 0; i < players.size(); i++) {
	if(players[i]->get_nick() == _nick) {
	  players[i]->set_image(I_TEE_A_DMG);
	  players[i]->loose_life(1);
	}
      }
    }
    
    //le client a recuperer h vie
    void do_health(int h){
      player->take_life(h);
    }
    
    //ajout d'un Item sur la map
    void do_addItem(int x, int y, int img, int id){
      ezlock hold(mutex);
      map_item[id] = new Item(x,y,0,0,(ITEM_T)0);
      auto it = map_item.find(id);
      it->second->set_image((Image_t)img);
      it->second->set_screen(sc);
    }


    //l'Item d'id i doit etre cache
    void do_hideItem(int i){
      ezlock hold(mutex);
      auto it = map_item.find(i);
      it->second->hide();
    }
    
    //l'item d'id i doit etre afficher
    void do_showItem(int i){
      ezlock hold(mutex);
      auto it = map_item.find(i);
      it->second->unhide();
    }

    //le joueur gagne 1 points
    void do_winPoint() {
      player->win_point();

    }
    
    //le joueur perd 1 points
    void do_loosePoint() {
      player->loose_point();

    }


    //affiche les autres joueur avec leurs nom sur la tete
    void show_players(){
      for(int i = 0 ; i < players.size() ; i++){
	players[i]->show();
	players[i]->show_nick();
	players[i]->get_weapon()->show();
	if(players[i]->get_wrong_img())
	  players[i]->set_image(I_TEE_A);
      }
    }

    //affiche le joueur
    void show_m_player(){
      player->show();
      if(player->get_wrong_img())
	player->set_image(I_TEE_P);
      player->get_weapon()->show();
      player->get_ammo()->show();
      player->show_life();
      player->show_points();
      
    }

    //boucle d'affichage
    void affiche(){
      ezlock hold(mutex);
      sc->clean();
      for(int i = 0 ; i < obstacle.size() ; i++){
	obstacle[i].show();
      }
      auto it = map_bullet.begin();
      for(; it != map_bullet.end(); it++) {
	it->second->show();
      }
      for(int i = vec_explode.size()-1; i >= 0; i--) {
	vec_explode[i].show();
	vec_explode.pop_back();
      }
     
      show_players();
     
      for(auto it = map_item.begin() ; it != map_item.end() ; it++){
	it->second->show();
      }
      show_m_player();

      sc->Flip();

    }
    
    //fait tourner l'arme du joueur
    //envoi au serveur que l'arme a été tourne
    void rotationArme(int _x, int _y){
      int angle = -atan2(_x-player->get_x(), _y-player->get_y())*180/M_PI+90;
      int x = player->get_x()+ player->get_l()/2;;
      int y = player->get_y() + player->get_h()/2; 
      player->get_weapon()->set_angle(angle);
      player->get_weapon()->rotate(0, x, y,10);
      proto.rotate(angle);
    }
    
    //envoi que le client a tire depuis les point envoyer
    void shoot(){
      int x1 = player->get_weapon()->get_xb();
      int y1 = player->get_weapon()->get_yb();
      int x2 = player->get_weapon()->get_xba();
      int y2 = player->get_weapon()->get_yba();
      
      proto.shoot(x1,y1,x2,y2);
    }

    void map_Bullet_pass_row(){
      ezlock hold(mutex);
      auto it = map_bullet.begin();
      for(; it != map_bullet.end() ; it++){
	it->second->pass_row();
      }
    }
    

   };
};



//boucle de connexion et d'interaction
void * routine(void * arg){
  
  Event e;
  Teelol::session_on_client * c = (Teelol::session_on_client*)arg;

  string pseudo;
  while(c->state == Teelol::STARTING) {
    cout << "Pseudo : ";
    cin>>pseudo;
    c->proto.nick(pseudo);
    cout<<"message envoye"<<endl;

    while(tentative_connexion) {}
    tentative_connexion = true;
  }
  
  if(c->state == Teelol::STARTED) {
    while(!e[QUIT] && c->state == Teelol::STARTED){
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
    c->affiche();
    c->map_Bullet_pass_row();
    SDL_Delay(40);
  }
  }

  delete c->sc;
  c->proto.quit();
}


int main(int argc, char ** argv){
  TTF_Init();
  netez::client<Teelol::session_on_client> client(argc,argv);

  pthread_t th;
  pthread_create(&th, NULL, routine, (void*)&client.session);
  pthread_join(th,NULL);
  client.join();
  
  
}
