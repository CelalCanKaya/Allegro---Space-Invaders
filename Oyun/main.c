#include <stdio.h>
#include <allegro5/allegro.h>
#include <allegro5/allegro_primitives.h>
#include <time.h>
#define WIDTH 400
#define HEIGHT 600
#define FPS 60
#define MAX_BULLET 20
#define MAX_ENEMY 8

enum IDS{PLAYER, BULLET, ENEMY};

typedef struct{
    int ID;
    int x;
    int y;
    int lives;
    int speed;
    int boundx;
    int boundy;
}SpaceShip;

typedef struct{
	int ID;
	int x;
	int y;
	int live;
	int speed;
}Bullet;

typedef struct{
	int ID;
	int x;
	int y;
	int live;
	int speed;
	int boundx;
	int boundy;
}Enemy;

void create_display(ALLEGRO_DISPLAY *display);
void init_ship(SpaceShip *ship);
void draw_ship(SpaceShip ship);
void move_ship(SpaceShip *ship, int x, int y, int z, int w);
void init_bullet(Bullet bullet[], int i, int size);
void draw_bullet(Bullet bullet[], int i, int size);
void fire_bullet(Bullet bullet[], int i, int p, int size, SpaceShip *ship);
void update_bullet(Bullet bullet[], int i, int size);
void init_enemy(Enemy enemy[], int i, int size);
void draw_enemy(Enemy enemy[], int i, int size);
void spawn_enemy(Enemy enemy[], int i, int size);
void update_enemy(Enemy enemy[], int i, int size);


int main(){

    al_init();
    al_init_primitives_addon();
	al_install_keyboard();

    /* ********************************************** */
    ALLEGRO_DISPLAY *display = NULL;
    ALLEGRO_EVENT_QUEUE *event_queue = NULL;
	ALLEGRO_TIMER *timer = NULL;
    /* *********************************************** */

    display = al_create_display(WIDTH,HEIGHT);
	event_queue = al_create_event_queue();
	timer = al_create_timer(1.0 / FPS);

    SpaceShip ship;
    Enemy enemy[MAX_ENEMY];
    Bullet bullets[MAX_BULLET];
    int curBullet = 0;
    int delayBullet = 20;

    al_register_event_source(event_queue, al_get_keyboard_event_source());
	al_register_event_source(event_queue, al_get_display_event_source(display));
	al_register_event_source(event_queue, al_get_timer_event_source(timer));

    init_ship(&ship);
    draw_ship(ship);
    init_bullet(bullets,0,MAX_BULLET);
    init_enemy(enemy,0,MAX_ENEMY);

    al_start_timer(timer);

    int status = 1, redraw = 1, p_up = 0 , p_down = 0, p_left = 0 , p_right = 0, p_space = 0;

    while(status){
        ALLEGRO_EVENT ev;
		al_wait_for_event(event_queue, &ev);
		if(ev.type == ALLEGRO_EVENT_DISPLAY_CLOSE){
			status = 0;
		}
		else if(ev.type == ALLEGRO_EVENT_TIMER)
		{
			redraw = 1;
            move_ship(&ship,p_up,p_down,p_left,p_right);
            update_bullet(bullets,0,MAX_BULLET);
            if(++curBullet > delayBullet)
            {
               fire_bullet(bullets,0, p_space,MAX_BULLET,&ship);
               curBullet = 0;
            }
            spawn_enemy(enemy,0,MAX_ENEMY);
            update_enemy(enemy,0,MAX_ENEMY);
		}
		else if(ev.type == ALLEGRO_EVENT_KEY_DOWN){
			switch(ev.keyboard.keycode){
				case ALLEGRO_KEY_UP:
                    p_up = 1;
                    break;
				case ALLEGRO_KEY_DOWN:
                    p_down = 1;
					break;
                case ALLEGRO_KEY_LEFT:
                    p_left = 1;
					break;
				case ALLEGRO_KEY_RIGHT:
					p_right = 1;
					break;
				case ALLEGRO_KEY_SPACE:
				    p_space=1;
                    curBullet = delayBullet;
                    break;
			}
		}
		else if(ev.type == ALLEGRO_EVENT_KEY_UP){
			switch(ev.keyboard.keycode){
				case ALLEGRO_KEY_UP:
                    p_up = 0;
                    break;
				case ALLEGRO_KEY_DOWN:
                    p_down = 0;
					break;
                case ALLEGRO_KEY_LEFT:
                    p_left = 0;
					break;
				case ALLEGRO_KEY_RIGHT:
					p_right = 0;
					break;
				case ALLEGRO_KEY_SPACE:
                    p_space=0;
                    curBullet = 0;
					break;
			}
		}
        if(redraw && al_is_event_queue_empty(event_queue)){
			redraw = 0;
            al_clear_to_color(al_map_rgb(0,0,0));
			draw_ship(ship);
            draw_enemy(enemy,0,MAX_ENEMY);
			draw_bullet(bullets,0,MAX_BULLET);
			al_flip_display();
 		}
    }

    al_destroy_event_queue(event_queue);
	al_destroy_timer(timer);
    al_destroy_display(display);
    return 0;
}

void init_ship(SpaceShip *ship){
    ship->ID = PLAYER;
    ship->x = WIDTH/2;
    ship->y = HEIGHT - 50;
    ship->lives = 3;
    ship->speed = 5;
    ship->boundx = 6;
    ship->boundy = 7;

}

void draw_ship(SpaceShip ship){
    al_draw_filled_triangle(ship.x, ship.y, ship.x-15, ship.y+20, ship.x+15, ship.y+20, al_map_rgb(0,255,0));
}

void move_ship(SpaceShip *ship, int x, int y, int z, int w){
    if(x==1){
        ship->y -= ship->speed;
        if(ship->y < 0)
            ship->y = 0;
    }
    if(y==1){
        ship->y += ship->speed;
        if(ship->y > HEIGHT-20)
            ship->y = HEIGHT-20;
    }
    if(z==1){
        ship->x -= ship->speed;
        if(ship->x < 15)
            ship->x = 15;
    }
    if(w==1){
        ship->x += ship->speed;
        if(ship->x > WIDTH-15)
            ship->x = WIDTH-15;
    }
}

void init_bullet(Bullet bullet[], int i, int size){
    for(i = 0; i < size; i++){
		bullet[i].ID = BULLET;
		bullet[i].speed = 12;
		bullet[i].live = 0;
	}
}

void draw_bullet(Bullet bullet[], int i, int size){
    for(i = 0; i < size; i++){
        if(bullet[i].live){
            al_draw_filled_circle(bullet[i].x, bullet[i].y, 2, al_map_rgb(255, 255, 255));
        }
    }
}

void fire_bullet(Bullet bullet[], int i, int p, int size, SpaceShip *ship){
	for(i = 0; i < size; i++){
		if(!bullet[i].live && p){
			bullet[i].x = ship->x;
			bullet[i].y = ship->y;
			bullet[i].live = 1;
			break;
		}
	}
}

void update_bullet(Bullet bullet[], int i, int size){
    for(i=0; i < size; i++){
        if(bullet[i].live){
            bullet[i].y -= bullet[i].speed;
            if(bullet[i].y < 0){
                bullet[i].live = 0;
            }
        }
    }
}

void init_enemy(Enemy enemy[], int i, int size){
	for(i = 0; i < size; i++){
		enemy[i].ID = ENEMY;
		enemy[i].live = 0;
		enemy[i].speed = 4;
		enemy[i].boundx = 18;
		enemy[i].boundy = 18;
	}
}

void draw_enemy(Enemy enemy[], int i, int size){
    for(i = 0; i < size; i++){
		if(enemy[i].live){
			al_draw_filled_circle(enemy[i].x, enemy[i].y, 10, al_map_rgb(255, 0, 0));
		}
    }
}

void spawn_enemy(Enemy enemy[], int i, int size){
    for(i = 0; i < size; i++){
		if(!enemy[i].live){
            if(rand() % 500 == 0){
				enemy[i].live = 1;
				enemy[i].x = 15 + rand() % (WIDTH -30);
                enemy[i].y = 0;
            }
		}
	}
}

void update_enemy(Enemy enemy[], int i, int size){
    for(i = 0; i < size; i++){
		if(enemy[i].live){
        enemy[i].y += enemy[i].speed;}
        if(enemy[i].y > HEIGHT){
				enemy[i].live = 0;
		}
	}
}

