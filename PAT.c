#include "PAT.h"

//-----------------------------------------------------
const size_t PAT_COLOR_SIZE = sizeof (PAT_Color);

//-----------------------------------------------------
PAT pat = {
  .mouse = { .x = 0, .y = 0, .state = 0 },
  .width = 400,
  .height = 400,
  .win_width = 800,
  .win_height = 800,
  .thickness = 1,
  .margin = 16,
  .zoom = 1.0,
  .quit_flag = false,
  .draw_flag = false,
  .clear_flag = true,
};

//-----------------------------------------------------
PAT_STATUS
PAT_Init ()
{
  srand (time (NULL));

  if (SDL_Init (SDL_INIT_VIDEO) < 0)
    {
      printf ("[error] SDL_Init failed: %s\n", SDL_GetError ());
      return PAT_ERR;
    }

  if (IMG_Init (IMG_INIT_PNG | IMG_INIT_JPG) < 0)
    {
      printf ("[error] IMG_Init failed: %s\n", SDL_GetError ());
      return PAT_ERR;
    }

  pat.win = SDL_CreateWindow ("hmm", SDL_WINDOWPOS_CENTERED,
                              SDL_WINDOWPOS_CENTERED, pat.win_width,
                              pat.win_height, SDL_WINDOW_SHOWN);
  if (!pat.win)
    {
      printf ("[error] SDL_CreateWindow failed: %s\n", SDL_GetError ());
      return PAT_ERR;
    }

  pat.ren = SDL_CreateRenderer (pat.win, -1, 0);
  if (!pat.ren)
    {
      printf ("[error] SDL_CreateRenderer failed: %s\n", SDL_GetError ());
      return PAT_ERR;
    }

  PAT_Resize (pat.width, pat.height);

  memset (pat.palette, 0, PAT_NUM_COLORS * PAT_COLOR_SIZE);
  pat.palette[PAT_TRANSPARENT] = 0x0000;

  pat.palette[PAT_BLACK] = 0x000f;
  pat.palette[PAT_WHITE] = 0xffff;

  pat.palette[PAT_RED] = 0xf00f;
  pat.palette[PAT_GREEN] = 0x0f0f;
  pat.palette[PAT_BLUE] = 0x00ff;

  pat.palette[PAT_YELLOW] = 0xff0f;
  pat.palette[PAT_MAGENTA] = 0xf0ff;
  pat.palette[PAT_CYAN] = 0x0fff;

  PAT_SetBG (PAT_BLACK);
  PAT_SetFG (PAT_WHITE);

  PAT_UpdateZoom (MIN ((float)pat.win_width / pat.width,
                       (float)pat.win_height / pat.height),
                  pat.margin);

  PAT_ClearAll ();

  pat.keyboard = SDL_GetKeyboardState (NULL);

  return PAT_OK;
}

PAT_STATUS
PAT_InitFont (char *font_name, Uint16 size)
{
  if (TTF_Init () < 0)
    {
      printf ("[error] TTF_Init failed: %s\n", SDL_GetError ());
      return PAT_ERR;
    }

  pat.font = TTF_OpenFont ("font.ttf", size);
  if (!pat.font)
    {
      printf ("[error] TTF_OpenFont failed: %s\n", SDL_GetError ());
      TTF_Quit ();
      return PAT_ERR;
    }

  return PAT_OK;
}

//-----------------------------------------------------
PAT_STATUS
PAT_Cleanup ()
{
  if (pat.font)
    {
      TTF_CloseFont (pat.font);
      TTF_Quit ();
    }

  SDL_DestroyTexture (pat.tex);
  SDL_DestroyRenderer (pat.ren);
  SDL_DestroyWindow (pat.win);

  return PAT_OK;
}

//-----------------------------------------------------
void
PAT_Events ()
{
  SDL_Event evt;

  while (SDL_PollEvent (&evt))
    {
      switch (evt.type)
        {

        //--- system
        case SDL_QUIT:
          pat.quit_flag = true;
          break;
        //--- ~system

        //--- window
        case SDL_WINDOWEVENT:
          {
            switch (evt.window.event)
              {

              case SDL_WINDOWEVENT_RESIZED:
                pat.win_width = evt.window.data1;
                pat.win_height = evt.window.data2;
                PAT_UpdateZoom (MIN ((float)pat.win_width / pat.width,
                                     (float)pat.win_height / pat.height),
                                pat.margin);
                SDL_SetRenderTarget (pat.ren, NULL);
                PAT_Clear ();
                SDL_SetRenderTarget (pat.ren, pat.tex);
                break;

              default:
                break;
              }
          }
          break;
          //--- ~window

        //--- keyboard
        case SDL_KEYDOWN:
          PAT_KeyDown (evt.key.keysym.sym);
          break;

        case SDL_KEYUP:
          PAT_KeyUp (evt.key.keysym.sym);
          break;
          //--- ~keyboard

        //--- mouse
        case SDL_MOUSEMOTION:
          {
            pat.mouse.x
                = (float)CLAMP (evt.motion.x - pat.dest.x, 0, pat.dest.w)
                  / pat.dest.w * pat.width;
            pat.mouse.y
                = (float)CLAMP (evt.motion.y - pat.dest.y, 0, pat.dest.h)
                  / pat.dest.h * pat.height;
            PAT_MouseMove (pat.mouse.x, pat.mouse.y);
          }
          break;

        case SDL_MOUSEBUTTONDOWN:
          {
            pat.mouse.state = 0;

            switch (evt.button.button)
              {
              case SDL_BUTTON_LEFT:
                pat.mouse.state = SDL_BUTTON_LEFT;
                break;

              case SDL_BUTTON_RIGHT:
                pat.mouse.state = SDL_BUTTON_RIGHT;
                break;

              default:
                break;
              }

            PAT_MouseDown (pat.mouse.state);
          }
          break;

        case SDL_MOUSEBUTTONUP:
          {
            PAT_MouseUp (pat.mouse.state);
            pat.mouse.state = 0;
          }
          break;
          //--- mouse
        }
    }
}

//-----------------------------------------------------
void
PAT_Draw ()
{
  if (!pat.draw_flag)
    return;

  SDL_SetRenderTarget (pat.ren, NULL);

  if (pat.clear_flag)
    PAT_Clear ();

  SDL_RenderCopy (pat.ren, pat.tex, NULL, &pat.dest);
  SDL_RenderPresent (pat.ren);

  SDL_SetRenderTarget (pat.ren, pat.tex);

  if (pat.clear_flag)
    PAT_Clear ();

  pat.draw_flag = false;
}

//-----------------------------------------------------
void
PAT_Start ()
{
  double inv_perf_freq
      = 1.0 / ((double)SDL_GetPerformanceFrequency () * 1000.0);

  while (!pat.quit_flag)
    {
      double st = SDL_GetPerformanceCounter ();

      PAT_Events ();
      PAT_Loop ();
      PAT_Draw ();

      double et = (SDL_GetPerformanceCounter () - st) * inv_perf_freq;

      SDL_Delay (CLAMP (16.666 - et, 0, 1000));
    }
}

//-----------------------------------------------------
void
PAT_SetDrawTarget (PAT_Sprite *spr)
{
  SDL_SetRenderTarget (pat.ren, spr->tex);
}

void
PAT_ResetDrawTarget ()
{
  SDL_SetRenderTarget (pat.ren, pat.tex);
}

//-----------------------------------------------------
void
PAT_Clear ()
{
  PAT_ResetDrawColor ();
  SDL_RenderClear (pat.ren);

  pat.draw_flag = true;
}

void
PAT_ClearAll ()
{
  PAT_ResetDrawColor ();

  SDL_SetRenderTarget (pat.ren, NULL);
  SDL_RenderClear (pat.ren);

  SDL_SetRenderTarget (pat.ren, pat.tex);
  SDL_RenderClear (pat.ren);

  pat.draw_flag = true;
}

//-----------------------------------------------------
void
PAT_UpdateZoom (float zoom, float margin)
{
  pat.zoom = MAX (0.0, zoom);
  pat.margin = margin;

  float w = pat.zoom * pat.width;
  float h = pat.zoom * pat.height;
  float x = (pat.win_width - w) * 0.5f;
  float y = (pat.win_height - h) * 0.5f;
  pat.dest.x = round (x + pat.margin);
  pat.dest.y = round (y + pat.margin);
  pat.dest.w = round (w - pat.margin * 2.0f);
  pat.dest.h = round (h - pat.margin * 2.0f);

  SDL_SetRenderTarget (pat.ren, NULL);
  PAT_Clear ();
  SDL_SetRenderTarget (pat.ren, pat.tex);

  pat.draw_flag = true;
}

//-----------------------------------------------------
void
PAT_Resize (Uint32 w, Uint32 h)
{
  SDL_DestroyTexture (pat.tex);

  pat.tex
      = SDL_CreateTexture (pat.ren, SDL_PIXELFORMAT_RGBA8888,
                           SDL_TEXTUREACCESS_TARGET, pat.width, pat.height);
  if (!pat.tex)
    {
      printf ("[error] SDL_CreateTexture failed: %s\n", SDL_GetError ());
      pat.quit_flag = true;
      return;
    }

  PAT_ClearAll ();
}

//-----------------------------------------------------
float
PAT_RandF (float lo, float hi)
{
  return SCALE ((float)rand () / (float)RAND_MAX, 0.0f, 1.0f, lo, hi);
}

Uint32
PAT_Rand (Int32 lo, Int32 hi)
{
  return rand () % ((hi + 1) - lo) + lo;
}

//-----------------------------------------------------
SDL_Color
PAT_ConvertColorToSDL (PAT_Color col)
{
  Uint8 r = (col >> 12) & 0xf;
  Uint8 g = (col >> 8) & 0xf;
  Uint8 b = (col >> 4) & 0xf;
  Uint8 a = col & 0xf;

  SDL_Color out = {
    .r = r | (r << 4),
    .g = g | (g << 4),
    .b = b | (b << 4),
    .a = a | (a << 4),
  };

  return out;
}

PAT_Color
PAT_GetColor (PAT_Color index)
{
  return pat.palette[index];
}

void
PAT_ConvertColors (PAT_Color indices[], size_t num)
{
  for (size_t i = 0; i < num; ++i)
    indices[i] = PAT_GetColor (indices[i]);
}

void
PAT_SetBG (PAT_Color index)
{
  pat.bg = PAT_ConvertColorToSDL (PAT_GetColor (index));
}

void
PAT_SetFG (PAT_Color index)
{
  pat.fg = PAT_ConvertColorToSDL (PAT_GetColor (index));
}

void
PAT_SetDrawColor (PAT_Color index)
{
  SDL_Color col = PAT_ConvertColorToSDL (PAT_GetColor (index));
  SDL_SetRenderDrawColor (pat.ren, col.r, col.g, col.b, col.a);
}

void
PAT_ResetDrawColor ()
{
  SDL_SetRenderDrawColor (pat.ren, pat.bg.r, pat.bg.g, pat.bg.b, pat.bg.a);
}

//-----------------------------------------------------
void
PAT_DrawRect (Uint32 x, Uint32 y, Uint32 w, Uint32 h, PAT_Color col)
{
  PAT_SetDrawColor (col);

  SDL_Rect rect;

  // top
  rect.x = x;
  rect.y = y;
  rect.w = w;
  rect.h = pat.thickness;
  SDL_RenderFillRect (pat.ren, &rect);

  // bot
  rect.x = x;
  rect.y = y + h - pat.thickness;
  rect.w = w;
  rect.h = pat.thickness;
  SDL_RenderFillRect (pat.ren, &rect);

  // left
  rect.x = x;
  rect.y = y + pat.thickness;
  rect.w = pat.thickness;
  rect.h = h - pat.thickness;
  SDL_RenderFillRect (pat.ren, &rect);

  // right
  rect.x = x + w - pat.thickness;
  rect.y = y + pat.thickness;
  rect.w = pat.thickness;
  rect.h = h - pat.thickness * 2;
  SDL_RenderFillRect (pat.ren, &rect);

  PAT_ResetDrawColor ();

  pat.draw_flag = true;
}

void
PAT_DrawRectFilled (Uint32 x, Uint32 y, Uint32 w, Uint32 h, PAT_Color col)
{
  PAT_SetDrawColor (col);

  SDL_Rect rect = { .x = x, .y = y, .w = w, .h = h };
  SDL_RenderFillRect (pat.ren, &rect);

  PAT_ResetDrawColor ();

  pat.draw_flag = true;
}

void
PAT_DrawPixel (Uint32 x, Uint32 y, PAT_Color col)
{
  PAT_SetDrawColor (col);

  SDL_RenderDrawPoint (pat.ren, x, y);

  PAT_ResetDrawColor ();

  pat.draw_flag = true;
}

//-----------------------------------------------------
bool
PAT_GetKey (SDL_KeyCode key)
{
  return pat.keyboard[SDL_GetScancodeFromKey (key)];
}

//-----------------------------------------------------
PAT_Sprite
PAT_CreateSprite (Uint32 width, Uint32 height, SDL_TextureAccess access)
{
  PAT_Sprite spr = {
    .width = width,
    .height = height,
    .cx = 0,
    .cy = 0,
    .sx = 1,
    .sy = 1,
    .access = access,
  };

  spr.tex
      = SDL_CreateTexture (pat.ren, PAT_PIXEL_FORMAT, access, width, height);
  SDL_SetTextureBlendMode (spr.tex, SDL_BLENDMODE_BLEND);

  return spr;
}

PAT_Sprite
PAT_CreateSpriteFromFile (char *filename)
{
  PAT_Sprite spr = {
    .width = 0,
    .height = 0,
    .cx = 0,
    .cy = 0,
    .sx = 1,
    .sy = 1,
    .access = SDL_TEXTUREACCESS_STATIC,
  };

  spr.tex = IMG_LoadTexture (pat.ren, filename);

  if (!spr.tex)
    {
      printf ("[error] unable to load sprite, filename %s\n", filename);
    }
  else
    {
      SDL_QueryTexture (spr.tex, NULL, NULL, (int *)&spr.width,
                        (int *)&spr.height);
    }

  return spr;
}

void
PAT_DestroySprite (PAT_Sprite *spr)
{
  SDL_DestroyTexture (spr->tex);
}

void
PAT_LoadSprite (PAT_Sprite *spr, PAT_Color *pix)
{
  if (spr->access == SDL_TEXTUREACCESS_TARGET)
    {
      printf ("[error] must use STATIC or STREAMING for PAT_LoadSprite!\n");
      return;
    }

  SDL_UpdateTexture (spr->tex, NULL, pix, spr->width * PAT_COLOR_SIZE);
}

void
PAT_DrawSprite (PAT_Sprite *spr, Uint32 x, Uint32 y)
{
  SDL_Rect dest = {
    .x = x - spr->cx * spr->sx,
    .y = y - spr->cy * spr->sy,
    .w = spr->width * spr->sx,
    .h = spr->height * spr->sy,
  };

  SDL_RenderCopy (pat.ren, spr->tex, NULL, &dest);

  pat.draw_flag = true;
}

void
PAT_DrawSpriteRot (PAT_Sprite *spr, Uint32 x, Uint32 y, Uint16 rot,
                   SDL_RendererFlip flip)
{
  SDL_Rect dest = {
    .x = x - spr->cx * spr->sx,
    .y = y - spr->cy * spr->sy,
    .w = spr->width * spr->sx,
    .h = spr->height * spr->sy,
  };

  SDL_RenderCopyEx (pat.ren, spr->tex, NULL, &dest, rot, NULL, flip);

  pat.draw_flag = true;
}

void
PAT_DrawSpriteRect (PAT_Sprite *spr, Uint32 x, Uint32 y, Uint32 w, Uint32 h)
{
  SDL_Rect dest = {
    .x = x,
    .y = y,
    .w = w,
    .h = h,
  };

  SDL_RenderCopy (pat.ren, spr->tex, NULL, &dest);

  pat.draw_flag = true;
}

void
PAT_DrawSpriteRectRot (PAT_Sprite *spr, Uint32 x, Uint32 y, Uint32 w, Uint32 h,
                       Uint16 rot, SDL_RendererFlip flip)
{
  SDL_Rect dest = {
    .x = x,
    .y = y,
    .w = w,
    .h = h,
  };

  SDL_RenderCopyEx (pat.ren, spr->tex, NULL, &dest, rot, NULL, flip);

  pat.draw_flag = true;
}

void
PAT_DrawSubSprite (PAT_Sprite *spr, Uint32 sx, Uint32 sy, Uint32 sw, Uint32 sh,
                   Uint32 x, Uint32 y)
{
  SDL_Rect dest = {
    .x = x - spr->cx * spr->sx,
    .y = y - spr->cy * spr->sy,
    .w = spr->width * spr->sx,
    .h = spr->height * spr->sy,
  };

  SDL_Rect src = {
    .x = sx,
    .y = sy,
    .w = sw,
    .h = sh,
  };

  SDL_RenderCopy (pat.ren, spr->tex, &src, &dest);

  pat.draw_flag = true;
}

void
PAT_DrawSubSpriteRot (PAT_Sprite *spr, Uint32 sx, Uint32 sy, Uint32 sw,
                      Uint32 sh, Uint32 x, Uint32 y, Uint16 rot,
                      SDL_RendererFlip flip)
{
  SDL_Rect dest = {
    .x = x - spr->cx * spr->sx,
    .y = y - spr->cy * spr->sy,
    .w = spr->width * spr->sx,
    .h = spr->height * spr->sy,
  };

  SDL_Rect src = {
    .x = sx,
    .y = sy,
    .w = sw,
    .h = sh,
  };

  SDL_RenderCopyEx (pat.ren, spr->tex, &src, &dest, rot, NULL, flip);

  pat.draw_flag = true;
}

void
PAT_DrawSubSpriteRect (PAT_Sprite *spr, Uint32 sx, Uint32 sy, Uint32 sw,
                       Uint32 sh, Uint32 x, Uint32 y, Uint32 w, Uint32 h)
{
  SDL_Rect dest = {
    .x = x,
    .y = y,
    .w = w,
    .h = h,
  };

  SDL_Rect src = {
    .x = sx,
    .y = sy,
    .w = sw,
    .h = sh,
  };

  SDL_RenderCopy (pat.ren, spr->tex, &src, &dest);

  pat.draw_flag = true;
}

void
PAT_DrawSubSpriteRectRot (PAT_Sprite *spr, Uint32 sx, Uint32 sy, Uint32 sw,
                          Uint32 sh, Uint32 x, Uint32 y, Uint32 w, Uint32 h,
                          Uint16 rot, SDL_RendererFlip flip)
{
  SDL_Rect dest = {
    .x = x,
    .y = y,
    .w = w,
    .h = h,
  };

  SDL_Rect src = {
    .x = sx,
    .y = sy,
    .w = sw,
    .h = sh,
  };

  SDL_RenderCopyEx (pat.ren, spr->tex, &src, &dest, rot, NULL, flip);

  pat.draw_flag = true;
}

void
PAT_SetSpriteColorMod (PAT_Sprite *spr, PAT_Color index)
{
  SDL_Color c = PAT_ConvertColorToSDL (pat.palette[index]);
  SDL_SetTextureColorMod (spr->tex, c.r, c.g, c.b);
}

void
PAT_ScaleSprite (PAT_Sprite *spr, float scale)
{
  spr->sx = spr->sy = scale;
}

Uint32
PAT_GetSpriteWidth (PAT_Sprite *spr)
{
  return spr->width * spr->sx;
}
Uint32
PAT_GetSpriteHeight (PAT_Sprite *spr)
{
  return spr->height * spr->sy;
}

void
PAT_CenterSprite (PAT_Sprite *spr)
{
  spr->cx = spr->width * 0.5;
  spr->cy = spr->height * 0.5;
}
void
PAT_ResetSpriteCenter (PAT_Sprite *spr)
{
  spr->cx = spr->cy = 0;
}

//
//
//

PAT_Sprite
PAT_CreateCharSprite (char c)
{
  SDL_Surface *surf = TTF_RenderGlyph_Solid (pat.font, c, pat.fg);
  SDL_Texture *tex = SDL_CreateTextureFromSurface (pat.ren, surf);
  SDL_FreeSurface (surf);

  PAT_Sprite spr = {
    .width = surf->w,
    .height = surf->h,
    .sx = 1,
    .sy = 1,
    .tex = tex,
  };

  return spr;
}

PAT_Sprite
PAT_CreateStringSprite (char *str)
{
  SDL_Surface *surf = TTF_RenderText_Solid (pat.font, str, pat.fg);
  SDL_Texture *tex = SDL_CreateTextureFromSurface (pat.ren, surf);
  SDL_FreeSurface (surf);

  PAT_Sprite spr = {
    .width = surf->w,
    .height = surf->h,
    .sx = 1,
    .sy = 1,
    .tex = tex,
  };

  return spr;
}

//
//
//

PAT_SpriteSheet
PAT_CreateSpriteSheet (char *filename, Uint8 num_x, Uint8 num_y)
{
  PAT_SpriteSheet spr_sh = {
    .num_x = num_x,
    .num_y = num_y,
  };

  spr_sh.spr = PAT_CreateSpriteFromFile (filename);
  spr_sh.res_x = IFLOOR ((float)spr_sh.spr.width / num_x);
  spr_sh.res_y = IFLOOR ((float)spr_sh.spr.height / num_y);

  return spr_sh;
}

void
PAT_DestroySpriteSheet (PAT_SpriteSheet *spr_sh)
{
  PAT_DestroySprite (&spr_sh->spr);
}

void
PAT_DrawSpriteSheet (PAT_SpriteSheet *spr_sh, Uint8 tile_x, Uint8 tile_y,
                     Uint32 x, Uint32 y)
{
  PAT_DrawSubSprite (&spr_sh->spr, tile_x * spr_sh->res_x,
                     tile_y * spr_sh->res_y, spr_sh->res_x, spr_sh->res_y,
                     spr_sh->res_x * spr_sh->spr.sx,
                     spr_sh->res_y * spr_sh->spr.sy);
}

void
PAT_DrawSpriteSheetRect (PAT_SpriteSheet *spr_sh, Uint8 tile_x, Uint8 tile_y,
                         Uint32 x, Uint32 y, Uint32 w, Uint32 h)
{
  PAT_DrawSubSpriteRect (&spr_sh->spr, tile_x * spr_sh->res_x,
                         tile_y * spr_sh->res_y, spr_sh->res_x, spr_sh->res_y,
                         x, y, w, h);
}
