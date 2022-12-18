
#include <stdio.h>

#include <allegro.h>

#include "block.h"
#include "grid.h"

#define ERROR_ALLEGRO 1
#define ERROR_ALLEGRO_GFX 2
#define ERROR_ALLEGRO_MOUSE 4

#define PATH_BLOCKS "blocks"
#define PATH_SEP '/'

typedef unsigned char uint8_t;

#define error_printf( ... ) fprintf( stderr, __VA_ARGS__ )

BITMAP* load_block_bitmap( const char* filename ) {
   BITMAP* bmp_out = NULL;
   int x = 0,
      y = 0,
      px = 0;
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
   uint8_t grid[GRID_TILE_D][GRID_TILE_H][GRID_TILE_W],
   BITMAP* blocks[BLOCK_MAX]
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
            if( 0 >= blocks[grid[z][y][x]] ) {
               continue;
            }

            /* TODO: Optimize drawing off-screen out. */
            grid_to_screen_coords( &px_x, &px_y, x, y, view_x, view_y );

            draw_sprite( buffer, blocks[grid[z][y][x]], px_x, px_y - (z * 4) );
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
   uint8_t running = 1;
   int retval = 0,
      i = 0,
      view_x = 0,
      view_y = 0,
      tile_x = 0,
      tile_y = 0,
      tile_z = 0,
      block_placed = 0,
      toolbox_selected = 1;
   BITMAP* blocks[BLOCK_MAX] = { NULL };
   BITMAP* buffer = NULL;
   uint8_t grid[GRID_TILE_D][GRID_TILE_H][GRID_TILE_W];

   memset( blocks, '\0', sizeof( blocks ) );
   memset( grid, '\0', GRID_TILE_W * GRID_TILE_H * GRID_TILE_D );

   /* === Setup === */

   if( allegro_init() ) {
      error_printf( "could not setup allegro!\n" );
      retval = ERROR_ALLEGRO;
      goto cleanup;
   }

   install_keyboard();
   install_timer();

   if( set_gfx_mode( GFX_AUTODETECT_WINDOWED, 320, 240, 0, 0 ) ) {
      error_printf( "could not setup graphics!\n" );
      retval = ERROR_ALLEGRO_GFX;
      goto cleanup;
   }

   if( 0 > install_mouse() ) {
      error_printf( "could not setup mouse!\n" );
      retval = ERROR_ALLEGRO_MOUSE;
      goto cleanup;
   }

   buffer = create_bitmap( SCREEN_W, SCREEN_H );

   /* === Load Assets === */
   blocks[BLOCK_1x1x1_RED] = load_block_bitmap( "b1x1x1r.bmp" );
   blocks[BLOCK_1x1x1_GREEN] = load_block_bitmap( "b1x1x1g.bmp" );
   blocks[BLOCK_1x1x1_BLUE] = load_block_bitmap( "b1x1x1b.bmp" );

   /* === Main Loop === */

   do {
      /* Start loop. */
      poll_mouse();
      show_mouse( NULL ); /* Disable mouse before drawing. */
      acquire_screen();

      clear_to_color( buffer, makecol( 128, 128, 128 ) );

      draw_grid( buffer, view_x, view_y, grid, blocks );

      /* Draw toolbox on top of grid. */
      draw_toolbox( buffer, toolbox_selected, blocks );

      blit( buffer, screen, 0, 0, 0, 0, SCREEN_W, SCREEN_H );

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

            if(
               0 <= tile_x && GRID_TILE_W > tile_x &&
               0 <= tile_y && GRID_TILE_H > tile_y
            ) {
               /* Click was inside the grid! */
               tile_z = 0;
               while(
                  GRID_TILE_D > tile_z && 0 != grid[tile_z][tile_y][tile_x]
               ) {
                  tile_z++;
               }
               grid[tile_z][tile_y][tile_x] = toolbox_selected;
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

         grid_from_screen_coords(
            &tile_x, &tile_y, mouse_x, mouse_y, view_x, view_y );
         if(
            0 <= tile_x && GRID_TILE_W > tile_x &&
            0 <= tile_y && GRID_TILE_H > tile_y
         ) {
            tile_z = GRID_TILE_D - 1;
            while( 0 <= tile_z && 0 == grid[tile_z][tile_y][tile_x] ) {
               /* Descend until we find the top-most block. */
               tile_z--;
            }
            grid[tile_z][tile_y][tile_x] = 0;
         }
      } else {
         /* Stopped holding down mouse button. */
         block_placed = 0;
      }

      /* Finish loop. */
      if( keypressed() ) {
         running = 0;
      }
      release_screen();
      show_mouse( screen ); /* Enable mouse after drawing. */
      vsync();

   } while( running );

   save_screenshot( buffer, "out.bmp" );

cleanup:
   if( ERROR_ALLEGRO != retval ) {
      clear_keybuf();
   }

   for( i = 0 ; BLOCK_MAX > i ; i++ ) {
      if( NULL != blocks[i] ) {
         destroy_bitmap( blocks[i] );
      }
   }

   if( NULL != buffer ) {
      destroy_bitmap( buffer );
   }

   return 0;
}
END_OF_MAIN()

