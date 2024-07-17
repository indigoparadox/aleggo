
#ifdef BLOCKS_XPM
#include "blocks_xpm.h"
#endif /* BLOCKS_XPM */

#define MAUG_C
#include <maug.h>
#include <retroflt.h>
#include <retrofnt.h>
#include <retrogui.h>
#include <retrocon.h>

#define BLOCKS_C
#include "blocks.h"
#include "grid.h"

struct ALEGGO_DATA {
   MAUG_MHANDLE blocks_h;
   MAUG_MHANDLE grid_h;
   int view_x;
   int view_y;
   struct RETROCON con;
   uint8_t dirty;
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
         0, 0, 0, (i - 1) * BLOCK_PX_H, BLOCK_PX_W, BLOCK_PX_H,
         RETROFLAT_INSTANCE_NULL );
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
            px_y -= (z * 4);

            if(
               (size_t)(px_x + BLOCK_PX_W) >= retroflat_screen_w() ||
               (size_t)(px_y + BLOCK_PX_H) >= retroflat_screen_h()
            ) {
               continue;
            }

            assert( 0 <= px_x );
            assert( 0 <= px_y );

            retroflat_blit_bitmap(
               NULL, &(blocks[grid[grid_idx( z, y, x )]]),
               0, 0, px_x, px_y, BLOCK_PX_W, BLOCK_PX_H,
               RETROFLAT_INSTANCE_NULL );
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
   RETROFLAT_IN_KEY input = 0;
   int i = 0,
      tile_x = 0,
      tile_y = 0;
   struct RETROFLAT_INPUT input_evt;
   char status[256] = { 0 };
   struct RETROFLAT_BITMAP* blocks = NULL;
   uint8_t* grid = NULL;
   MERROR_RETVAL retval = MERROR_OK;
   retrogui_idc_t con_idc = RETROGUI_IDC_NONE;

   maug_mlock( data->grid_h, grid );
   maug_cleanup_if_null_alloc( uint8_t*, grid );

   maug_mlock( data->blocks_h, blocks );
   maug_cleanup_if_null_alloc( struct RETROFLAT_BITMAP*, blocks );

   /* Start loop. */
   input = retroflat_poll_input( &input_evt );

   retrocon_input( &(data->con), &input, &input_evt, &con_idc, NULL, 0 );
   if( RETROCON_IDC_CLOSE == con_idc ) {
      data->dirty = 1;
   }

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
            toolbox_selected, tile_x, tile_y, grid,
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
      data->dirty = 1;
      break;

   case RETROFLAT_MOUSE_B_RIGHT:
      /* Right mouse button down. */

      if( !block_placed ) {
         grid_from_screen_coords(
            &tile_x, &tile_y,
            input_evt.mouse_x, input_evt.mouse_y, data->view_x, data->view_y );
         if( 0 <=  grid_remove( 
            tile_x, tile_y, grid, GRID_TILE_W, GRID_TILE_H, GRID_TILE_D
         ) ) {
            /* Block was removed. */
            block_placed = -1;
         }
      }
      data->dirty = 1;
      break;

   case RETROFLAT_KEY_RIGHT:
      grid_drag( &(data->view_x), &(data->view_y), --key_x, key_y );
      data->dirty = 1;
      break;

   case RETROFLAT_KEY_LEFT:
      grid_drag( &(data->view_x), &(data->view_y), ++key_x, key_y );
      data->dirty = 1;
      break;

   case RETROFLAT_KEY_UP:
      grid_drag( &(data->view_x), &(data->view_y), key_x, ++key_y );
      data->dirty = 1;
      break;

   case RETROFLAT_KEY_DOWN:
      grid_drag( &(data->view_x), &(data->view_y), key_x, --key_y );
      data->dirty = 1;
      break;

   case RETROFLAT_KEY_1:
      toolbox_selected = 1;
      data->dirty = 1;
      break;

   case RETROFLAT_KEY_2:
      toolbox_selected = 2;
      data->dirty = 1;
      break;

   case RETROFLAT_KEY_3:
      toolbox_selected = 3;
      data->dirty = 1;
      break;

   case RETROFLAT_KEY_SPACE:
      grid_from_screen_coords(
         &tile_x, &tile_y, 
         retroflat_screen_w() / 2, retroflat_screen_h() / 2,
         data->view_x, data->view_y );
      grid_place(
         toolbox_selected, tile_x, tile_y, grid,
         GRID_TILE_W, GRID_TILE_H, GRID_TILE_D );
      data->dirty = 1;
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

   if( data->dirty ) {
      /* Clear screen to gray. */
      retroflat_rect(
         NULL, RETROFLAT_COLOR_GRAY, 0, 0,
         retroflat_screen_w(), retroflat_screen_h(),
         RETROFLAT_FLAGS_FILL );

      draw_grid( data->view_x, data->view_y, grid, blocks );

      /* Draw toolbox on top of grid. */
      draw_toolbox( toolbox_selected, blocks );

      /*
      maug_mzero( status, 256 );
      maug_snprintf( status, 255, "%lu", retroflat_get_ms() );
      retroflat_string(
         NULL, RETROFLAT_COLOR_WHITE, status, 255, NULL, BLOCK_PX_W, 0, 0
      );
      */

      /* retroflat_cursor( NULL, 0 ); */

      data->dirty = 0;
   }

   retrocon_display( &(data->con), NULL );
         
   retroflat_draw_release( NULL );

cleanup:

   if( NULL != grid ) {
      maug_munlock( data->grid_h, grid );
   }

   if( NULL != blocks ) {
      maug_munlock( data->blocks_h, blocks );
   }

   if( MERROR_OK != retval ) {
      retroflat_quit( retval );
   }
}

int main( int argc, char** argv ) {
   int retval = 0,
      i = 0;
   struct ALEGGO_DATA* data = NULL;
   struct RETROFLAT_ARGS args;
   struct RETROFLAT_BITMAP* blocks = NULL;
   uint8_t* grid = NULL;

   logging_init();

   data = calloc( sizeof( struct ALEGGO_DATA ), 1 );
   maug_cleanup_if_null_alloc( struct ALEGGO_DATA*, data );

   data->grid_h =
      maug_malloc( 1, GRID_TILE_D * GRID_TILE_H * GRID_TILE_W );
   maug_cleanup_if_null_alloc( MAUG_MHANDLE, data->grid_h );
   maug_mlock( data->grid_h, grid );
   maug_cleanup_if_null_alloc( uint8_t*, grid );
   maug_mzero( grid, GRID_TILE_D * GRID_TILE_H * GRID_TILE_W );

   data->blocks_h =
      maug_malloc( sizeof( struct RETROFLAT_BITMAP ), BLOCK_MAX );
   maug_cleanup_if_null_alloc( MAUG_MHANDLE, data->blocks_h );
   maug_mlock( data->blocks_h, blocks );
   maug_mzero( blocks, sizeof( struct RETROFLAT_BITMAP ) * BLOCK_MAX );

   data->dirty = 1;

   /* === Setup === */

   maug_mzero( &args, sizeof( struct RETROFLAT_ARGS ) );

   args.title = "Aleggo";
   args.assets_path = "blocks";

   retval = retroflat_init( argc, argv, &args );
   if( RETROFLAT_OK != retval ) {
      goto cleanup;
   }

   retrocon_init( &(data->con), "unscii-8.hex",
      (retroflat_screen_w() >> 1) - 100,
      (retroflat_screen_h() >> 1) - 50,
      200, 100 );

   data->con.lbuffer_color = RETROFLAT_COLOR_WHITE;
   data->con.sbuffer_color = RETROFLAT_COLOR_GRAY;
   data->con.bg_color = RETROFLAT_COLOR_BLACK;

   /* === Load Assets === */

   for( i = 1 ; BLOCK_MAX > i ; i++ ) {
#ifdef BLOCKS_XPM
      retval = retroflat_load_xpm(
         gc_block_filenames[i], &(blocks[i]) );
#else
      retval = retroflat_load_bitmap(
         gc_block_filenames[i], &(blocks[i]), 0 );
#endif /* BLOCKS_XPM */
      if( RETROFLAT_OK != retval ) {
         retroflat_message( RETROFLAT_MSG_FLAG_ERROR,
            "Aleggo Error", "Could not load bitmap: %s", gc_block_filenames[i]
         );
         goto cleanup;
      }
   }

   maug_munlock( data->grid_h, grid );
   maug_munlock( data->blocks_h, blocks );

   /* === Main Loop === */

   retroflat_loop( (retroflat_loop_iter)aleggo_loop, NULL, data );

 #if 0
   save_screenshot( buffer, "out.bmp" );
#endif

cleanup:

#ifndef RETROFLAT_OS_WASM

   retrocon_shutdown( &(data->con) );

   if( NULL != data->grid_h ) {
      maug_mfree( data->grid_h );
   }

   if( NULL != data->blocks_h ) {
      maug_mlock( data->blocks_h, blocks );
      for( i = 0 ; BLOCK_MAX > i ; i++ ) {
         if( retroflat_bitmap_ok( &(blocks[i]) ) ) {
            retroflat_destroy_bitmap( &(blocks[i]) );
         }
      }
      maug_munlock( data->blocks_h, blocks );
      maug_mfree( data->blocks_h );
   }

   if( NULL != data ) {
      free( data );
   }

   retroflat_shutdown( retval );

   logging_shutdown();

#endif /* !RETROFLAT_OS_WASM */

   return retval;
}
END_OF_MAIN()

