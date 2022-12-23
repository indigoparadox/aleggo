
#include <stdio.h>

#define RETROFLT_C
#include <retroflt.h>

#define BLOCKS_C
#include "blocks.h"
#include "grid.h"

void draw_toolbox(
   int toolbox_selected, struct RETROFLAT_BITMAP blocks[BLOCK_MAX]
) {
   int i = 0;
   RETROFLAT_COLOR bg;

   for( i = 1 ; BLOCK_MAX > i ; i++ ) {
      if( i == toolbox_selected ) {
         bg = RETROFLAT_COLOR_WHITE;
      } else {
         bg = RETROFLAT_COLOR_BLACK;
      }

      retroflat_rect(
         NULL, bg, 0, (i - 1) * BLOCK_PX_H, BLOCK_PX_W, i * BLOCK_PX_H,
         RETROFLAT_FLAGS_FILL );
   
      retroflat_blit_bitmap( NULL, &(blocks[i]), 0, (i - 1) * BLOCK_PX_H );
   }
}

void draw_grid(
   int view_x, int view_y,
   unsigned char* grid, struct RETROFLAT_BITMAP blocks[BLOCK_MAX]
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
   retroflat_line( NULL,
      RETROFLAT_COLOR_BLACK,
      px_x - 8,
      px_y,
      px_x2 - 8,
      px_y2, 0 );

   /* Draw top border. */
   x2 = GRID_TILE_W - 1;
   y2 = 2;
   grid_to_screen_coords( &px_x, &px_y, x, y, view_x, view_y );
   grid_to_screen_coords( &px_x2, &px_y2, x2, y2, view_x, view_y );
   retroflat_line( NULL,
      RETROFLAT_COLOR_BLACK,
      px_x - 8,
      px_y,
      px_x2 - 8,
      px_y2, 0 );

   /* Draw right border. */
   x = GRID_TILE_W - 1;
   y = 2;
   x2 = GRID_TILE_W - 1;
   y2 = GRID_TILE_H + 2;
   grid_to_screen_coords( &px_x, &px_y, x, y, view_x, view_y );
   grid_to_screen_coords( &px_x2, &px_y2, x2, y2, view_x, view_y );
   retroflat_line( NULL,
      RETROFLAT_COLOR_BLACK,
      px_x - 8,
      px_y,
      px_x2 - 8,
      px_y2, 0 );

   /* Draw bottom border. */
   x = -1;
   y = GRID_TILE_H + 2;
   x2 = GRID_TILE_W - 1;
   y2 = GRID_TILE_H + 2;
   grid_to_screen_coords( &px_x, &px_y, x, y, view_x, view_y );
   grid_to_screen_coords( &px_x2, &px_y2, x2, y2, view_x, view_y );
   retroflat_line( NULL,
      RETROFLAT_COLOR_BLACK,
      px_x - 8,
      px_y,
      px_x2 - 8,
      px_y2, 0 );

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

            retroflat_blit_bitmap( NULL,
               &(blocks[grid[grid_idx( z, y, x )]]), px_x, px_y - (z * 4) );
         }
      }
   }
}

#if 0
void save_screenshot( BITMAP* buffer, const char* filename ) {
   BITMAP* bmp_ss = NULL;
   PALETTE pal;

   get_palette( pal );
   bmp_ss = create_sub_bitmap( buffer, 0, 0, SCREEN_W, SCREEN_H );
   save_bitmap( filename, bmp_ss, pal );

   destroy_bitmap( bmp_ss );
}
#endif

int main() {
   unsigned char running = 1;
   int retval = 0,
      i = 0,
      view_x = 0,
      view_y = 0,
      input = 0,
      key_x = 0,
      key_y = 0,
      tile_x = 0,
      tile_y = 0,
#ifdef RETROFLAT_MOUSE
      block_placed = 0,
#endif /* RETROFLAT_MOUSE */
      toolbox_selected = 1;
   struct RETROFLAT_BITMAP* blocks = NULL;
   struct RETROFLAT_INPUT input_evt;
   unsigned char* grid = NULL;

   grid = calloc( GRID_TILE_D * GRID_TILE_H * GRID_TILE_W, 1 );
   blocks = calloc( sizeof( struct RETROFLAT_BITMAP ), BLOCK_MAX );

   retroflat_set_assets_path( "blocks" );

   /* === Setup === */
   retroflat_init( 320, 200 );

   /* === Load Assets === */

   for( i = 1 ; BLOCK_MAX > i ; i++ ) {
      retval = retroflat_load_bitmap( gc_block_filenames[i], &(blocks[i]) );
      if( RETROFLAT_OK != retval ) {
         goto cleanup;
      }
   }

   /* === Main Loop === */

   do {
      /* Start loop. */
      input = retroflat_poll_input( &input_evt );

      switch( input ) {
      case RETROFLAT_MOUSE_B_LEFT:
         /* Left mouse button down. */
         if( !block_placed ) {
            /* Check to see if the toolbox was clicked. */
            /* TODO: Support dragging out of the toolbox. */
            if(
               BLOCK_PX_W > input_evt.mouse_x &&
               BLOCK_PX_H * (BLOCK_MAX - 1) > input_evt.mouse_y
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
               &tile_x, &tile_y,
               input_evt.mouse_x, input_evt.mouse_y, view_x, view_y );

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
               grid_drag( &view_x, &view_y,
               input_evt.mouse_x, input_evt.mouse_y );
            }
         }
         break;

      case RETROFLAT_MOUSE_B_RIGHT:
         /* Right mouse button down. */

         if( !block_placed ) {
            grid_from_screen_coords(
               &tile_x, &tile_y,
               input_evt.mouse_x, input_evt.mouse_y, view_x, view_y );
            if( 0 <=  grid_remove( 
               tile_x, tile_y, grid, GRID_TILE_W, GRID_TILE_H, GRID_TILE_D
            ) ) {
               /* Block was removed. */
               block_placed = -1;
            }
         }
         break;

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


      default:
         /* Stopped holding down mouse button. */
         block_placed = 0;
         break;
      }

      /*  === Drawing === */

      retroflat_draw_lock();

      retroflat_rect(
         NULL, RETROFLAT_COLOR_GRAY, 0, 0, SCREEN_W, SCREEN_H,
         RETROFLAT_FLAGS_FILL );

      draw_grid( view_x, view_y, grid, blocks );

      /* Draw toolbox on top of grid. */
      draw_toolbox( toolbox_selected, blocks );

      retroflat_draw_flip();

   } while( running );

#if 0
   save_screenshot( buffer, "out.bmp" );
#endif

cleanup:

   if( NULL != grid ) {
      free( grid );
   }

   if( NULL != blocks ) {
      for( i = 0 ; BLOCK_MAX > i ; i++ ) {
         if( retroflat_bitmap_ok( &(blocks[i]) ) ) {
            retroflat_destroy_bitmap( &(blocks[i]) );
         }
      }
      free( blocks );
   }

   retroflat_shutdown( retval );

   return 0;
}
END_OF_MAIN()

