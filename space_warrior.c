/*  DHBW-space-warrior.c

    The final fight between good old C and object-orientation is going on.
	It's up to a few BA students to save the world from dominance of the expanding OO-empire.
    Shoot the evil OO-Spaceships, but do not hit your friendly C spaceships!
    Get ECTS (Extraterrestrial C training space) points for each OO-enemy destroyed,
	lose ECTS points for each C spaceship accidentally hit!

	TO DO: Have a look at the program and understand how it works.
	TO DO: With which key can the program be exited?
	TO DO: Get it compiled and run using the SDL library,
	       take into account your specific development environment
		   http://www.libsdl.org
    TO DO: There are some bugs, which need to be fixed:
	       - The ship is moving too slow, make it faster
		   - the enemy should shoot more frequently
		   - shots should be faster
		   - currently you can't be hit by the enemy shots, what's the problem?
    TO DO: Populate the space with more objects
	TO DO: Give the ships a different look (shape and colors)
	TO DO: Make your own speed changeable via the keyboard
	TO DO: Count the destroyed spaceships and the hits to the player's ship
	       and change the ECTS points accordingly.
	TO DO: Add a display area, where the hit counts are displayed.
	TO DO: Make the motions of enemies and friends more interesting.
	TO DO: Make the enemy shots more dangerous by targeting your position
	TO DO: Introduce an energy meter, which is reduced each time you are hit.
    TO DO: The game should be finished, when there are no more enemy-objects left
	       or when you run out of energy.
	TO DO: Use SDL audio functions to add sound effects.
	TO DO: Use the SDL Bitmap loader to provide more interesting graphics.
    TO DO: Split the source code into separate files to make it more maintainable,
	       adapt your build/make configuration accordingly.
    TO DO: Add different game levels with varying difficulty
	TO DO: Make this an interesting, original, feature-rich gaming experience ...
*/

#include <stdio.h>
#include <stdlib.h>
#include "SDL/SDL.h"
#include <SDL/SDL_ttf.h>
#include <time.h>
#include <stdbool.h>
#include <tgmath.h>
#include "SDL_String.h"
#include "chained_list.h"
#include "draw_gui.h"
#include <SDL/SDL_mixer.h>

// Screen resolution
int width, height;

// Different type of objects in space
enum obj_type_enum
{
    spc_obj_star=0, spc_obj_shot=1, spc_obj_enemy=2,
    spc_obj_friend=3, spc_obj_enemy_shot=4, spc_obj_explosion=5,
    spc_obj_type_cnt=6, menu_obj_=7         // This is not an object, but denotes the number of types
};

// Number of objects  (Remaining are stars)
#define MAX_OBJ 2500
#define MAX_SHOT 50
#define MAX_ENEMY 50
#define MAX_FRIEND 50
#define MAX_ENEMY_SHOT 50
#define MAX_MENU_OBJ 1
#define HEALTH_PER_HIT 2
#define REGENERATION 100
#define REGENERATION_STEP 1
#define GUN_HEAT_STEPS 10
#define GUN_HEATREGEN_STEPS 5

// Structure that defines space object instances
struct spc_obj
{
    enum obj_type_enum type;    // Type of object
    char active;                // Flag, whether object is active
    float x, y, z;              // Coordinates in Space
    float vx, vy, vz;           // Speed
};

// Object array
struct spc_obj obj[MAX_OBJ];

int shield_hit, ects,cnt=0, life=100, kills=0, gun_heat=100;
bool esc=false, paused=false, mouse=false;
// Init object instances
void init_object_instances()
{
    int i;
    enum obj_type_enum t;

    for(i=0; i<MAX_OBJ; i++)
    {
        // Use different array areas for different object types
        if(i<MAX_SHOT)      t=spc_obj_shot;
        else if(i<MAX_SHOT+MAX_ENEMY_SHOT) t=spc_obj_enemy_shot;
        else if(i<MAX_SHOT+MAX_ENEMY_SHOT+MAX_ENEMY)  t=spc_obj_enemy;
        else if(i<MAX_SHOT+MAX_ENEMY_SHOT+MAX_ENEMY+MAX_FRIEND)  t=spc_obj_friend;
        else if(i<MAX_SHOT+MAX_ENEMY_SHOT+MAX_ENEMY+MAX_FRIEND+MAX_MENU_OBJ) t=menu_obj_;
        else t=spc_obj_star;

        obj[i].type=t;
        obj[i].active=1;   // No active shot
        if(t!=menu_obj_)
        {
            // Random positions
            obj[i].x=(float)(rand()%10000-5000);
            obj[i].y=(float)(rand()%10000-5000);
            obj[i].z=(float)(rand()%10000);
            obj[i].vx=obj[i].vy=obj[i].vz=0;
            if(obj[i].type==spc_obj_enemy || obj[i].type==spc_obj_friend)
            {
                obj[i].vx=(float)((rand()%40-20)/20.);
                obj[i].vy=(float)((rand()%40-20)/20.);
                obj[i].vz=(float)(rand()%10-5);
            }
        }else
        {
            obj[i].x=200;
            obj[i].y=200;
            obj[i].z=1;
            obj[i].vx=1;
            obj[i].vy=1;
            obj[i].vz=1;
            obj[i].active=1;
        }
    }
    shield_hit=0;
    ects=0;
}

// Initialize Shot
void init_shot()
{
    static float shot_pos_x=50;
    int i;

    if(gun_heat>10)
    {
        for(i=0; i<MAX_SHOT; i++)
        {
            if(!obj[i].active)
            {
                obj[i].active=1;
                shot_pos_x=-shot_pos_x;
                obj[i].x=shot_pos_x;
                obj[i].y=100;
                obj[i].z=100;
                obj[i].vz=10;
                obj[i].vx=obj[i].vy=0;
                gun_heat-=(gun_heat-GUN_HEAT_STEPS>0 ? GUN_HEAT_STEPS : gun_heat);
                break;
            }
        }
    }
}
//makes no sense!
void init_friend_shot(int friend_)
{
    for(int i=0; i<MAX_SHOT; i++)
    {
        if(!obj[i].active)
        {
            obj[i].active=1;
            obj[i].x=obj[friend_].x;
            obj[i].y=obj[friend_].y;
            obj[i].z=obj[friend_].z;
            float myvx=1000000,myvy=1000000;
            int help2=0, help3=1000000;
            int enemy=0;
            for(int j=MAX_SHOT+MAX_ENEMY_SHOT; j<MAX_SHOT+MAX_ENEMY_SHOT+MAX_ENEMY; j++)
            {
                help2=sqrt(pow(obj[j].x,2)+pow(obj[j].y,2)+pow(obj[j].z,2));
                if(help2<1000)
                {
                    if(help2<help3)
                    {
                        myvx=obj[j].x;
                        myvy=obj[j].y;
                        enemy=j;
                    }
                }
                help3=help2;
            }
            //set shot direction in range of the player
            float dx=-(obj[friend_].x-myvx);
            float dy=-(obj[friend_].y-myvy);
            float help=(10/sqrt(pow(dx,2)+pow(dy,2)));
            obj[i].vx=dx*help;
            obj[i].vy=dy*help;
            if(obj[friend_].z-obj[enemy].z < 0)
            {
                obj[i].vz= 10;
            }else
            {
                obj[i].vz=-10;
            }
            obj[i].z= obj[i].z+obj[i].vz;
            break;
        }
    }
}
void init_enemy_shot(int enemy,int myvx,int myvy)
{
    int i;

    for(i=MAX_SHOT; i<MAX_SHOT+MAX_ENEMY_SHOT; i++)
    {
        if(!obj[i].active)
        {
            obj[i].active=1;
            obj[i].x=obj[enemy].x;
            obj[i].y=obj[enemy].y;
            obj[i].z=obj[enemy].z;
            obj[i].vz=-10;

            int help2=0, help3=1000000;
            if(rand()%2)for(int j=MAX_SHOT+MAX_ENEMY_SHOT+MAX_ENEMY; j<MAX_SHOT+MAX_ENEMY_SHOT+MAX_ENEMY+MAX_FRIEND; j++)
            {
                help2=sqrt(pow(obj[j].x,2)+pow(obj[j].y,2)+pow(obj[j].z,2));
                if(help2<5000)
                {
                    if(help2<help3)
                    {
                        myvx=obj[j].x+rand()%20;
                        myvy=obj[j].y+rand()%20;
                    }
                }
                help3=help2;
            }
            //set shot direction in range of the player
            float dx=-(obj[enemy].x-myvx);
            float dy=-(obj[enemy].y-myvy);
            float help=(10/sqrt(pow(dx,2)+pow(dy,2)));
            obj[i].vx=dx*help;
            obj[i].vy=dy*help;
            break;
        }
    }
}
void move_object_instances(float myvx, float myvy)
{
    int i, j;

    if(shield_hit>0)
    {
        shield_hit--;
    }
    for(i=0; i<MAX_OBJ; i++)
        if(obj[i].active)
        {
            // Move object
            obj[i].x+=(float)(obj[i].vx+myvx*obj[i].z/10000.);
            obj[i].y+=(float)(obj[i].vy+myvy*obj[i].z/10000.);
            obj[i].z+=obj[i].vz-7;
            // Object hits player's ship?
            if(obj[i].type!=spc_obj_star && abs((int)obj[i].x)<100 && abs((int)obj[i].y)<100 &&
                    obj[i].z<50)
            {
                shield_hit+=10;
                life-=(life-HEALTH_PER_HIT>0 ? HEALTH_PER_HIT : life);
            }
            // Init object, if disappeared
            if(obj[i].z<=0)     // Object disappeared behind viewer
            {
                if(obj[i].type==spc_obj_enemy_shot || obj[i].type==spc_obj_explosion)
                    obj[i].active=0;
                else
                {
                    obj[i].z+=10000.;
                    obj[i].x=(float)(rand()%10000-5000);
                    obj[i].y=(float)(rand()%10000-5000);
                }
            }
            // Enemy can shoot
            if(obj[i].type==spc_obj_enemy && (rand()%51<rand()%25))
            {
                if(cnt==0 || cnt==100)
                {
                    init_enemy_shot(i,myvx+rand()%15,myvy+rand()%15);
                }
            }
            // Stop shot at end of range
            if(obj[i].z>8000 && (obj[i].type==spc_obj_shot || obj[i].type==spc_obj_enemy_shot))
            {
                obj[i].active=0;
            }
            // check if shot hits anything
            if(obj[i].type==spc_obj_shot || obj[i].type==spc_obj_enemy_shot)
            {
                for(j=0; j<MAX_OBJ; j++)
                    if(obj[j].active &&
                            ((obj[i].type==spc_obj_shot && obj[j].type==spc_obj_enemy) ||
                             (obj[i].type==spc_obj_enemy_shot && obj[j].type==spc_obj_friend)))
                        if(abs((int)(obj[j].x-obj[i].x))<50 &&
                                abs((int)(obj[j].y-obj[i].y))<50 &&
                                abs((int)(obj[j].z-obj[i].z))<250)    // Hit
                        {
                            obj[j].type=spc_obj_explosion;
                            if(obj[i].type==spc_obj_shot)kills++;
                            //kick the object a bit in the direction of the shot..
                            obj[j].vx=obj[j].vx+(obj[i].vx/3);
                            obj[j].vy=obj[j].vy+(obj[i].vy/3);
                            obj[j].vz=obj[j].vz+(obj[i].vz/3);
                            //kill the shot
                            obj[i].active=0;
                        }
            }
        }

}
void DrawObject(SDL_Surface *screen, float z, int sx, int sy, int type)
{
    // Ship colors
    static unsigned char c[][12]=
    {
        { 0, 0, 0, 255, 255, 255, 128, 128, 128, 64, 64, 64 },
        { 0, 0, 0, 255, 0, 0,     255, 255, 0,   255, 128, 64 },
        { 0, 0, 0,  0, 192, 192, 100, 100, 120, 200, 200, 240 },
        { 0, 0, 0,  128, 255, 128, 192, 220, 192,  255, 255, 0 },
        { 0, 0, 0, 255, 0, 255,     128, 128, 255,   255, 0, 128 },
        { 0, 0, 0, 255, 0, 0,     255, 255, 0,   255, 128, 64 },
        { 0, 0, 0, 255, 0, 0,     255, 255, 0,   255, 128, 64 }
    };
    static char menu[][20]=
    {
        "22222222222222222222",
        "21111111111111111112",
        "21111111111111111112",
        "21111133333333311112",
        "21111133333333311112",
        "21111133333333311112",
        "21111133333333311112",
        "21111111111111111112",
        "21111111111111111112",
        "22222222222222222222"
    };
    // Ship shapes 8 Lines each
    static char shape[][20]=
    {
        "                    ",
        "      1      1      ",
        "      1122 211      ",
        " 13   12233221   31 ",
        "  2222222332222222  ",
        "       22 222       ",
        "      3      3      ",
        "                    ",

        "     1111111111     ",
        "   11111     11111  ",
        "  111          111  ",
        "  111               ",
        "   11111     11111  ",
        " 222211111111122222 ",
        "22222332222223322222",
        "   22233333333222   "
    };

    static int obj_size[]= { 5, 5,  40, 40, 120, 80, 120,80, 40, 40, 200, 200, 100, 100, 100 };

    Uint32 *bufp;
    Uint32 color[4];
    int i, x, y, dist, size_x, size_y;
    char v;

    size_x=(int)(256*obj_size[type*2]/z);
    size_y=(int)(256*obj_size[type*2+1]/z);

    dist=(255-z*255/15000);
    for(i=0; i<4; i++)
    {
        color[i]=SDL_MapRGB(screen->format, dist*c[type][i*3]/256, c[type][i*3+1]*dist/256, c[type][i*3+2]*dist/256);
    }
    if(type==0) size_x=size_y=1;

    if(sx-size_x>=0 && sx+size_x<width)
    {
        for(y=-size_y; y<=size_y; y++)
            if(sy+y>=0 && sy+y<height)
            {
                bufp = (Uint32 *)screen->pixels + (y+sy)*screen->pitch/4 + sx-size_x;
                for(x=-size_x; x<=size_x; x++)
                {
                    if(type<2 || type==4 || type==5)
                    {
                        if(type!=5)
                        {
                            if(x<5&&x>-5)
                            {
                                if(x*x+y*y<size_y*size_y) *bufp++ = color[type? 2:1];
                                    else bufp++;
                            }else bufp++;
                        }else
                        {
                            if(x*x+y*y < (size_y*size_y)*(rand()%4346)/6000)
                                *bufp++ = color[rand()&3];
                            else
                                bufp++;
                        }
                    }
                    else
                    {
                        if(type!=7)
                        {
                            v=shape[(y+size_y)*8/(size_y+size_y+1)+(type==2? 0:8)][(x+size_x)*20/(size_x+size_x+1)];
                            if(v==' ') bufp++;
                                else *bufp++ = color[v&3];
                        }else
                        {
                            v=menu[(y+size_y)*8/(size_y+size_y+1)+0][(x+size_x)*20/(size_x+size_x+1)];
                            //if(v==' ') bufp++;
                            *bufp++ = color[5];
                        }
                    }
                }
            }
    }
}
void draw_object_instances(SDL_Surface *screen)
{
    int i, c;
    SDL_Rect dest;
    Uint32 color;

    dest.x=0;
    dest.y=0;
    dest.w=width;
    dest.h=height;

    c=shield_hit;
    if(c>255) c=255;
    color=SDL_MapRGB(screen->format, c, c/4, c/4);
    SDL_FillRect(screen, &dest, color);

    for(i=0; i<MAX_OBJ; i++)
    {
        if(obj[i].active)
        {
            if(obj[i].type!=menu_obj_)
            {
                DrawObject(screen,obj[i].z, (int)(width/2+width*obj[i].x/obj[i].z),
                    (int)(height/2+width*obj[i].y/obj[i].z), (int)(obj[i].type));
            }else
            {
                DrawObject(screen,obj[i].z, obj[i].x,obj[i].y, (int)(obj[i].type));
            }
        }
    }
}
void draw_interface_objects(SDL_Surface *screen)
{
    Uint32 color;

    //life beam
    SDL_Rect r;
    r.y=10;
    r.w=life*1.5,r.h=20;
    r.x=width/2-r.w/2;
    color=SDL_MapRGB(screen->format, life*2.5, 0, 0);
    SDL_FillRect(screen,&r,color);
    char c2[5];
    sprintf(c2,"%d%c",life,'%');
    drawS(c2,r.x+r.w/2-15,r.y+r.h/2-8,screen);
    if(shield_hit==0&&!paused)
    {
        if(life+REGENERATION_STEP-1<REGENERATION)
        {
            if(cnt==0)
            {
                life+=REGENERATION_STEP;
            }
        }else
        {
            life=REGENERATION;
        }
    }
    //kills
    sprintf(c2,"%d",kills);
    drawS(c2,width-100,50,screen);
    //Gun-Heat
    r.y=height-39;
    r.w=gun_heat*1.5,r.h=20;
    r.x=width/2-r.w/2;
    color=SDL_MapRGB(screen->format, 255-(gun_heat*2.5), gun_heat*2.5, 0);
    if(!cnt)
    {
        if(gun_heat+GUN_HEATREGEN_STEPS-1<100)gun_heat+=GUN_HEATREGEN_STEPS;
            else gun_heat=100;
    }
    SDL_FillRect(screen,&r,color);
    //Target
    r.y=height/2;
    r.x=width/2-25;
    r.w=50,r.h=1;
    color=SDL_MapRGB(screen->format, 150, 0, 0);
    SDL_FillRect(screen,&r,color);
    r.y=height/2-25;
    r.x=width/2;
    r.w=1,r.h=50;
    color=SDL_MapRGB(screen->format, 150, 0, 0);
    SDL_FillRect(screen,&r,color);
    //mouse_control(&vx,&vy,screen);
    //increment count
    cnt++;
    cnt=cnt%200;
}
void draw_menu(SDL_Surface *screen)
{
    if(list_start==NULL)
    {
        struct menu_obj *panel;
        panel=newPanel("1",width/2-150,height/2-200,300,400,SDL_MapRGB(screen->format, 0, 0, 150));
        addMenu(panel);

        struct menu_obj *m_obj=newButton("ClickMe!",width/2-50,height/2-100,100,200,true,SDL_MapRGB(screen->format, 100, 0, 0));
        addMenu2Panel(panel,m_obj,1);
        add2MouseListener(m_obj);

        struct menu_obj *m_obj2=newButton("Exit!",width/2-100,height/2+100,200,50,true,SDL_MapRGB(screen->format, 100, 100, 0));
        addMenu2Panel(panel,m_obj2,2);
        add2MouseListener(m_obj2);
    }
    draw_menu_objects(screen,*list_start);
}
void mouseClicked(struct menu_obj *button,SDL_Surface *screen)
{
    printf("%s clicked! \n",mouse_objs->name);
    if(!strcmp(button->name,"ClickMe!"))
    {
        mouse_objs->color=SDL_MapRGB(screen->format, 0, 150, 0);
    }
    if(!strcmp(button->name,"Exit!"))
    {
        exit(0);
    }
}
void mouse_control(float *vx, float *vy, SDL_Surface *screen,SDL_Event *event)
{
    if(mouse)
    {
        if(!paused)
        {
            *vx=(width/2)-event->motion.x;
            *vy=(height/2)-event->motion.y;
        }
        for(int i=0;i<mouse_objs_length;i++)
        {
            if(mouse_objs->mouselistener)
            {
                if(event->type==SDL_MOUSEBUTTONDOWN)
                    if(event->button.x > mouse_objs->r.x && event->button.x < (mouse_objs->r.x+mouse_objs->r.w) &&
                   event->button.y > mouse_objs->r.y && event->button.y < (mouse_objs->r.y+mouse_objs->r.h))
                {
                    mouseClicked(mouse_objs,screen);
                }
            }
            if(i<mouse_objs_length-1)mouse_objs++;
        }
        for(int i=0;i<mouse_objs_length-1;i++)mouse_objs--;

    }
}
int key_control(float *vx, float *vy,SDL_Event *event)
{
    if(event->type==SDL_KEYDOWN)
    {
        switch(event->key.keysym.sym)
        {
        case SDLK_LEFT:
            *vx=10;
            break;
        case SDLK_RIGHT:
        {
            *vx=-10;
            break;
        }
        case SDLK_UP:
            *vy=10;
            break;
        case SDLK_DOWN:
            *vy=-10;
            break;
        case SDLK_SPACE:
            init_shot();
            break;
        case SDLK_ESCAPE:
        {
            if(paused)
            {
                paused=false;
                mouse=false;
            }else
            {
                paused=true;
                mouse=true;
            }
            break;
        }
        default:
            break;
        }
    }
    else if(event->type==SDL_KEYUP)
    {
        switch(event->key.keysym.sym)
        {
        case SDLK_LEFT:
            *vx=0;
            break;
        case SDLK_RIGHT:
            *vx=0;
            break;
        case SDLK_UP:
            *vy=0;
            break;
        case SDLK_DOWN:
            *vy=0;
            break;
        default:
            break;
        }
    }
    return true;
}
int event_control(float *vx, float *vy, SDL_Surface *screen)
{
    SDL_Event event;
    SDL_PollEvent(&event);
    switch(event.type)
    {
        case SDL_MOUSEBUTTONDOWN:
        //case SDL_MOUSEBUTTONUP:
        case SDL_MOUSEMOTION:
            mouse_control(vx,vy,screen,&event);
            break;
        case SDL_KEYDOWN:
        case SDL_KEYUP:
            return key_control(vx,vy,&event);
        case SDL_QUIT:
            return 0;
        default: break;
    }
    return 1;

}
void cleanUp(Mix_Music *play_sound)
{
     Mix_FreeMusic(play_sound);
     Mix_CloseAudio();
     SDL_Quit();
}
// main
int main(int argc, char *argv[])
{
    float vx=0, vy=0;
    //init SDL
    SDL_Surface *screen;
    if ( SDL_Init(SDL_INIT_AUDIO|SDL_INIT_VIDEO) < 0 )
    {
        fprintf(stderr, "Unable to init SDL: %s\n", SDL_GetError());
        return EXIT_FAILURE;
    }
    atexit(SDL_Quit);
    #ifdef __WIN32__
     freopen("CON","w",stdout);
     freopen("CON","w",stderr);
    #endif


    //init SDL_ttf
    if (TTF_Init() == -1)
    {
        printf("Unable to initialize SDL_ttf: %s \n", TTF_GetError());
        return EXIT_FAILURE;
    }else setFont("arial.ttf", 14);
    if(ttf_font == NULL)
    {
        fprintf(stderr, "Could not load Font: %s \n", TTF_GetError());
        return EXIT_FAILURE;
    }

    // Initialize screen
    width=1024;
    height=768;
    screen = SDL_SetVideoMode(width, height, 32, SDL_HWSURFACE |  SDL_DOUBLEBUF | SDL_FULLSCREEN);
    if( screen == NULL )
    {
        fprintf(stderr, "Unable to init video: %s\n", SDL_GetError());
        return EXIT_FAILURE;
    }

    //Audio
    //Mix_OpenAudio(22050, MIX_DEFAULT_FORMAT, 2, 4096);
    //Mix_Music* play_sound = Mix_LoadMUS("music.wav");
    //Mix_PlayMusic(play_sound, -1);

    //init
    init_object_instances();
    bool running=true;
    while(running)
    {
        running=event_control(&vx, &vy,screen);
        while(!paused&&life>0)
        {
            draw_object_instances(screen);
            draw_interface_objects(screen);
            event_control(&vx, &vy,screen);
            move_object_instances(vx, vy);
            //flip
            SDL_Flip(screen);
        }
        draw_object_instances(screen);
        draw_interface_objects(screen);
        if(paused)
        {
            draw_menu(screen);
        }
        //flip
        SDL_Flip(screen);
    }
    //cleanUp audio and exit.
    //cleanUp(play_sound);
    return 0;
}
