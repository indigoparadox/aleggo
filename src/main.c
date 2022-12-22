
#include <stdio.h>

#include <allegro.h>

#include "block.h"
#include "grid.h"

#define ERROR_ALLEGRO 1
#define ERROR_ALLEGRO_GFX 2
#define ERROR_ALLEGRO_MOUSE 4

#define PATH_BLOCKS "blocks"

#ifdef DOS
#define PATH_SEP '\\'
#else
#define PATH_SEP '/'
#endif

#define error_printf( ... ) fprintf( stderr, __VA_ARGS__ )

BITMAP* load_block_bitmap( const char* filename ) {
   BITMAP* bmp_out = NULL;
   char filename_path[PATH_MAX + 1];

   /* Build the path to the bitmap. */
   memset( filename_path, '\0', PATH_MAX + 1 );
   snprintf(
      filename_path, PATH_MAX, "%s%c%s", PATH_BLOCKS, PATH_SEP, filename );

   /* Load and adjust the bitmap. */
   bmp_out = load_bitmap( filename_path, NULL );

   return bmp_out;
}

void draw_toolbox(
   BITMAP* buffer, int toolbox_selected, BITMAP* blocks[BLOCK_MAX]
) {
   int i = 0,
      bg = 0;

   for( i = 1 ; BLOCK_MAX > i ; i++ ) {
      if( i == toolbox_selected ) {
         bg = makecol( 255, 255, 255 );
      } else {
         bg = makecol( 0, 0, 0 );
      }

      rectfill(
         buffer, 0, (i - 1) * BLOCK_PX_H, BLOCK_PX_W, i * BLOCK_PX_H, bg );
   
      draw_sprite( buffer, blocks[i], 0, (i - 1) * BLOCK_PX_H );
   }
}

void draw_grid(
   BITMAP* buffer, int view_x, int view_y,
   unsigned char* grid, BITMAP* blocks[BLOCK_MAX]
) {
   int x = -1,
      y = 2,
      z = 0,
      x2 = -1,
      y2 = GRID_TILE_H + 2,
      px_x = 0,
      px_y = 0,
      px_x2 = 0,
      px_y2 = 0;

   /* Draw left border. */
   grid_to_screen_coords( &px_x, &px_y, x, y, view_x, view_y );
   grid_to_screen_coords( &px_x2, &px_y2, x2, y2, view_x, view_y );
   line( buffer,
      px_x - 8,
      px_y,
      px_x2 - 8,
      px_y2,
      makecol( 0, 0, 0 ) );

   /* Draw top border. */
   x2 = GRID_TILE_W - 1;
   y2 = 2;
   grid_to_screen_coords( &px_x, &px_y, x, y, view_x, view_y );
   grid_to_screen_coords( &px_x2, &px_y2, x2, y2, view_x, view_y );
   line( buffer,
      px_x - 8,
      px_y,
      px_x2 - 8,
      px_y2,
      makecol( 0, 0, 0 ) );

   /* Draw right border. */
   x = GRID_TILE_W - 1;
   y = 2;
   x2 = GRID_TILE_W - 1;
   y2 = GRID_TILE_H + 2;
   grid_to_screen_coords( &px_x, &px_y, x, y, view_x, view_y );
   grid_to_screen_coords( &px_x2, &px_y2, x2, y2, view_x, view_y );
   line( buffer,
      px_x - 8,
      px_y,
      px_x2 - 8,
      px_y2,
      makecol( 0, 0, 0 ) );

   /* Draw bottom border. */
   x = -1;
   y = GRID_TILE_H + 2;
   x2 = GRID_TILE_W - 1;
   y2 = GRID_TILE_H + 2;
   grid_to_screen_coords( &px_x, &px_y, x, y, view_x, view_y );
   grid_to_screen_coords( &px_x2, &px_y2, x2, y2, view_x, view_y );
   line( buffer,
      px_x - 8,
      px_y,
      px_x2 - 8,
      px_y2,
      makecol( 0, 0, 0 ) );

   for( z = 0 ; GRID_TILE_D > z ; z++ ) {
      for( y = 0 ; GRID_TILE_H > y ; y++ ) {

         /* Draw X coordinates backwards to fix overlapping. */
         for( x = GRID_TILE_W - 1 ; 0 <= x ; x-- ) {

            /* Skip empty blocks. */
            if( 0 == grid[grid_idx( z, y, x )] ) {
               continue;
            }

            /* TODO: Optimize drawing off-screen out. */
            grid_to_screen_coords( &px_x, &px_y, x, y, view_x, view_y );

            draw_sprite( buffer, blocks[grid[grid_idx( z, y, x )]],
               px_x, px_y - (z * 4) );
         }
      }
   }
}

void save_screenshot( BITMAP* buffer, const char* filename ) {
   BITMAP* bmp_ss = NULL;
   PALETTE pal;

   get_palette( pal );
   bmp_ss = create_sub_bitmap( buffer, 0, 0, SCREEN_W, SCREEN_H );
   save_bitmap( filename, bmp_ss, pal );

   destroy_bitmap( bmp_ss );
}

int main() {
   unsigned char running = 1;
   int retval = 0,
      i = 0,
      view_x = 0,
      view_y = 0,
      key_val = 0,
      key_x = 0,
      key_y = 0,
      tile_x = 0,
      tile_y = 0,
#ifdef MOUSE
      block_placed = 0,
#endif /* MOUSE */
      toolbox_selected = 1;
   BITMAP** blocks = NULL;
   BITMAP* buffer = NULL;
   unsigned char* grid = NULL;

   grid = calloc( GRID_TILE_D * GRID_TILE_H * GRID_TILE_W, 1 );
   blocks = calloc( sizeof( BITMAP* ), BLOCK_MAX );

   /* === Setup === */

   if( allegro_init() ) {
      allegro_message( "could not setup allegro!" );
      retval = ERROR_ALLEGRO;
      goto cleanup;
   }

   install_keyboard();
#ifdef MOUSE
   /* XXX: Broken in DOS. */
   install_timer();
#endif

#ifdef DOS
   if( set_gfx_mode( GFX_AUTODETECT, 320, 200, 0, 0 ) ) {
#else
   if( set_gfx_mode( GFX_AUTODETECT_WINDOWED, 320, 240, 0, 0 ) ) {
#endif
      allegro_message( "could not setup graphics!" );
      retval = ERROR_ALLEGRO_GFX;
      goto cleanup;
   }

#ifdef MOUSE
   /* XXX: Broken in DOS. */
   if( 0 > install_mouse() ) {
      allegro_message( "could not setup mouse!" );
      retval = ERROR_ALLEGRO_MOUSE;
      goto cleanup;
   }
#endif /* MOUSE */

#if 0
   text_mode( makecol( 0, 0, 255 ) );
   textout_centre( screen, font, "Hey", 100, 100, makecol( 255, 255, 0 ) );
   while( !keypressed() ) {}
   goto cleanup;
#endif

   buffer = create_bitmap( SCREEN_W, SCREEN_H );
   if( NULL == buffer ) {
      allegro_message( "unable to allocate screen buffer!" );
      goto cleanup;
   }

   /* === Load Assets === */

   blocks[BLOCK_1x1x1_RED] = load_block_bitmap( "b1x1x1r.bmp" );
   if( NULL == blocks[BLOCK_1x1x1_RED] ) {
      allegro_message( "unable to load b1x1x1r.bmp" );
      goto cleanup;
   }
   blocks[BLOCK_1x1x1_GREEN] = load_block_bitmap( "b1x1x1g.bmp" );
   if( NULL == blocks[BLOCK_1x1x1_RED] ) {
      allegro_message( "unable to load b1x1x1g.bmp" );
      goto cleanup;
   }
   blocks[BLOCK_1x1x1_BLUE] = load_block_bitmap( "b1x1x1b.bmp" );
   if( NULL == blocks[BLOCK_1x1x1_RED] ) {
      allegro_message( "unable to load b1x1x1b.bmp" );
      goto cleanup;
   }

   /* === Main Loop === */

   do {
      /* Start loop. */
#ifdef MOUSE
      /* XXX: Broken in DOS. */
      poll_mouse();
#endif

#ifdef MOUSE
      /* XXX: Broken in DOS. */
      if( mouse_b & 0x01 ) {
         /* Left mouse button down. */

         if( !block_placed ) {

            /* Check to see if the toolbox was clicked. */
            /* TODO: Support dragging out of the toolbox. */
            if(
               BLOCK_PX_W > mouse_x &&
               BLOCK_PX_H * (BLOCK_MAX - 1) > mouse_y
            ) {
               /* Click was inside the toolbox... but which block? */
               for( i = 1 ; BLOCK_MAX > i ; i++ ) {
                  if( mouse_y < (i * BLOCK_PX_H) ) {
                     toolbox_selected = i;
                     break;
                  }
               }
            }

            /* Get isometric mouse coordinates. */
            grid_from_screen_coords(
               &tile_x, &tile_y, mouse_x, mouse_y, view_x, view_y );

            if( 0 <= grid_place(
               toolbox_selected, tile_x, tile_y, grid,
               GRID_TILE_W, GRID_TILE_H, GRID_TILE_D
            ) ) {
               /* Block was placed. */
               block_placed = 1;
            } else {
               /* Handle viewport dragging if we're not clicking on anything
                * else.
                */
               grid_drag( &view_x, &view_y, mouse_x, mouse_y );
            }
         }
      } else if( mouse_b & 0x02 ) {
         /* Right mouse button down. */

         if( !block_placed ) {
            grid_from_screen_coords(
               &tile_x, &tile_y, mouse_x, mouse_y, view_x, view_y );
            if( 0 <=  grid_remove( 
               tile_x, tile_y, grid, GRID_TILE_W, GRID_TILE_H, GRID_TILE_D
            ) ) {
               /* Block was removed. */
               block_placed = -1;
            }
         }
      } else {
         /* Stopped holding down mouse button. */
         block_placed = 0;
      }
#endif /* MOUSE */

      /* Finish loop. */
      if( keypressed() ) {
         key_val = readkey();
         switch( (key_val >> 8) ) {
         case KEY_RIGHT:
            grid_drag( &view_x, &view_y, --key_x, key_y );
            break;

         case KEY_LEFT:
            grid_drag( &view_x, &view_y, ++key_x, key_y );
            break;

         case KEY_UP:
            grid_drag( &view_x, &view_y, key_x, ++key_y );
            break;

         case KEY_DOWN:
            grid_drag( &view_x, &view_y, key_x, --key_y );
            break;

         case KEY_1:
            toolbox_selected = 1;
            break;

         case KEY_2:
            toolbox_selected = 2;
            break;

         case KEY_3:
            toolbox_selected = 3;
            break;

         case KEY_SPACE:
            grid_from_screen_coords(
               &tile_x, &tile_y, SCREEN_W / 2, SCREEN_H / 2, view_x, view_y );
            grid_place(
               toolbox_selected, tile_x, tile_y, grid,
               GRID_TILE_W, GRID_TILE_H, GRID_TILE_D );
            break;

         case KEY_Q:
            running = 0;
            break;
         }
      }

      /*  === Drawing === */

#ifdef MOUSE
      /* XXX: Broken in DOS. */
      show_mouse( NULL ); /* Disable mouse before drawing. */
#endif
      acquire_screen();
      clear_to_color( buffer, makecol( 128, 128, 128 ) );

      draw_grid( buffer, view_x, view_y, grid, blocks );

      /* Draw toolbox on top of grid. */
      draw_toolbox( buffer, toolbox_selected, blocks );

      blit( buffer, screen, 0, 0, 0, 0, SCREEN_W, SCREEN_H );

      release_screen();
#ifdef MOUSE
      /* XXX: Broken in DOS. */
      show_mouse( screen ); /* Enable mouse after drawing. */
#endif /* MOUSE */
      vsync();

   } while( running );

#if 0
   save_screenshot( buffer, "out.bmp" );
#endif

cleanup:

   if( ERROR_ALLEGRO != retval ) {
      clear_keybuf();
   }

   if( NULL != grid ) {
      free( grid );
   }

   if( NULL != blocks ) {
      for( i = 0 ; BLOCK_MAX > i ; i++ ) {
         if( NULL != blocks[i] ) {
            destroy_bitmap( blocks[i] );
         }
      }
      free( blocks );
   }

   if( NULL != buffer ) {
      destroy_bitmap( buffer );
   }

   return 0;
}
END_OF_MAIN()

