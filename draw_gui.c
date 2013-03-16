#include "draw_gui.h"
itemType* list_start;
struct menu_obj *mouse_objs;
int mouse_objs_length=0;

struct menu_obj* newPanel(char *name,int x,int y,int w, int h, Uint32 SDL_color)
{
    struct menu_obj* panel;
    panel=malloc(sizeof(struct menu_obj));
    panel->r.y=y;
    panel->r.x=x;
    panel->r.w=w;
    panel->r.h=h;
    panel->color=SDL_color;
    strcpy(panel->name, name);

    panel->objs_size=0;
    panel->overlay_objs=NULL;
    panel->mouselistener=false;
    panel->displayName=false;
    return panel;
}
struct menu_obj* newPanelMouse(char *name,int x,int y,int w, int h,bool mouselistener,Uint32 SDL_color)
{
    struct menu_obj* panel=newPanel(name, x, y, w,  h, SDL_color);
    panel->mouselistener=mouselistener;
    return panel;
}
struct menu_obj* newButton(char *name,int x,int y,int w, int h,bool mouselistener,Uint32 SDL_color)
{
    struct menu_obj* button=newPanel(name, x, y, w,  h, SDL_color);
    button->displayName=true;
    button->mouselistener=mouselistener;
}
// returns the List (-Start) pointing to the last added Item
itemType* addMenu(struct menu_obj *m_obj)
{
    itemType *currentItem=create_item_generic(m_obj->name,m_obj);
    insert_item(currentItem,&list_start);
}
void addMenu2Panel(struct menu_obj *panel, struct menu_obj *objs,int objs_size)
{
    panel->overlay_objs=realloc(panel->overlay_objs,objs_size*sizeof(struct menu_obj*));
    printf("%d\n",panel->overlay_objs);
    if(panel->overlay_objs!=NULL)
    {
        for(int i=0;i<objs_size-1;i++)
        {
            panel->overlay_objs++;
            objs++;
        }
        printf("%d %d \n",panel->overlay_objs,objs);
        panel->overlay_objs=objs;
        for(int i=0;i<objs_size-1;i++)
        {
            panel->overlay_objs--;
            objs--;
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
        if(m_obj->displayName)
        {
            int len=strlen(m_obj->name);
            drawS(m_obj->name , m_obj->r.x+m_obj->r.w/2-(len/2)*7 , m_obj->r.y+m_obj->r.h/2-font_size/2 , screen);
        }
        /* LARGE NUMBER IN m_obj->obj_size ?!?!*/

        for(int i=0;i<m_obj->objs_size;i++)
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
