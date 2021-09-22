#ifndef _PAT
#define _PAT

//-----------------------------------------------------
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>

#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

//-----------------------------------------------------
typedef int32_t Int32;
typedef int16_t Int16;
typedef int8_t Int8;

#define BIT(x) (1 << (x))

#define SQR(x) ((x) * (x))

#define SIGN(x) ((x) < 0.0 ? -1 : 1)

#define FLOOR(x) floor ((x))

#define IFLOOR(x) (int)floor ((x))

#define CLAMP(x, lo, hi) ((x) < (lo) ? (lo) : ((x > hi) ? (hi) : (x)))

#define SCALE(x, inlo, inhi, outlo, outhi)                                    \
  (((x) - (inlo)) * ((outhi) - (outlo)) / ((inhi) - (inlo)) + (outlo))

#define MIN(a, b) ((a) < (b) ? (a) : (b))
#define MAX(a, b) ((a) > (b) ? (a) : (b))

#define PI (3.14159265358979323846f)
#define TAU (2.0f * PI)

//-----------------------------------------------------
#define PAT_PIXEL_FORMAT SDL_PIXELFORMAT_RGBA4444
#define PAT_Color Uint16
extern const size_t PAT_COLOR_SIZE;

#define PAT_STATIC_SPRITE SDL_TEXTUREACCESS_STATIC
#define PAT_TARGET_SPRITE SDL_TEXTUREACCESS_TARGET
#define PAT_STREAMING_SPRITE SDL_TEXTUREACCESS_STREAMING

//-----------------------------------------------------
typedef enum
{
  PAT_OK = 0,
  PAT_ERR = -1
} PAT_STATUS;

enum
{
  PAT_TRANSPARENT,

  PAT_BLACK,
  PAT_WHITE,

  PAT_RED,
  PAT_GREEN,
  PAT_BLUE,

  PAT_YELLOW,
  PAT_MAGENTA,
  PAT_CYAN,

  PAT_NUM_COLORS,
};

//-----------------------------------------------------
typedef struct
{
  Uint32 x, y;
} PAT_Pos;

typedef struct
{
  float x, y;
} PAT_PosF;

//-----------------------------------------------------
typedef struct
{
  struct
  {
    Uint32 x, y;
    Uint8 state;
  } mouse;

  SDL_Window *win;
  SDL_Renderer *ren;
  SDL_Texture *tex;
  TTF_Font *font;
  const Uint8 *keyboard;
  PAT_Color palette[PAT_NUM_COLORS];
  SDL_Color bg, fg;
  SDL_Rect dest;
  float zoom, margin;
  Uint32 width, height, win_width, win_height;
  Uint32 thickness;
  bool quit_flag, draw_flag, clear_flag;
} PAT;

//-----------------------------------------------------
typedef struct
{
  SDL_Texture *tex;
  Uint32 width, height, cx, cy;
  float sx, sy;
  SDL_TextureAccess access;
} PAT_Sprite;

typedef struct
{
  Uint8 res_x, res_y, num_x, num_y;
  PAT_Sprite spr;
} PAT_SpriteSheet;

extern PAT pat;

//-----------------------------------------------------
extern void PAT_KeyDown (SDL_KeyCode key);
extern void PAT_KeyUp (SDL_KeyCode key);
extern void PAT_MouseDown (Uint8 button);
extern void PAT_MouseUp (Uint8 button);
extern void PAT_MouseMove (Uint32 x, Uint32 y);
extern void PAT_Loop ();

//-----------------------------------------------------
PAT_STATUS PAT_Init ();
PAT_STATUS PAT_InitFont (char *font_name, Uint16 size);
PAT_STATUS PAT_Cleanup ();
void PAT_Events ();
void PAT_Draw ();
void PAT_Start ();

//-----------------------------------------------------
void PAT_SetDrawTarget (PAT_Sprite *spr);
void PAT_ResetDrawTarget ();

void PAT_Clear ();
void PAT_ClearAll ();

void PAT_UpdateZoom (float zoom, float margin);

void PAT_Resize (Uint32 w, Uint32 h);

float PAT_RandF (float lo, float hi);
Uint32 PAT_Rand (Int32 lo, Int32 hi);

SDL_Color PAT_ConvertColorToSDL (PAT_Color col);
PAT_Color PAT_GetColor (PAT_Color index);
void PAT_ConvertColors (PAT_Color indices[], size_t num);

void PAT_SetBG (PAT_Color index);
void PAT_SetFG (PAT_Color index);

void PAT_SetDrawColor (PAT_Color index);
void PAT_ResetDrawColor ();

void PAT_DrawRect (Uint32 x, Uint32 y, Uint32 w, Uint32 h, PAT_Color col);
void PAT_DrawRectFilled (Uint32 x, Uint32 y, Uint32 w, Uint32 h,
                         PAT_Color col);

void PAT_DrawPixel (Uint32 x, Uint32 y, PAT_Color col);

bool PAT_GetKey (SDL_KeyCode key);

//-----------------------------------------------------
PAT_Sprite PAT_CreateSprite (Uint32 width, Uint32 height,
                             SDL_TextureAccess access);

PAT_Sprite PAT_CreateSpriteFromFile (char *filename);

void PAT_DestroySprite (PAT_Sprite *spr);

void PAT_LoadSprite (PAT_Sprite *spr, PAT_Color *pix);

void PAT_DrawSprite (PAT_Sprite *spr, Uint32 x, Uint32 y);

void PAT_DrawSpriteRot (PAT_Sprite *spr, Uint32 x, Uint32 y, Uint16 rot,
                        SDL_RendererFlip flip);
void PAT_DrawSpriteRect (PAT_Sprite *spr, Uint32 x, Uint32 y, Uint32 w,
                         Uint32 h);

void PAT_DrawSpriteRectRot (PAT_Sprite *spr, Uint32 x, Uint32 y, Uint32 w,
                            Uint32 h, Uint16 rot, SDL_RendererFlip flip);

void PAT_DrawSubSprite (PAT_Sprite *spr, Uint32 sx, Uint32 sy, Uint32 sw,
                        Uint32 sh, Uint32 x, Uint32 y);

void PAT_DrawSubSpriteRot (PAT_Sprite *spr, Uint32 sx, Uint32 sy, Uint32 sw,
                           Uint32 sh, Uint32 x, Uint32 y, Uint16 rot,
                           SDL_RendererFlip flip);

void PAT_DrawSubSpriteRect (PAT_Sprite *spr, Uint32 sx, Uint32 sy, Uint32 sw,
                            Uint32 sh, Uint32 x, Uint32 y, Uint32 w, Uint32 h);

void PAT_DrawSubSpriteRectRot (PAT_Sprite *spr, Uint32 sx, Uint32 sy,
                               Uint32 sw, Uint32 sh, Uint32 x, Uint32 y,
                               Uint32 w, Uint32 h, Uint16 rot,
                               SDL_RendererFlip flip);

void PAT_SetSpriteColorMod (PAT_Sprite *spr, PAT_Color index);

void PAT_ScaleSprite (PAT_Sprite *spr, float scale);

Uint32 PAT_GetSpriteWidth (PAT_Sprite *spr);
Uint32 PAT_GetSpriteHeight (PAT_Sprite *spr);

void PAT_CenterSprite (PAT_Sprite *spr);
void PAT_ResetSpriteCenter (PAT_Sprite *spr);

PAT_Sprite PAT_CreateCharSprite (char c);
PAT_Sprite PAT_CreateStringSprite (char *str);

PAT_SpriteSheet PAT_CreateSpriteSheet (char *filename, Uint8 num_x,
                                       Uint8 num_y);

void PAT_DestroySpriteSheet (PAT_SpriteSheet *spr_sh);

void PAT_DrawSpriteSheet (PAT_SpriteSheet *spr_sh, Uint8 tile_x, Uint8 tile_y,
                          Uint32 x, Uint32 y);

void PAT_DrawSpriteSheetRect (PAT_SpriteSheet *spr_sh, Uint8 tile_x,
                              Uint8 tile_y, Uint32 x, Uint32 y, Uint32 w,
                              Uint32 h);

#endif
