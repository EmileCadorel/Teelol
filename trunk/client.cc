#include "client.hh" 

namespace Teelol {


    session_on_client::session_on_client(socket &io): session<my_proto>(io), p(screen_s.h/2.0f - 25/2.0f ,screen_s.l/2.0f - 25/2.0f){

	sc = new Ecran(screen_s.h,screen_s.l);
	notif = new Notif(sc);


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
	proto.notif.sig_recv.connect(EZMETHOD(this, do_notif));

	sig_end.connect(EZMETHOD(this, on_end));
    }

    void session_on_client::on_end() {
	proto.quit();
	state = QUITTING;
    }
    
    //le serveur envoi que le joueur est en x,y
    void session_on_client::do_moveOk(int x ,int y){
	player->set_position(x, y);
    }

    //le joueur nick est en x,y
    void session_on_client::do_moved(int x, int y, string nick) {
	for(int i = 0; i < players.size(); i++) {
	    if(players[i]->get_nick() == nick) {
		players[i]->set_position(x, y);
		break;
	    }
	} 
    }
    
    //erreur envoyer du serveur
    void session_on_client::do_err(string err) {
	if(state == STARTING) {
	    tentative_connexion = false;
	}
	cout << "System : " << err << endl;	
    }
    
    //le nick est accepte
    void session_on_client::do_okNick(string n) {
	if(state == STARTING) {
	    cout << "System : nick changed !" << endl;
	    state = STARTED;
	    tentative_connexion = false;
	}
	player->set_nick(n);
	notif->add_notif("Bienvenue !");
    }
    
    //reponse positive du serveur
    void session_on_client::do_ok() {
	cout << "System : success" << endl;
    }

    //un nouveau joueur est present
    void session_on_client::do_joined(string nick) {
	players.push_back(new Player(nick, I_TEE_A, 0, 0, 10, 10, sc));
	string msg = nick + " a joint la partie !";
	notif->add_notif(msg.c_str());
    }

    //le joueur nick est partis
    void session_on_client::do_left(string nick) {
	for(int i = 0; i < players.size(); i++) {
	    if(players[i]->get_nick() == nick) {
		delete players[i];
		players[i] = players[players.size() - 1];
		players.pop_back();
		break;
	    }
	}
	string msg = nick + " a quitté la partie !";
	notif->add_notif(msg.c_str());
    }

    //reception d'un obstacle depuis le serveur
    void session_on_client::do_addObstacle(int img, int x, int y, int h, int l){
	obstacle.push_back(Form(x,y,h,l));
	obstacle[obstacle.size()-1].set_screen(sc);
	obstacle[obstacle.size()-1].set_image((Image_t)img);
    }
    
    //le joueur nick a tourner son arme de l'angle angle
    void session_on_client::do_rotated(int angle, string nick){
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
    void session_on_client::do_addBullet(string s, int x, int y, int x_s, int y_s){
	ezlock hold(mutex);
	int id = 0;
	map_bullet[s] = new Bullet(x,y,0,0,0,x_s,y_s,I_GRENADE_C,id);
	map_bullet[s]->set_screen(sc);
    }

    //un missile a explosé
    void session_on_client::do_explode(string s) {
	ezlock hold(mutex);
	int id = 0;
	vec_explode.push_back(Bullet(map_bullet[s]->get_x(), map_bullet[s]->get_y(), 0, 0, 0, 0, 0, I_CART_EX, id));
	vec_explode[vec_explode.size()-1].set_screen(sc);
	delete map_bullet[s];
	map_bullet.erase(s);
    }

    //le client a nb munitions
    void session_on_client::do_nbAmmo(int nb){
	player->get_ammo()->set_nb(nb);
    }

    //le joueur a pris dmg degat
    void session_on_client::do_hurt(int dmg){

	if(dmg > 0){
	    player->set_image(I_TEE_DMG);
	    player->loose_life(dmg);
	}
	else if(player->get_wrong_img())
	    player->set_image(I_TEE_P);
    }

    //le joueur _nick a pris des degat
    void session_on_client::do_hurted(string _nick){
	for(int i = 0; i < players.size(); i++) {
	    if(players[i]->get_nick() == _nick) {
		players[i]->set_image(I_TEE_A_DMG);
		players[i]->loose_life(1);
	    }
	}
    }
    
    //le client a recuperer h vie
    void session_on_client::do_health(int h){
	player->take_life(h);
    }
    
    //ajout d'un Item sur la map
    void session_on_client::do_addItem(int x, int y, int img, int id){
	ezlock hold(mutex);
	map_item[id] = new Item(x,y,0,0,(ITEM_T)0);
	auto it = map_item.find(id);
	it->second->set_image((Image_t)img);
	it->second->set_screen(sc);
    }


    //l'Item d'id i doit etre cache
    void session_on_client::do_hideItem(int i){
	ezlock hold(mutex);
	auto it = map_item.find(i);
	it->second->hide();
    }
    
    //l'item d'id i doit etre afficher
    void session_on_client::do_showItem(int i){
	ezlock hold(mutex);
	auto it = map_item.find(i);
	it->second->unhide();
    }

    //le joueur gagne 1 points
    void session_on_client::do_winPoint() {
	player->win_point();

    }
    
    //le joueur perd 1 points
    void session_on_client::do_loosePoint() {
	player->loose_point();
    }

    //affiche la notif envoyée par le serveur
    void session_on_client::do_notif(string msg) {
	notif->add_notif(msg.c_str());
    }

    //affiche les autres joueur avec leurs nom sur la tete
    void session_on_client::show_players(){
	for(int i = 0 ; i < players.size() ; i++){
	    point poi = p.parse_this(players[i], player->get_x(), player->get_y());
	    players[i]->show(poi.x, poi.y);
	    int x = poi.x + players[i]->get_l()/2;
	    int y = poi.y + players[i]->get_h()/2;
	    players[i]->get_weapon()->rotate(0,x,y,10);
	    players[i]->get_weapon()->show();
	    players[i]->show_nick(poi.x , poi.y);
	    if(players[i]->get_wrong_img())
		players[i]->set_image(I_TEE_A);
	}
    }

    //affiche le joueur
    void session_on_client::show_m_player(){
	point poi = p.parse_this(player, player->get_x(), player->get_y());
	player->show(poi.x, poi.y);
	if(player->get_wrong_img())
	    player->set_image(I_TEE_P);
	player->get_weapon()->show();
	player->get_ammo()->show(p, poi.x, poi.y);
	player->show_life();
	player->show_points();
      
    }

    //boucle d'affichage
    void session_on_client::affiche(){
	ezlock hold(mutex);
	sc->clean();
	for(int i = 0 ; i < obstacle.size() ; i++){
	    point poi = p.parse_this(&obstacle[i], player->get_x(), player->get_y());
	    obstacle[i].show(poi.x, poi.y);
	}
	auto it = map_bullet.begin();
	for(; it != map_bullet.end(); it++) {
	    point poi = p.parse_this(it->second, player->get_x(), player->get_y());
	    it->second->show(poi.x, poi.y);
	}
	for(int i = vec_explode.size()-1; i >= 0; i--) {
	    point poi = p.parse_this(&vec_explode[i], player->get_x(), player->get_y());
	    vec_explode[i].show(poi.x, poi.y);
	    vec_explode.pop_back();
	}
     
	show_players();
     
	for(auto it = map_item.begin() ; it != map_item.end() ; it++){
	    point poi = p.parse_this(it->second, player->get_x(), player->get_y());
	    it->second->show(poi.x, poi.y);
	}
	show_m_player();

	notif->pass_row();

	sc->Flip();

    }
    
    //fait tourner l'arme du joueur
    //envoi au serveur que l'arme a été tourne
    void session_on_client::rotationArme(int _x, int _y){
	int angle = -atan2(_x-(screen_s.l/2 - 25/2), _y-(screen_s.h/2 - 25/2))*180/M_PI+90;
	int x = screen_s.l/2 - 25/2 + player->get_l()/2;
	int y = screen_s.h/2 - 25/2 + player->get_h()/2; 
	player->get_weapon()->set_angle(angle);
	player->get_weapon()->rotate(0, x, y,10);
	proto.rotate(angle);
    }
    
    //envoi que le client a tire depuis les point envoyer
    void session_on_client::shoot(){

	int x1 = player->get_weapon()->get_xb();
	int y1 = player->get_weapon()->get_yb();
	point poi = p.inv_parse_this(x1, y1, player->get_x(), player->get_y());
	x1 = poi.x; y1 = poi.y;
	int x2 = player->get_weapon()->get_xba();
	int y2 = player->get_weapon()->get_yba();
	poi = p.inv_parse_this(x2,y2, player->get_x(), player->get_y());
	x2 = poi.x; y2 = poi.y;
	proto.shoot(x1,y1,x2,y2);
    }

    void session_on_client::map_Bullet_pass_row(){
	ezlock hold(mutex);
	auto it = map_bullet.begin();
	for(; it != map_bullet.end() ; it++){
	    it->second->pass_row();
	}
    }


    void session_on_client::do_event_left() {
	proto.move("left");
    }

    void session_on_client::do_event_right() {
	proto.move("right");
    }

    void session_on_client::do_event_jump() {
	proto.move("jump");
    }

    void session_on_client::do_event_quit() {
	//proto.quit();
    }

    void session_on_client::do_mouseevent_left() {
	shoot();
    }

    void session_on_client::do_mouseevent_motion(int x, int y) {
	rotationArme(x,y);
    }


    bool continuer = true;


    string boucle_pseudo(Teelol::session_on_client * c){

	Button b("valider", 150,250,50,100);
	Focuser f;
	Text t(150,150,50,100);
	f.add_focusable(&t);
	t.focused.connect(f);
	while(!e[QUIT] && !b.getClicked() && !t.Validated()){
	    e.UpdateEvent();
	    c->sc->clean();
	    b.pass_row(e);
	    t.pass_row(e);
	    b.show(c->sc);
	    t.show(c->sc);
	    c->sc->Flip();
	    SDL_Delay(50);
	}
	if(e[QUIT])
	    return "";
	else return t.getText();
    }


    void MapMenu(Ecran * sc) {
	Focuser f;

	CatchKey left(screen_s.l/2 , 50, 30,100), right (screen_s.l/2, 100, 30, 100) , jump(screen_s.l/2,150,30,100);
	Label l_left(screen_s.l/2 - 100 , 50,  "left"), l_right(screen_s.l/2 - 100, 100, "right"), l_jump(screen_s.l/2 - 100, 150 ,"jump");
	Button ok("OK", screen_s.l/2 - 50, 200, 30, 100);
	f.add_focusable(&left);
	f.add_focusable(&right);
	f.add_focusable(&jump);
	while ( !e[QUIT] && !ok.getClicked() ) {
	    e.UpdateEvent();
	    left.pass_row(e);
	    right.pass_row(e);
	    jump.pass_row(e);
	    left.show(sc);
	    right.show(sc);
	    jump.show(sc);
	    l_left.show(sc);
	    l_right.show(sc);
	    l_jump.show(sc);
	    ok.pass_row(e);
	    ok.show(sc);
	    sc->Flip();
	    sc->clean();
	}
	if ( left.getKey() != SDLK_EURO && left.getKey() != SDLK_ESCAPE ) {
	    e.change_key(LEFT, left.getKey());
	}
	if ( right.getKey() != SDLK_EURO  && right.getKey() != SDLK_ESCAPE ) {
	    e.change_key(RIGHT, right.getKey());
	}
	if ( jump.getKey() != SDLK_EURO && jump.getKey() != SDLK_ESCAPE  ) {
	    e.change_key(JUMP, jump.getKey());
	}
    }




    bool Menu(Ecran * sc) {
	e[QUIT] = 0;
	Focuser f;
	Button continuer("Continuer", Teelol::screen_s.l/2 - 50, 100, 30, 100);
	Button quitter("quitter", Teelol::screen_s.l/2 - 50, 350, 30, 100);
	NumberEdit largeur(Teelol::screen_s.l/2 - 100, 150, 30,200, 900,400);
	NumberEdit hauteur(Teelol::screen_s.l/2 - 100, 200, 30,200,800,400);
	Button map("Controle", screen_s.l/2 - 50 , 250, 30 , 100);
	Button ok("ok", Teelol::screen_s.l/2 - 50, 300,30, 100);
	f.add_focusable(&largeur);
	f.add_focusable(&hauteur);
	while( !e[QUIT] && !continuer.getClicked() && !quitter.getClicked() ) {
	    e.UpdateEvent();
	    sc->clean();
	    continuer.pass_row(e);
	    quitter.pass_row(e);
	    map.pass_row(e);
	    largeur.pass_row(e);
	    hauteur.pass_row(e);
	    ok.pass_row(e);
	    continuer.show(sc);
	    quitter.show(sc);
	    largeur.show(sc);
	    hauteur.show(sc);
	    ok.show(sc);
	    map.show(sc);
	    sc->Flip();
	    if( ok.getClicked() ) {
		Teelol::screen_s.l = largeur.getValue();
		Teelol::screen_s.h = hauteur.getValue();
		continuer.x() = Teelol::screen_s.l/2 - 50;
		quitter.x() = Teelol::screen_s.l/2 - 50;
		ok.x() = Teelol::screen_s.l/2 - 50;
		map.x() = screen_s.l/2 - 50;
		hauteur.set_x(Teelol::screen_s.l/2 - 100);
		largeur.set_x(Teelol::screen_s.l/2 - 100);
		sc->Resize(hauteur.getValue(), largeur.getValue());
	    }
	    if ( map.getClicked() ) {
		MapMenu(sc);
	    }
	}
	if (!quitter.getClicked() ) {
	    return true;
	} else {
	    return false;
	}
    }


    void * routine(void * arg){
  
	Teelol::session_on_client * c = (Teelol::session_on_client*)arg;
	while(c->state == Teelol::STARTING) {
	    string pseudo = boucle_pseudo(c);
	    if(pseudo == "")
		break;
	    c->proto.nick(pseudo);
    
	    while(tentative_connexion) {}
	    tentative_connexion = true;
	}
    
	if(c->state == Teelol::STARTED) {
	    int prec_wheel = e.WheelChange();
	    e.key_left.connect(boost::bind(&Teelol::session_on_client::do_event_left, c));
	    e.key_right.connect(boost::bind(&Teelol::session_on_client::do_event_right, c));
	    e.key_jump.connect(boost::bind(&Teelol::session_on_client::do_event_jump, c));
	    e.left_click.connect(boost::bind(&Teelol::session_on_client::do_mouseevent_left, c));
	    e.mouse_motion.connect(boost::bind(&Teelol::session_on_client::do_mouseevent_motion, c,_1, _2));
	    while(!e[QUIT] && c->state == Teelol::STARTED){
		e.UpdateEvent();
		if(e[JUMP]) { 
		    e[JUMP] = 0;
		}
		if(!e[RIGHT] && !e[LEFT]){
		    c->proto.move("stopx");
		}
		if( e[QUIT] ) {
		    if ( Menu(c->sc) ) {
			c->p.x() = Teelol::screen_s.l/2.0f - 25/2.0f;
			c->p.y() = Teelol::screen_s.h/2.0f - 25/2.0f;
			e[QUIT] = 0;
		    } else {
			e[QUIT] = 1;
		    }
		}
		if(e.WheelChange() != prec_wheel){
		    c->proto.change_weap(e.WheelChange() - prec_wheel);
		    prec_wheel = e.WheelChange();
		}
		c->map_Bullet_pass_row();
		c->affiche();
		SDL_Delay(40);
	    }
	}
	delete c->sc;
	c->proto.quit();
	c->finish();
    }

    void choose_server() {
	Ecran sc(Teelol::screen_s.h, Teelol::screen_s.l);
	Event e;
	ListView lv(10, 10, 280, 370, 30);
	Text zoneIp(150, 295, 25, 200);
	Button ok("Lancer", 10, 330, 25, 380);
	Focuser focus;
	focus.add_focusable(&zoneIp);
	focus.add_focusable(&lv);

	lv.add_Item(new ListItem("localhost"));
  
	while(!e[QUIT] && (!ok.getClicked() || !lv.selected())) {
	    e.UpdateEvent();
	    sc.clean();
	    lv.pass_row(e);
	    lv.show(&sc);
	    focus.pass_row(e);
	    zoneIp.pass_row(e);
	    zoneIp.show(&sc);
	    ok.pass_row(e);
	    ok.show(&sc);
	    sc.Flip();
	    SDL_Delay(50);
	}
  
	if(lv.selected() && !e[QUIT]) {
	    netez::client<Teelol::session_on_client> client(lv.selected()->text(), 9999);
	    pthread_t th;
	    pthread_create(&th, NULL, routine, (void*)&client.session);
	    pthread_join(th, NULL);
	}
    }
};

int main(int argc, char ** argv){
    TTF_Init();
    Teelol::screen_s.h = 400;
    Teelol::screen_s.l = 400;
    Teelol::choose_server();
}
