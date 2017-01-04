#include <Arduboy2.h>
#define BG_STARS_NUM 10
#define PLAYER_WIDTH 8
#define PLAYER_HEIGHT 8
#define PLAYER_SHOT_NUM 15
#define ENEMY_SHOT_NUM 27
#define SHOT_INTERVAL_FRAME 4
#define MAX_ENEMY_NUM 12
#define EXPLOSION_NUM 10
#define TINY_PI 3.141516
#define BOSS_HP 100

#define EN_MOV_STRAIGHT 0
#define EN_MOV_SIN 1
#define EN_MOV_TARGET 2
#define EN_MOV_BOSS1 3

#define EN_SHOT_STRAIGHT 0
#define EN_SHOT_TARGET 1
#define EN_SHOT_HOMING 2;
#define EN_SHOT_3WAY 3;


#define NONE -1

#define MODE_TITLE 0
#define MODE_INTRO 5
#define MODE_GAME 10
#define MODE_CLEAR 11
#define MODE_NEXT_STAGE 12
#define MODE_PLAYER_DIE 15
#define MODE_GAME_OVER 20
#define MODE_SCORE 30

unsigned int hi_score;
unsigned int score;
unsigned int frame;
unsigned short timeline_last_index;
char shot_interval;
char game_mode;
char game_level;
bool is_pause;

/****************************************************
   Timeline
 ****************************************************/
typedef struct {
  unsigned short frame;
  short w;
  short h;
  short x;
  short y;
  short angle;
  char x_speed;
  char y_speed;
  short hp;
  short enemy_type;
  short enemy_shot_type;
} EnemyTimeline;


/****************************************************
   Player
 ****************************************************/
typedef struct {
  float x, y;
  float vx, vy;
  bool is_shot;
  bool is_active;
} Player;



/***************************************************
   PlayerShot
 ****************************************************/
typedef struct {
  unsigned char x, y;
  bool is_active;
} PlayerShot;



/***************************************************
   EnemyShot
 ****************************************************/
typedef struct {
  short x, y;
  short angle;
  char speed;
  char type;
  bool is_active;
} EnemyShot;



/***************************************************
   Enemy
 ****************************************************/
typedef struct {
  char type;
  char shot_type;
  float x, y;
  short angle;
  char x_speed;
  char y_speed;
  char w, h;
  unsigned short hitpoint;
  bool is_active;
  unsigned short frame;
  bool is_receive_damage;
} Enemy;


/***************************************************
   Explosion
 ****************************************************/
typedef struct {
  char x, y;
  char speed;
  char radius;
  char dest_radius;
  bool is_active;
} Explosion;

// make an instance of arduboy used for many functions
Arduboy2 arduboy;
Player pl;
PlayerShot pl_shots[PLAYER_SHOT_NUM];
PlayerShot bg_stars[BG_STARS_NUM];
Enemy enemies[MAX_ENEMY_NUM];
Explosion explosions[EXPLOSION_NUM];
EnemyShot enemy_shots[ENEMY_SHOT_NUM];
Enemy* current_boss;

char shake_amount;
char shake_x, shake_y;


PROGMEM const EnemyTimeline en_timeline[] = {
//{frame,w,h,x,y,angle,x_speed,y_speed,hp,enemy_type, enemy_shot_type},
  {100, 10, 10, 128, 10, 270, 10, 0, 1, EN_MOV_STRAIGHT, EN_SHOT_TARGET},
  {125, 10, 10, 128, 10, 270, 10, 0, 1, EN_MOV_STRAIGHT, EN_SHOT_TARGET},
  {150, 10, 10, 128, 10, 270, 10, 0, 1, EN_MOV_STRAIGHT, EN_SHOT_TARGET},
  {175, 10, 10, 128, 10, 270, 10, 0, 1, EN_MOV_STRAIGHT, EN_SHOT_TARGET},
  {175, 10, 10, 128, 50, 270, 10, 0, 1, EN_MOV_STRAIGHT, EN_SHOT_TARGET},
  {200, 10, 10, 128, 10, 270, 10, 0, 1, EN_MOV_STRAIGHT, EN_SHOT_TARGET},
  {200, 10, 10, 128, 50, 270, 10, 0, 1, EN_MOV_STRAIGHT, EN_SHOT_TARGET},
  {225, 10, 10, 128, 50, 270, 10, 0, 1, EN_MOV_STRAIGHT, EN_SHOT_TARGET},
  {250, 10, 10, 128, 50, 270, 10, 0, 1, EN_MOV_STRAIGHT, EN_SHOT_TARGET},

  {250, 10, 10, 128, 10, 270, 10, 0, 1, EN_MOV_STRAIGHT, EN_SHOT_TARGET},
  {275, 10, 10, 128, 10, 270, 10, 0, 1, EN_MOV_STRAIGHT, EN_SHOT_TARGET},
  {300, 10, 10, 128, 10, 270, 10, 0, 1, EN_MOV_STRAIGHT, EN_SHOT_TARGET},
  {325, 10, 10, 128, 10, 270, 10, 0, 1, EN_MOV_STRAIGHT, EN_SHOT_TARGET},
  {350, 10, 10, 128, 50, 270, 10, 0, 1, EN_MOV_STRAIGHT, EN_SHOT_TARGET},
  {375, 10, 10, 128, 10, 270, 10, 0, 1, EN_MOV_STRAIGHT, EN_SHOT_TARGET},
  {375, 10, 10, 128, 50, 270, 10, 0, 1, EN_MOV_STRAIGHT, EN_SHOT_TARGET},
  {400, 10, 10, 128, 50, 270, 10, 0, 1, EN_MOV_STRAIGHT, EN_SHOT_TARGET},
  {425, 10, 10, 128, 50, 270, 10, 0, 1, EN_MOV_STRAIGHT, EN_SHOT_TARGET},

 // 18
  {475, 10, 10, 128, 10, 270, 2, 40, 1, EN_MOV_SIN, EN_SHOT_STRAIGHT},
  {475, 10, 10, 128, 20, 270, 2, 40, 1, EN_MOV_SIN, EN_SHOT_STRAIGHT},
  {475, 10, 10, 128, 30, 270, 4, 5, 1, EN_MOV_SIN, EN_SHOT_TARGET},
  {475, 10, 10, 128, 40, 270, 2, 40, 1, EN_MOV_SIN, EN_SHOT_STRAIGHT},
  {475, 10, 10, 128, 50, 270, 2, 40, 1, EN_MOV_SIN, EN_SHOT_STRAIGHT},

  {575, 10, 10, 128, 10, 270, 2, 40, 1, EN_MOV_SIN, EN_SHOT_STRAIGHT},
  {575, 10, 10, 128, 20, 270, 2, 40, 1, EN_MOV_SIN, EN_SHOT_STRAIGHT},
  {575, 10, 10, 128, 30, 270, 4, 5, 1, EN_MOV_SIN, EN_SHOT_TARGET},
  {575, 10, 10, 128, 40, 270, 2, 40, 1, EN_MOV_SIN, EN_SHOT_STRAIGHT},
  {575, 10, 10, 128, 50, 270, 2, 40, 1, EN_MOV_SIN, EN_SHOT_STRAIGHT},
 // 29
  {700, 60, 40, 128, 10, 270, 3, 40, 20, EN_MOV_SIN, EN_SHOT_STRAIGHT},

  {100 + 700, 10, 10, 128, 10, 270, 10, 0, 1, EN_MOV_STRAIGHT, EN_SHOT_TARGET},
  {150 + 700, 10, 10, 128, 10, 270, 10, 0, 1, EN_MOV_STRAIGHT, EN_SHOT_TARGET},
  {200 + 700, 10, 10, 128, 50, 270, 10, 0, 1, EN_MOV_STRAIGHT, EN_SHOT_TARGET},
  {250 + 700, 10, 10, 128, 50, 270, 10, 0, 1, EN_MOV_STRAIGHT, EN_SHOT_TARGET},

// 34
  {1100, 10, 10, 128, 10, 270, 35, 15, 1, EN_MOV_TARGET, EN_SHOT_TARGET},
  {1110, 10, 10, 128, 20, 270, 35, 15, 1, EN_MOV_TARGET, EN_SHOT_TARGET},
  {1120, 10, 10, 128, 30, 270, 35, 15, 1, EN_MOV_TARGET, EN_SHOT_TARGET},
  {1130, 10, 10, 128, 40, 270, 35, 15, 1, EN_MOV_TARGET, EN_SHOT_TARGET},
  {1140, 10, 10, 128, 10, 270, 35, 15, 1, EN_MOV_TARGET, EN_SHOT_TARGET},
  {1200, 10, 10, 128, 10, 270, 35, 15, 1, EN_MOV_TARGET, EN_SHOT_TARGET},
  {1200, 10, 10, 128, 20, 270, 35, 15, 1, EN_MOV_TARGET, EN_SHOT_TARGET},
  {1200, 10, 10, 128, 30, 270, 35, 15, 1, EN_MOV_TARGET, EN_SHOT_TARGET},
  {1200, 10, 10, 128, 40, 270, 35, 15, 1, EN_MOV_TARGET, EN_SHOT_TARGET},

//43
  {1300, 10, 10, 128, 10, 270, 5, 15, 3, EN_MOV_SIN, EN_SHOT_STRAIGHT},
  {1300, 10, 10, 128, 40, 270, 5, 15, 3, EN_MOV_SIN, EN_SHOT_STRAIGHT},
  {1330, 10, 10, 128, 20, 270, 5, 15, 3, EN_MOV_SIN, EN_SHOT_STRAIGHT},
  {1330, 10, 10, 128, 30, 270, 5, 15, 3, EN_MOV_SIN, EN_SHOT_STRAIGHT},
  {1360, 10, 10, 128, 25, 270, 5, 15, 5, EN_MOV_SIN, EN_SHOT_STRAIGHT},
  
  {1500, 40, 40, 128, 15, 270, 5, 15, BOSS_HP, EN_MOV_BOSS1, EN_SHOT_TARGET},
  

};


void initialize() {

  // setup game.
  timeline_last_index = 0;
  frame = 0;
  
  current_boss = NULL;
  
  // set shake.
  shake_amount = 0;

  // setup player.
  pl.is_active = true;
  pl.is_shot = false;
  pl.y = 30;
  pl.x = 5;
  // create bg.
  create_bg();

  /***************************************************************************************
     Initialize enemy.
   ****************************************************************************************/
  for (int ii = 0; ii < MAX_ENEMY_NUM; ii++) {
    enemies[ii].x = 0;
    enemies[ii].y = 0;
    enemies[ii].angle = 0;
    enemies[ii].x_speed = 0;
    enemies[ii].y_speed = 0;
    enemies[ii].type = 0;
    enemies[ii].hitpoint = 0;
    enemies[ii].w = 0;
    enemies[ii].h = 0;
    enemies[ii].is_active = false;
    enemies[ii].is_receive_damage = false;
  }

  /***************************************************************************************
     Initialize explosion
   ****************************************************************************************/
  for (int ii = 0; ii < EXPLOSION_NUM; ii++) {
    explosions[ii].x = 0;
    explosions[ii].y = 0;
    explosions[ii].radius = 0;
    explosions[ii].dest_radius = 0;
    explosions[ii].is_active = false;
  }


  /***************************************************************************************
     Initialize Enemy shots.
   ****************************************************************************************/
  for (int ii = 0; ii < ENEMY_SHOT_NUM; ii++) {
    enemy_shots[ii].x = 0;
    enemy_shots[ii].y = 0;
    enemy_shots[ii].angle = 0;
    enemy_shots[ii].speed = 0;
    enemy_shots[ii].type = EN_SHOT_STRAIGHT;
    enemy_shots[ii].is_active = false;
  }

  // debug
//  for (int ii = 0; ii < 4; ii++) {
//    enemies[ii].x = 108;
//    enemies[ii].y = ii * 18 ;
//    enemies[ii].angle = 270;
//    enemies[ii].x_speed = 0;
//    enemies[ii].y_speed = 60;
//    enemies[ii].w = 10;
//    enemies[ii].h = 15;
//    enemies[ii].hitpoint = 10;
//    enemies[ii].is_active = true;
//    enemies[ii].type = EN_MOV_STRAIGHT;
//    enemies[ii].shot_type = EN_SHOT_STRAIGHT;
//  }
}

void set_game_mode(char new_game_mode) {
  frame = 0;
  game_mode = new_game_mode;
  if(new_game_mode == MODE_TITLE) {
    if(score > hi_score) {
      hi_score = score;
    }
  }
}

/****************************************************************************************
   Setup
 ****************************************************************************************/
void setup() {
  // initiate arduboy instance
  arduboy.boot();

  // here we set the framerate to 15, we do not need to run at
  // default 60 and it saves us battery life
  arduboy.setFrameRate(60);

  // setup initial mode.
  set_game_mode(MODE_TITLE);
  score = 0;
  hi_score = 0;
  game_level = 3;
  is_pause = false;
  initialize();
}

void create_bg() {
  for (int ii = 0; ii < BG_STARS_NUM; ii++) {
    bg_stars[ii].x = random(0, 120);
    bg_stars[ii].y = random(0, 80);
  }
}


/***************************************************************************************
   Rent a explosion object
 ****************************************************************************************/
Explosion* rent_explosion() {
  for (int ii = 0; ii < EXPLOSION_NUM; ii++) {
    if (explosions[ii].is_active == false) {
      explosions[ii].is_active = true;
      explosions[ii].x = 0;
      explosions[ii].y = 0;
      explosions[ii].radius = 0;
      explosions[ii].dest_radius = 0;
      return &explosions[ii];
    }
  }
  return NULL;
}

Enemy* rent_enemy() {
  for(int ii=0;ii<MAX_ENEMY_NUM;ii++) {
    if(enemies[ii].is_active == false) {
      enemies[ii].type = 0;
      enemies[ii].shot_type = 0;
      enemies[ii].x = 0;
      enemies[ii].y = 0;
      enemies[ii].angle = 0;
      enemies[ii].x_speed = 0;
      enemies[ii].y_speed = 0;
      enemies[ii].w = 0;
      enemies[ii].h = 0;
      enemies[ii].hitpoint = 0;
      enemies[ii].is_active = true;
      enemies[ii].frame = 0;
      return &enemies[ii];
    }
  }

  return NULL;
}

EnemyShot* rent_enemy_shot() {
  for (int ii = 0; ii < ENEMY_SHOT_NUM; ii++) {
    if (enemy_shots[ii].is_active == false) {
      enemy_shots[ii].is_active = true;
      enemy_shots[ii].x = 0;
      enemy_shots[ii].y = 0;
      enemy_shots[ii].speed = 10;
      enemy_shots[ii].type = EN_SHOT_STRAIGHT;
      return &enemy_shots[ii];
    }
  }
  return NULL;
}

void die_player() {
  Explosion *explosion = rent_explosion();
  if (explosion != NULL) {
    explosion->x = pl.x + PLAYER_WIDTH / 2;
    explosion->y = pl.y + PLAYER_HEIGHT / 2;
    explosion->dest_radius = random(20, 30);
  }

  pl.is_active = false;
  shake_amount = 40;
  set_game_mode(MODE_PLAYER_DIE);

}

/***************************************************************************************
   Update enemy shot.
 ****************************************************************************************/
void update_enemy_shot() {
  for (int ii = 0; ii < ENEMY_SHOT_NUM; ii++) {
    if (enemy_shots[ii].is_active) {

      /***************************
         At first check collision with player
       ****************************/
      float en_x = enemy_shots[ii].x * 0.01;
      float en_y = enemy_shots[ii].y * 0.01;
      if (game_mode == MODE_GAME && pl.is_active && (pl.x + PLAYER_WIDTH / 2 - 2) < en_x &&  (pl.x + PLAYER_WIDTH / 2 + 2) > en_x &&
          (pl.y + PLAYER_HEIGHT / 2 - 2) < en_y &&  (pl.y + PLAYER_HEIGHT / 2 + 2) > en_y) {
        enemy_shots[ii].is_active = false;
        die_player();
        continue;
      }

      float vx = 0;
      float vy = 0;
      switch (enemy_shots[ii].type) {

        case EN_SHOT_STRAIGHT:
          vx = cos((enemy_shots[ii].angle * 0.01) * TINY_PI / 180) * 100;
          vy = sin((enemy_shots[ii].angle * 0.01) * TINY_PI / 180) * 100;
        break;
          
      }

      vx *= enemy_shots[ii].speed * 0.1;
      vy *= enemy_shots[ii].speed * 0.1;

      short tmp_x = enemy_shots[ii].x + vx;
      short tmp_y = enemy_shots[ii].y + vy;
      
      if (tmp_x * 0.01 <= 0 || tmp_x * 0.01 >= 128 || tmp_y * 0.01 <= 0 || tmp_y * 0.01 >= 60) {
        enemy_shots[ii].is_active = false;
      } else {
        enemy_shots[ii].x = tmp_x;
        enemy_shots[ii].y = tmp_y;
      }
    }
  }
}

/***************************************************************************************
   Update player shot.
 ****************************************************************************************/
void update_shot() {
  if (pl.is_shot && shot_interval == SHOT_INTERVAL_FRAME) {
    shot_interval = 0;
    for (int ii = 0; ii < PLAYER_SHOT_NUM; ii++) {
      if (pl_shots[ii].is_active == false) {
        pl_shots[ii].is_active = true;
        pl_shots[ii].x = pl.x + 4;
        pl_shots[ii].y = pl.y + 2;
        break;
      }
    }
  }

  // Move player shots.
  for (int ii = 0; ii < PLAYER_SHOT_NUM; ii++) {
    if (pl_shots[ii].is_active) {
      if (pl_shots[ii].x + 8 > 120) {
        pl_shots[ii].is_active = false;
      } else {

        // Do hit check.
        char is_hit = checkIsShotHit(ii);
        if (is_hit == NONE) {
          pl_shots[ii].x += 8;
        } else {
          score += game_level;
          pl_shots[ii].is_active = false;
          enemies[is_hit].is_receive_damage = true;
          enemies[is_hit].hitpoint--;

          // Beat enemy.
          if (enemies[is_hit].hitpoint <= 0) {
            score += (enemies[is_hit].h * enemies[is_hit].w * game_level) + (1000 - enemies[ii].frame);
            enemies[is_hit].is_active = false;
            // Check is boss enemy.
            if(enemies[is_hit].type == EN_MOV_BOSS1) {
              current_boss = NULL;
              set_game_mode(MODE_CLEAR);
            }
            shake_amount = random(enemies[is_hit].w/2, enemies[is_hit].w/2 + 4);

            // Show explosions.
            char rnd = random(1, 3);
            for (int ii = 0; ii < rnd; ii++) {
              Explosion *explosion = rent_explosion();
              if (explosion != NULL) {
                explosion->x = enemies[is_hit].x + enemies[is_hit].w/2;
                explosion->y = enemies[is_hit].y + enemies[is_hit].h/2;
                explosion->dest_radius = random(enemies[is_hit].w, enemies[is_hit].w + 10);
              }
            }
          }
        }
      }
    }
  }
}



/***************************************************************************************
   Update Background stars.
 ****************************************************************************************/
void update_bg() {
  if (frame % 2 == 0) {
    for (int ii = 0; ii < BG_STARS_NUM; ii++) {
      if (bg_stars[ii].x - 1 == 0) {
        bg_stars[ii].x = 128;
      } else {
        bg_stars[ii].x -= 1;
      }
    }
  }
}

void create_enemy_shot(int x, int y, char speed, int angle, unsigned char shot_type) {
  EnemyShot *es = rent_enemy_shot();
  if (es != NULL) {
    es->x = x * 100;
    es->y = y * 100;
    
    if (shot_type == EN_SHOT_STRAIGHT) {
      es->angle = angle * 100;
    } else if (EN_SHOT_TARGET) {
      es->angle = (atan2((pl.y + PLAYER_HEIGHT/2) - y, pl.x - x) * 180 / TINY_PI) * 100;
    }
  }
}

/***************************************************************************************
   Update Enemies
 ****************************************************************************************/
void update_enemy() {
  for (int ii = 0; ii < MAX_ENEMY_NUM; ii++) {

    // Find active enemies.
    if (enemies[ii].is_active) {

      // Check is in screen.
      if(enemies[ii].x < 0) {
        enemies[ii].is_active = false;
        continue;
      }

      // Check collision with player.
      if(pl.x + PLAYER_WIDTH/2 > enemies[ii].x && pl.x < enemies[ii].x + enemies[ii].w &&
         pl.y + PLAYER_HEIGHT/2 > enemies[ii].y && pl.y < enemies[ii].y + enemies[ii].h) {
          enemies[ii].is_active = false;
        die_player();    
        continue;
      }
      
      enemies[ii].frame++;

      float vx = 0;
      float vy = 0;
      switch (enemies[ii].type) {
        /****************************************************************
           ENEMY MOVE BY SIN CURVE.
         *****************************************************************/
        case EN_MOV_SIN:
          {
            float angle = enemies[ii].frame * (enemies[ii].y_speed * 0.1);
            vy = (enemies[ii].y_speed * 0.1) * sin(angle * TINY_PI / 180) * 0.1;
            vx = enemies[ii].x_speed * 0.1 * -1;
          }
          break;


        /****************************************************************
           ENEMY MOMVE BY ANGLE AND SPEED.
         *****************************************************************/
        case EN_MOV_TARGET:
        case EN_MOV_STRAIGHT:
          {
            float angle = enemies[ii].angle - 90;
            vx = cos(angle * TINY_PI / 180);
            vy = sin(angle * TINY_PI / 180);

            vx *= enemies[ii].x_speed * 0.1;
            vy *= enemies[ii].x_speed * 0.1;
          }
          break;

        /****************************************************************
           ENEMY MOMVE BOSS1.
         *****************************************************************/
        case EN_MOV_BOSS1:
        {
          if(enemies[ii].frame < 250) {
           vx -= 0.2;
          }

          // Move up to down.
          if(enemies[ii].frame == 500) {
            enemies[ii].shot_type = 1;
          }
          
          if(enemies[ii].frame > 500) {
            if(enemies[ii].frame % 120 == 0) 
            {
              enemies[ii].shot_type *= -1;
            }
            vy = (float)enemies[ii].shot_type * (0.1 * game_level);
          }
          
        }
        break;
      }

      enemies[ii].x += vx;
      enemies[ii].y += vy;

      // if too close enemy and player then dont shoot.
      float distance = abs((pl.y-enemies[ii].y) + (pl.x-enemies[ii].x));
      if(distance > 0.2) {

        // Normal enemy.
        if(enemies[ii].type != EN_MOV_BOSS1) {
          if (frame % 15 == 0 && random(0,100) < game_level*10) {
            create_enemy_shot(enemies[ii].x, enemies[ii].y + enemies[ii].h/2, game_level * 5, 180, enemies[ii].shot_type);
          }
        }
        // Boss enemy.
        else{

          // Step 1.
          if(enemies[ii].frame < 500) {
            if(enemies[ii].frame % 20 == 0) {
              create_enemy_shot(enemies[ii].x, enemies[ii].y + enemies[ii].h/2, game_level * 15, 180, EN_SHOT_TARGET);
            }
          }else{
            if(enemies[ii].frame % 20 == 0) {
              create_enemy_shot(enemies[ii].x, enemies[ii].y + enemies[ii].h/2, game_level * 15, 180, EN_SHOT_STRAIGHT);
              if(random(0, 100) < 10 * game_level) {
                create_enemy_shot(enemies[ii].x, enemies[ii].y + enemies[ii].h/2, game_level * 15, 180, EN_SHOT_TARGET);
              }else{
                create_enemy_shot(enemies[ii].x, enemies[ii].y + enemies[ii].h/2, game_level * 15, 200, EN_SHOT_STRAIGHT);
              }
              create_enemy_shot(enemies[ii].x, enemies[ii].y + enemies[ii].h/2, game_level * 15, 160, EN_SHOT_STRAIGHT);
            }
          }
        }
      }
    }
  }
}



/***************************************************************************************
   Update explosion objects.
 ****************************************************************************************/
void update_explosion() {
  for (int ii = 0; ii < EXPLOSION_NUM; ii++) {
    if (explosions[ii].is_active) {
      explosions[ii].radius++;
      if (explosions[ii].radius == explosions[ii].dest_radius) {
        explosions[ii].is_active = false;
      }
    }
  }
}


void update_player() {

  // If player already dead. do nothing.
  if (pl.is_active == false) {
    return;
  }

  bool isPressed = false;
  if (arduboy.pressed(DOWN_BUTTON)) {
    pl.vy = 1;
    isPressed = true;
  }
  if (arduboy.pressed(UP_BUTTON)) {
    pl.vy = -1;
    isPressed = true;
  }
  if (arduboy.pressed(LEFT_BUTTON)) {
    pl.vx = -1;
    isPressed = true;
  }
  if (arduboy.pressed(RIGHT_BUTTON)) {
    pl.vx = 1;
    isPressed = true;
  }
  if (arduboy.pressed(B_BUTTON)) {
    pl.is_shot = true;
    shot_interval++;
  } else {
    pl.is_shot = false;
    shot_interval = 0;
  }

  // Move player graphics.
  float vec_len = abs(pl.vx) + abs(pl.vy);
  if (isPressed && vec_len > 0) {
    pl.vx /= vec_len;
    pl.vy /= vec_len;
  }

  pl.vx *= 1.5;
  pl.vy *= 1.5;
  
  pl.x += pl.vx;
  pl.y += pl.vy;

  pl.vx *= 0.5;
  pl.vy *= 0.5;

  // Restrict player move area.
  if (pl.x < 0) {
    pl.x = 0;
  }
  if (pl.y > 64 - 8) {
    pl.y = 64 - 8;
  }
  if (pl.y < 0) {
    pl.y = 0;
  }
}

void update_enemy_timeline() {
  // char data = pgm_read_byte(&tetrimino[type][spin_index][ii][jj]);
  
  int length = sizeof(en_timeline) / sizeof(EnemyTimeline);
  int tmp_max_index = timeline_last_index;

  for(int ii=timeline_last_index;ii<length;ii++) {
    short target_frame = pgm_read_word(&en_timeline[ii].frame);

    if(target_frame <= frame) {
      tmp_max_index = ii+1;
      Enemy *en = rent_enemy();
      if(en != NULL) {
        en->x = pgm_read_word(&en_timeline[ii].x);
        en->y = pgm_read_word(&en_timeline[ii].y);
        en->w = pgm_read_word(&en_timeline[ii].w);
        en->h = pgm_read_word(&en_timeline[ii].h);
        en->hitpoint = pgm_read_word(&en_timeline[ii].hp);
        en->type = pgm_read_word(&en_timeline[ii].enemy_type);
        en->shot_type = pgm_read_word(&en_timeline[ii].enemy_shot_type);

        en->angle = pgm_read_word(&en_timeline[ii].angle);
        en->x_speed = pgm_read_word(&en_timeline[ii].x_speed);
        en->y_speed = pgm_read_word(&en_timeline[ii].y_speed);

        // If enemy mov is target. overwrite angle.
        if(en->type == EN_MOV_TARGET) {
          en->angle = (atan2(pl.y - PLAYER_HEIGHT / 2 - en->y, pl.x - en->x) * 180 / TINY_PI) + 90;
        }
        // If enemy mov is boss. set current_boss.
        if(en->type == EN_MOV_BOSS1) {
          current_boss = en;
        }
      }
    }
  }

  timeline_last_index = tmp_max_index;
}

void update_title() {

  if (arduboy.justPressed(LEFT_BUTTON)) {
    game_level--;
  }
  if (arduboy.justPressed(RIGHT_BUTTON)) {
    game_level++;
  }
  if(game_level < 1) {
    game_level = 1;
  }
  if(game_level > 10) {
    game_level = 10;
  }
  
  if (arduboy.justPressed(B_BUTTON)) {
    set_game_mode(MODE_INTRO);
    score = 0;
  }
}

void update_game_over() {
  if (arduboy.justPressed(B_BUTTON)) {
    initialize();
    set_game_mode(MODE_TITLE);
    
  }
}

void update_next_stage() {
  pl.x += 3;
  if(pl.x > 128) {
    game_level++;
    initialize();
    set_game_mode(MODE_INTRO);
  }
}

void update_intro() {
  if(frame > 90) {
    set_game_mode(MODE_GAME);
  }
}

void update() {

  arduboy.pollButtons();

  // Update pause status.
  if(game_mode == MODE_GAME && arduboy.justPressed(A_BUTTON)) {
    is_pause = !is_pause;
  }

  if(is_pause) {
    return;
  }

  // Move bg stars.
  update_bg();

  switch (game_mode) {
    case MODE_TITLE:
      update_title();
      break;

    case MODE_INTRO:
      update_intro();
      break;

    case MODE_GAME_OVER:
      update_game_over();
      break;

    case MODE_PLAYER_DIE:
      if (frame > 40) {
        set_game_mode(MODE_GAME_OVER);
      }
    case MODE_NEXT_STAGE:
    case MODE_CLEAR:
    case MODE_GAME:
      if(game_mode == MODE_NEXT_STAGE){
        update_next_stage();
      }
      if(game_mode == MODE_CLEAR && frame > 120) {
        set_game_mode(MODE_NEXT_STAGE);
      }

      // Spawn enemy.
      update_enemy_timeline();

      if (game_mode == MODE_GAME || game_mode == MODE_CLEAR) {
        // Move player.
        update_player();
      }

      // Move player shots.
      update_shot();

      // Move enemies.
      update_enemy();

      // Move enemy shots.
      update_enemy_shot();

      // Move Explosions.
      update_explosion();


      break;
  }

  // Update shake
  shake_amount--;
  if (shake_amount < 0) {
    shake_amount = 0;
  }
}

char checkIsShotHit(int index) {
  for (int ii = 0; ii < MAX_ENEMY_NUM; ii++) {
    if (enemies[ii].is_active) {
      
      for(int jj=0;jj<8;jj++) {
        if ((pl_shots[index].x-jj) + 8 > enemies[ii].x && (pl_shots[ii].x-jj) < enemies[ii].x + enemies[ii].w &&
            pl_shots[index].y + 8 > enemies[ii].y && pl_shots[ii].y < enemies[ii].y + enemies[ii].h) {
          return ii;
        }
      }
      
    }
  }
  return NONE;
}



/***************************************************************************
   Draw functions.
 ****************************************************************************/
void draw_bg() {
  // Draw bg.
  for (int ii = 0; ii < BG_STARS_NUM; ii++) {
    arduboy.drawPixel(shake_x + bg_stars[ii].x, shake_y + bg_stars[ii].y);
  }
}

void draw_player() {
  // Draw player.
  if (pl.is_active) {
    arduboy.drawRect(shake_x + pl.x, shake_y + pl.y, PLAYER_WIDTH, PLAYER_HEIGHT);
  }
}

void draw_shot() {
  // Draw player shot.
  for (int ii = 0; ii < PLAYER_SHOT_NUM; ii++) {
    if (pl_shots[ii].is_active) {
      arduboy.drawTriangle(shake_x + pl_shots[ii].x, shake_y + pl_shots[ii].y,
                           shake_x + pl_shots[ii].x, shake_y + pl_shots[ii].y + 4,
                           shake_x + pl_shots[ii].x + 5, shake_y + pl_shots[ii].y + 2);
    }
  }
}

void draw_enemy() {
  // Draw enemies.
  for (int ii = 0; ii < MAX_ENEMY_NUM; ii++) {
    if (enemies[ii].is_active) {
      if(enemies[ii].is_receive_damage) {
        arduboy.drawRect(shake_x + enemies[ii].x, shake_y + enemies[ii].y, enemies[ii].w, enemies[ii].h);
        enemies[ii].is_receive_damage = false;
      }else{
        arduboy.fillRect(shake_x + enemies[ii].x, shake_y + enemies[ii].y, enemies[ii].w, enemies[ii].h);
      }
    }
  }
}

void draw_enemy_shot() {
  // Draw enemy shots.
  for (int ii = 0; ii < ENEMY_SHOT_NUM; ii++) {
    if (enemy_shots[ii].is_active) {
      arduboy.fillCircle(shake_x + enemy_shots[ii].x * 0.01, shake_y + enemy_shots[ii].y * 0.01, 1);
    }
  }
}

void draw_explosion() {
  // Draw explosions.
  for (int ii = 0; ii < EXPLOSION_NUM; ii++) {
    if (explosions[ii].is_active) {
      arduboy.drawCircle(shake_x + explosions[ii].x, shake_y + explosions[ii].y, explosions[ii].radius);
    }
  }

}

void draw_title() {
  arduboy.setCursor(23, 20);
  arduboy.print(F("SHOOTING GAME"));
  arduboy.setCursor(23, 35);
  arduboy.print(F("PUSH A TO START"));
  arduboy.setCursor(35, 50);
  arduboy.print(F("< LEVEL "));
  arduboy.print((int)game_level);
  arduboy.print(F(" >"));
  draw_score(); 
}

void draw_game_over() {
  arduboy.setCursor(40, 20);
  arduboy.print(F("GAME OVER"));
  arduboy.setCursor(23, 40);
  arduboy.print(F("PUSH A TO RETRY"));
}

void draw_boss_indicate() {
  arduboy.setCursor(0, 0);
  arduboy.print(F("BOSS HP"));
  arduboy.drawFastHLine(45,2, (80.0 * ((float)current_boss->hitpoint / BOSS_HP)));
}

void draw_stage_clear() {
  arduboy.setCursor(30, 20);
  arduboy.print(F("LEVEL "));
  arduboy.print((int)game_level);
  arduboy.print(F(" CLEAR!"));
  arduboy.setCursor(20, 40);
  arduboy.print(F("MOVE TO NEXT STAGE"));
  
}

void draw_intro() {
  arduboy.setCursor(30, 20);
  arduboy.print(F("LEVEL "));
  arduboy.print((int)game_level);
  arduboy.print(F(" START"));
  
}

void draw_score() {
  arduboy.setCursor(0,0);
  arduboy.print(F("SC:"));
  arduboy.print(score);
  if(game_mode == MODE_TITLE) {
    arduboy.setCursor(0,10);
    arduboy.print(F("HI:"));
    arduboy.print(hi_score);
  }
}

void draw_pause() {
  arduboy.fillRect(34,15, 60, 30, 0);
  arduboy.drawRect(34,15, 60, 30, 1);
  
  arduboy.setCursor(49,27);
  arduboy.print(F("PAUSE"));
}

void draw() {

  int ii;

  // Create shake x,y.
  shake_x = random(-shake_amount, shake_amount);
  shake_y = random(-shake_amount, shake_amount);


  draw_bg();

  switch (game_mode) {
    case MODE_TITLE:
      draw_title();
      break;

    case MODE_INTRO:
      draw_intro();
      break;

    case MODE_GAME_OVER:
    case MODE_CLEAR:
    case MODE_NEXT_STAGE:
    case MODE_PLAYER_DIE:
    case MODE_GAME:

      if(game_mode == MODE_GAME_OVER){ 
        draw_game_over();
      }

      if(game_mode == MODE_CLEAR) {
        draw_stage_clear();
      }
      
      draw_player();

      draw_shot();

      draw_enemy();

      draw_enemy_shot();

      draw_explosion();

      if(current_boss != NULL) {
        draw_boss_indicate();
      }else{
        if(game_mode == MODE_GAME || game_mode == MODE_GAME_OVER) {
          draw_score();
        }
      }

      break;
  }


  // Check pause.
  if(is_pause) {
    draw_pause();
  }
}


// our main game loop, this runs once every cycle/frame.
// this is where our game logic goes.
void loop() {
  // pause render until it's time for the next frame
  if (!(arduboy.nextFrame()))
    return;

  // first we clear our screen to black
  if(game_mode != MODE_NEXT_STAGE) {
    arduboy.clear();
  }

  update();
  draw();

  arduboy.display();
  if(!is_pause) {
    frame++;
  }
}
