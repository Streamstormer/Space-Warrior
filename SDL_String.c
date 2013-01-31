#include "SDL_String.h"
//Font stuff


TTF_Font *ttf_font;
/*
 * Font Stuff:
 * ttf_font has to be initialized at first use!
 * */
void setFont(char file[],int font_size)
{
    TTF_Font *ttf_font = TTF_OpenFont(file, font_size);
    if (ttf_font == NULL)
    {
        printf("Unable to load font: %s %s \n", file, TTF_GetError());
    }
}
/* Prints the given String to the Surface*/
void drawString(char* str, int r, int b, int g,
                int x, int y, int w, int h, SDL_Surface *screen)
{
    if (ttf_font != NULL)
    {
        SDL_Color foreground={r, b, g,0};
        SDL_Surface *text_surface = TTF_RenderText_Solid(ttf_font, str, foreground);
        //Setup the location on the screen to blit to
        SDL_Rect rect = { x, y, w, h };
        //Blit text_surface surface to the screen surface
        SDL_BlitSurface(text_surface, NULL, screen, &rect);
        //Free the text_surface surface
        SDL_FreeSurface(text_surface);
    }else printf("No Font loaded! Initialize Font before use.\n");
}
void drawS(char str[],int x,int y,SDL_Surface *screen)
{
    drawString(str,255,255,255,x,y,strlen(str)*15,20,screen);
}
