
#ifdef BLOCKS_XPM
#include "blocks_xpm.h"
#endif /* BLOCKS_XPM */

#define MAUG_C
#include <maug.h>

#define RETROFLT_C
#include <retroflt.h>

#define BLOCKS_C
#include "blocks.h"
#include "grid.h"

struct ALEGGO_DATA {
   struct RETROFLAT_BITMAP* blocks;
   unsigned char* grid;
   int view_x;
   int view_y;
};

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
   
      retroflat_blit_bitmap(
         NULL, &(blocks[i]),
         0, 0, 0, (i - 1) * BLOCK_PX_H, BLOCK_PX_W, BLOCK_PX_H );
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

            retroflat_blit_bitmap(
               NULL, &(blocks[grid[grid_idx( z, y, x )]]),
               0, 0, px_x, px_y - (z * 4), BLOCK_PX_W, BLOCK_PX_H );
         }
      }
   }
}

#if 0
void save_screenshot( BITMAP* buffer, const char* filename ) {
   BITMAP* bmp_ss = NULL;
   PALETTE pal;

   get_palette( pal );
   bmp_ss = create_sub_bitmap( buffer, 0, 0, retroflat_screen_w(), retroflat_screen_h() );
   save_bitmap( filename, bmp_ss, pal );

   destroy_bitmap( bmp_ss );
}
#endif

void aleggo_loop( struct ALEGGO_DATA* data ) {
   static int toolbox_selected = 1;
   static int key_x = 0;
   static int key_y = 0;
   static int block_placed = 0;
   int i = 0,
      input = 0,
      tile_x = 0,
      tile_y = 0;
   struct RETROFLAT_INPUT input_evt;
   char status[255] = { 0 };

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
               if( input_evt.mouse_y < (i * BLOCK_PX_H) ) {
                  toolbox_selected = i;
                  break;
               }
            }
         }

         /* Get isometric mouse coordinates. */
         grid_from_screen_coords(
            &tile_x, &tile_y,
            input_evt.mouse_x, input_evt.mouse_y, data->view_x, data->view_y );

         if( 0 <= grid_place(
            toolbox_selected, tile_x, tile_y, data->grid,
            GRID_TILE_W, GRID_TILE_H, GRID_TILE_D
         ) ) {
            /* Block was placed. */
            block_placed = 1;
         } else {
            /* Handle viewport dragging if we're not clicking on anything
               * else.
               */
            grid_drag( &(data->view_x), &(data->view_y),
            input_evt.mouse_x, input_evt.mouse_y );
         }
      }
      break;

   case RETROFLAT_MOUSE_B_RIGHT:
      /* Right mouse button down. */

      if( !block_placed ) {
         grid_from_screen_coords(
            &tile_x, &tile_y,
            input_evt.mouse_x, input_evt.mouse_y, data->view_x, data->view_y );
         if( 0 <=  grid_remove( 
            tile_x, tile_y, data->grid, GRID_TILE_W, GRID_TILE_H, GRID_TILE_D
         ) ) {
            /* Block was removed. */
            block_placed = -1;
         }
      }
      break;

   case RETROFLAT_KEY_RIGHT:
      grid_drag( &(data->view_x), &(data->view_y), --key_x, key_y );
      break;

   case RETROFLAT_KEY_LEFT:
      grid_drag( &(data->view_x), &(data->view_y), ++key_x, key_y );
      break;

   case RETROFLAT_KEY_UP:
      grid_drag( &(data->view_x), &(data->view_y), key_x, ++key_y );
      break;

   case RETROFLAT_KEY_DOWN:
      grid_drag( &(data->view_x), &(data->view_y), key_x, --key_y );
      break;

   case RETROFLAT_KEY_1:
      toolbox_selected = 1;
      break;

   case RETROFLAT_KEY_2:
      toolbox_selected = 2;
      break;

   case RETROFLAT_KEY_3:
      toolbox_selected = 3;
      break;

   case RETROFLAT_KEY_SPACE:
      grid_from_screen_coords(
         &tile_x, &tile_y, 
         retroflat_screen_w() / 2, retroflat_screen_h() / 2,
         data->view_x, data->view_y );
      grid_place(
         toolbox_selected, tile_x, tile_y, data->grid,
         GRID_TILE_W, GRID_TILE_H, GRID_TILE_D );
      break;

   case RETROFLAT_KEY_ESC:
      retroflat_quit( 0 );
      break;

   default:
      /* Stopped holding down mouse button. */
      block_placed = 0;
      break;
   }

   /*  === Drawing === */

   retroflat_draw_lock( NULL );

   /* Clear screen to gray. */
   retroflat_rect(
      NULL, RETROFLAT_COLOR_GRAY, 0, 0,
      retroflat_screen_w(), retroflat_screen_h(),
      RETROFLAT_FLAGS_FILL );

   draw_grid( data->view_x, data->view_y, data->grid, data->blocks );

   /* Draw toolbox on top of grid. */
   draw_toolbox( toolbox_selected, data->blocks );

   maug_snprintf( status, 255, "%lu", retroflat_get_ms() );
   retroflat_string(
      NULL, RETROFLAT_COLOR_WHITE, status, 255, NULL, BLOCK_PX_W, 0, 0 );
      
   retroflat_cursor( NULL, 0 );

   retroflat_draw_release( NULL );
}

int main( int argc, char** argv ) {
   int retval = 0,
      i = 0;
   struct ALEGGO_DATA* data = NULL;
   struct RETROFLAT_ARGS args;

   data = calloc( sizeof( struct ALEGGO_DATA ), 1 );
   data->grid = calloc( GRID_TILE_D * GRID_TILE_H * GRID_TILE_W, 1 );
   data->blocks = calloc( sizeof( struct RETROFLAT_BITMAP ), BLOCK_MAX );

   /* === Setup === */

   args.screen_w = 320;
   args.screen_h = 200;
   args.title = "Aleggo";
   args.assets_path = "blocks";

   retval = retroflat_init( argc, argv, &args );
   if( RETROFLAT_OK != retval ) {
      goto cleanup;
   }

   /* === Load Assets === */

   for( i = 1 ; BLOCK_MAX > i ; i++ ) {
#ifdef BLOCKS_XPM
      retval = retroflat_load_xpm(
         gc_block_filenames[i], &(data->blocks[i]) );
#else
      retval = retroflat_load_bitmap(
         gc_block_filenames[i], &(data->blocks[i]) );
#endif /* BLOCKS_XPM */
      if( RETROFLAT_OK != retval ) {
         retroflat_message(
            "Aleggo Error", "Could not load bitmap: %s", gc_block_filenames[i]
         );
         goto cleanup;
      }
   }

   /* === Main Loop === */

   retroflat_loop( (retroflat_loop_iter)aleggo_loop, data );

 #if 0
   save_screenshot( buffer, "out.bmp" );
#endif

cleanup:

#ifndef RETROFLAT_OS_WASM

   if( NULL != data->grid ) {
      free( data->grid );
   }

   if( NULL != data->blocks ) {
      for( i = 0 ; BLOCK_MAX > i ; i++ ) {
         if( retroflat_bitmap_ok( &(data->blocks[i]) ) ) {
            retroflat_destroy_bitmap( &(data->blocks[i]) );
         }
      }
      free( data->blocks );
   }

   if( NULL != data ) {
      free( data );
   }

   retroflat_shutdown( retval );

#endif /* !RETROFLAT_OS_WASM */

   return retval;
}
END_OF_MAIN()

