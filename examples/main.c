#include <PAT.h>

//-----------------------------------------------------
PAT_Sprite spr;
PAT_Color mod = PAT_RED;

#define NUM_SPRITES 64
PAT_PosF pos[NUM_SPRITES];

//-----------------------------------------------------
void
PAT_KeyDown (SDL_KeyCode key)
{
  switch (key)
    {
    case SDLK_ESCAPE:
      pat.quit_flag = true;
      break;

    case 'z':
      mod = (mod + 1) % (PAT_NUM_COLORS - PAT_WHITE) + PAT_WHITE;
      PAT_SetSpriteColorMod (&spr, mod);
      break;

    case 'x':
      PAT_SetSpriteColorMod (&spr, PAT_WHITE);
      break;

    case 'c':
      pat.clear_flag ^= true;
      break;

    default:
      break;
    }
}

//-----------------------------------------------------
void
PAT_KeyUp (SDL_KeyCode key)
{
}

//-----------------------------------------------------
void
PAT_MouseDown (Uint8 button)
{
}

//-----------------------------------------------------
void
PAT_MouseUp (Uint8 button)
{
}

//-----------------------------------------------------
void
PAT_MouseMove (Uint32 x, Uint32 y)
{
}

//-----------------------------------------------------
float t = 0;
void
PAT_Loop ()
{
  if (PAT_GetKey ('='))
    {
      PAT_UpdateZoom (pat.zoom * 1.01, pat.margin);
    }
  else if (PAT_GetKey ('-'))
    {
      PAT_UpdateZoom (pat.zoom * 0.99, pat.margin);
    }

  PAT_DrawSubSpriteRect (&spr, 0, 0, 8, 8, 0, 0, pat.width * 0.5,
                         pat.height * 0.5);
  PAT_DrawSubSpriteRect (&spr, 0, 8, 8, 8, pat.width * 0.5, 0, pat.width * 0.5,
                         pat.height * 0.5);
  PAT_DrawSubSpriteRect (&spr, 8, 0, 8, 8, pat.width * 0.5, pat.height * 0.5,
                         pat.width * 0.5, pat.height * 0.5);
  PAT_DrawSubSpriteRect (&spr, 8, 8, 8, 8, 0, pat.height * 0.5,
                         pat.width * 0.5, pat.height * 0.5);

  PAT_CenterSprite (&spr);
  PAT_ScaleSprite (&spr, 8);
  PAT_DrawSprite (&spr, pat.width * 0.5, pat.height * 0.5);
  PAT_ResetSpriteCenter (&spr);

  for (Uint32 i = 0; i < NUM_SPRITES; ++i)
    {
      PAT_PosF *p = &pos[i];

      p->x = SCALE (cos (t + (float)i / NUM_SPRITES * PI), -1.0, 1.0, 0.2, 0.8)
             * pat.width;
      p->y = SCALE (sin (t + (float)i / NUM_SPRITES * TAU * 1.3), -1.0, 1.0,
                    0.2, 0.8)
             * pat.height;

      PAT_ScaleSprite (&spr, SCALE (sin (t + (float)i / NUM_SPRITES * 16),
                                    -1.0, 1.0, 0.75, 4));

      PAT_DrawSpriteRot (&spr, p->x, p->y, t * 360, SDL_FLIP_VERTICAL);
    }

  PAT_DrawRect (pat.mouse.x - 8, pat.mouse.y - 8, 16, 16,
                pat.mouse.state == 1 ? PAT_RED : PAT_WHITE);

  t += 0.01;
}

//-----------------------------------------------------
int
main (void)
{
  if (PAT_Init ())
    return PAT_ERR;

  { // Setup
    PAT_Resize (258, 240);
    pat.margin = 16;
    pat.thickness = 3;
    pat.clear_flag = true;
    PAT_SetFG (PAT_WHITE);
    PAT_SetBG (PAT_BLACK);

    spr = PAT_CreateSpriteFromFile ("sprite.png");

    for (Uint32 i = 0; i < NUM_SPRITES; ++i)
      {
        pos[i].x = PAT_RandF (0.0, pat.width);
        pos[i].y = PAT_RandF (0.0, pat.height);
      }
  }

  PAT_Start ();

  { // Cleanup
    PAT_DestroySprite (&spr);
  }

  return PAT_Cleanup ();
}
