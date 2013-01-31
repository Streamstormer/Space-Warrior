// space_warrior.c

#include <stdio.h>
#include <stdlib.h>
#include "SDL/SDL.h"
#include <SDL/SDL_ttf.h>
#include <time.h>
#include <stdbool.h>
#include <tgmath.h>
#include "SDL_String.h"
#include "chained_list.h"
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

#define withaudio 0
// Structure that defines space object instances
struct spc_obj
{
    enum obj_type_enum type;    // Type of object
    char active;                // Flag, whether object is active
    float x, y, z;              // Coordinates in Space
    float vx, vy, vz;           // Speed
};
struct menu_obj
{
    char *name;
    SDL_Rect r;
    Uint32 color;
    int objs_size;
    bool mouselistener;
    struct menu_obj *overlay_objs;
};
// Object array
struct spc_obj obj[MAX_OBJ];

int shield_hit, ects,cnt=0, life=100, kills=0, gun_heat=100,mouse_objs_length=0;
bool esc=false, paused=false, mouse=false;
itemType* list_start;
struct menu_obj *mouse_objs;
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
    if(shield_hit==0)
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

// returns the List (-Start) pointing to the last added Item
itemType* addMenu(struct menu_obj *m_obj)
{
    itemType *currentItem=create_item_generic(m_obj->name,m_obj);
    insert_item(currentItem,&list_start);
    return currentItem; // Why no return!?
}
void addMenu2Panel(struct menu_obj *panel, struct menu_obj *objs,int objs_size)
{
    panel->overlay_objs=malloc(objs_size*sizeof(struct menu_obj));
    printf("%p\n",panel->overlay_objs);
    if(panel->overlay_objs!=NULL)
    {
        for(int i=0;i<objs_size;i++)
        {
            printf("%p %p \n",panel->overlay_objs[i],objs[i]);  // Ignor Warning. That is a pointer!
            panel->overlay_objs[i]=objs[i];
        }
        panel->objs_size=objs_size;
        printf("%d, %d\n",objs_size,panel->objs_size);
    }else
    {
        printf("unable to allocate memmory \n");
        panel->objs_size=0;
    }
}
void draw_menu_object(SDL_Surface *screen, struct menu_obj *m_obj)
{
            //printf("asdf\n");
    if(!(m_obj->r.x==0&&m_obj->r.y==0&&m_obj->r.w==0&&m_obj->r.h==0)&&m_obj->r.x>=0&&m_obj->r.y>=0)
    {
        SDL_FillRect(screen,&(m_obj->r),m_obj->color);
        //printf("%d\n",m_obj);
        //printf("%d\n",m_obj->objs_size);
        //printf("%d/%d %d/%d\n",m_obj->r.x,m_obj->r.y,m_obj->r.w,m_obj->r.h);

        /* LARGE NUMBER IN m_obj->obj_size ?!?!*/

        for(int i=0;i<(m_obj->objs_size>20?0:m_obj->objs_size);i++)
        {
            if(m_obj->overlay_objs!=NULL)draw_menu_object(screen,m_obj->overlay_objs+i);
        }
    }
            //printf("asdf2\n");
}
void draw_menu_objects(SDL_Surface *screen, itemType list_ptr)
{
    itemType *help_ptr=NULL;
    help_ptr=&list_ptr;
    int i=0;
    while(help_ptr!=NULL)
    {
        i++;
        draw_menu_object(screen,help_ptr->data);
        help_ptr=help_ptr->next_item;
    }
}
void add2MouseListener(struct menu_obj *obj)
{
    mouse_objs_length++;
    mouse_objs=realloc(mouse_objs,mouse_objs_length*sizeof(struct menu_obj));
    for(int i=0;i<mouse_objs_length-1;i++)mouse_objs++;
    mouse_objs=obj;
    for(int i=0;i<mouse_objs_length-1;i++)mouse_objs--;

}
void draw_menu(SDL_Surface *screen)
{
    if(list_start==NULL)
    {
        struct menu_obj *panel;
        panel=malloc(1*sizeof(struct menu_obj));
        panel->name="1";
        panel->r.y=height/2-200;
        panel->r.x=width/2-150;
        panel->r.w=300;
        panel->r.h=400;
        panel->objs_size=0;
        panel->mouselistener=false;
        panel->color=SDL_MapRGB(screen->format, 0, 0, 150);

        addMenu(panel);

        struct menu_obj *m_obj;
        m_obj=malloc(1*sizeof(struct menu_obj));
        m_obj->name="2";
        m_obj->r.y=height/2-100;
        m_obj->r.x=width/2-50;
        m_obj->r.w=100;
        m_obj->r.h=200;
        panel->objs_size=0;
        m_obj->mouselistener=true;
        m_obj->color=SDL_MapRGB(screen->format, 100, 0, 0);

        add2MouseListener(m_obj);
        addMenu2Panel(panel,m_obj,1);
    }
    draw_menu_objects(screen,*list_start);
}
void mouse_control(float *vx, float *vy, SDL_Surface *screen,SDL_Event *event)
{
    if(mouse)
    {
        *vx=(width/2)-event->motion.x;
        *vy=(height/2)-event->motion.y;
        for(int i=0;i<mouse_objs_length;i++)
        {
            if(mouse_objs->mouselistener)
            {
                if(event->button.x > mouse_objs->r.x && event->button.x < (mouse_objs->r.x+mouse_objs->r.w) &&
                   event->button.y > mouse_objs->r.y && event->button.y < (mouse_objs->r.y+mouse_objs->r.h))
                {
                    printf("%s\n",mouse_objs->name);
                    mouse_objs->color=SDL_MapRGB(screen->format, 0, 150, 0);
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
    return 1;
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
            return false;
        default: break;
    }
    return true;

}
void cleanUp(Mix_Music *play_sound)
{
     Mix_FreeMusic(play_sound);
     Mix_CloseAudio();
     SDL_Quit();
}
// The Windows SDL Lib needs argc and argv... WHY!?
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
        fprintf(stderr, "Unable to initialize SDL_ttf: %s \n", TTF_GetError());
        return EXIT_FAILURE;
    }else ttf_font = TTF_OpenFont("Ubuntu.ttf", 14);

    if(ttf_font == NULL)
    {
        fprintf(stderr, "Could not load Font: %s \n", TTF_GetError());
        return EXIT_FAILURE;
    }

    // Initialize screen
    width=1024;
    height=768;
    screen = SDL_SetVideoMode(width, height, 32, SDL_HWSURFACE |  SDL_DOUBLEBUF );
    if( screen == NULL )
    {
        fprintf(stderr, "Unable to init video: %s\n", SDL_GetError());
        return EXIT_FAILURE;
    }

#if withaudio
    Mix_OpenAudio(22050, MIX_DEFAULT_FORMAT, 2, 4096);
    Mix_Music* playMusic = Mix_LoadMUS("music.wav");
    if(playMusic == NULL)
        fprintf(stderr, "Could not Load Music-File: %s \n", Mix_GetError());

    Mix_PlayMusic(play_sound, -1);
#endif

    //init
    init_object_instances();

    bool running=true;
    while(running)
    {
        running=event_control(&vx, &vy,screen);
        while(!paused && life>0)
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

#if withaudio
    cleanUp audio and exit.
    cleanUp(play_sound);
#endif
}
