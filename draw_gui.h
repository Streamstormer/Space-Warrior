#ifndef DRAW_GUI_H_INCLUDED
#define DRAW_GUI_H_INCLUDED

#include <SDL/SDL.h>
#include <SDL/SDL_ttf.h>
#include <stdbool.h>
#include "chained_list.h"
#include "SDL_String.h"
/*
 *  Remember to set Pointer=NULL before you use realloc()
 */
struct menu_obj
{
    char name[50];
    SDL_Rect r;
    Uint32 color;
    int objs_size;
    bool mouselistener;
    bool displayName;
    struct menu_obj *overlay_objs;
};

extern itemType* list_start;
extern struct menu_obj *mouse_objs;
extern int mouse_objs_length;

struct menu_obj* newPanel(char *name,int x,int y,int w, int h,Uint32 SDL_color);
struct menu_obj* newPanelMouse(char *name,int x,int y,int w, int h,bool mouselistener,Uint32 SDL_color);
struct menu_obj* newButton(char *name,int x,int y,int w, int h,bool mouselistener,Uint32 SDL_color);
// returns the List (-Start) pointing to the last added Item
itemType* addMenu(struct menu_obj *m_obj);
void addMenu2Panel(struct menu_obj *panel, struct menu_obj *objs,int objs_size);
void draw_menu_object(SDL_Surface *screen, struct menu_obj *m_obj);
void draw_menu_objects(SDL_Surface *screen, itemType list_ptr);
void add2MouseListener(struct menu_obj *obj);
#endif // DRAW_GUI_H_INCLUDED
