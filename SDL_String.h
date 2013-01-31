#ifndef SDL_STRING_H_INCLUDED
#define SDL_STRING_H_INCLUDED

#include <SDL/SDL.h>
#include <SDL/SDL_ttf.h>


extern TTF_Font *ttf_font;
/*
 * Font Stuff:
 * ttf_font has to be initialized at first use!
 * */
void setFont(char file[],int font_size);


/* Prints the given String to the Surface*/
void drawString(char* str, int r, int b, int g,
                int x, int y, int w, int h, SDL_Surface *screen);
/* Print String with standard Font. */
void drawS(char str[],int x,int y,SDL_Surface *screen);

#endif // SDL_STRING_H_INCLUDED
