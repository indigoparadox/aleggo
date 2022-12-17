
#include <stdio.h>

#include <allegro.h>

#define BLOCK_1x1x1_RED       0x01
#define BLOCK_1x1x1_GREEN     0x02
#define BLOCK_1x1x1_BLUE      0x03
#define BLOCK_MAX             0x04

/*! \brief Width of a single 1x1 block bitmap. */
#define BLOCK_W               16
/*! \brief Height of a single 1x1 block bitmap. */
#define BLOCK_H               16
/*! \brief Isometric offset modifier for drawing blocks. */
#define BLOCK_OFFSET          8

/*! \brief Increment by which dragging modifies the viewport. */
#define DRAG_VIEW_INC         5

#define ERROR_ALLEGRO 1
#define ERROR_ALLEGRO_GFX 2
#define ERROR_ALLEGRO_MOUSE 4

#define GRID_W 64
#define GRID_H 64

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

#if 0
   /* Make the bitmap transparent. */
   /* Obsolete; just modify the palette directly in the assets for now. */
   for( y = 0 ; bmp_out->h > y ; y++ ) {
      for( x = 0 ; bmp_out->w > x ; x++ ) {
         px = getpixel( bmp_out, x, y );
         if(
            255 == getr( px ) &&
            0 == getg( px ) &&
            255 == getb( px )
         ) {
            putpixel( bmp_out, x, y, bitmap_mask_color( bmp_out ) );
         }
      }
   }
#endif

   return bmp_out;
}

void save_screenshot( const char* filename ) {
   BITMAP* bmp_ss = NULL;
   PALETTE pal;

   get_palette( pal );
   bmp_ss = create_sub_bitmap( screen, 0, 0, SCREEN_W, SCREEN_H );
   save_bitmap( filename, bmp_ss, pal );

   destroy_bitmap( bmp_ss );
}

void draw_grid(
   BITMAP* buffer, int view_x, int view_y,
   uint8_t grid[GRID_H][GRID_W], BITMAP* blocks[BLOCK_MAX]
) {
   int x = 0,
      y = 0;

   for( y = 0 ; GRID_H > y ; y++ ) {

      /* Draw X coordinates backwards to fix overlapping. */
      for( x = GRID_W - 1 ; 0 <= x ; x-- ) {

         /* TODO: Optimize drawing off-screen out. */

         draw_sprite( buffer, blocks[grid[y][x] + 1],
            /* Isometric transform X. */
            view_x + ((x * BLOCK_W / 2) + (y * BLOCK_W / 2)),
            /* Isometric transform Y. */
            view_y + ((y * BLOCK_OFFSET / 2) - (x * BLOCK_OFFSET / 2) ));
      }
   }
}

int main() {
   uint8_t running = 1;
   int retval = 0,
      i = 0,
      view_x = 0,
      view_y = 0,
      mouse_last_x = 0,
      mouse_last_y = 0;
   BITMAP* blocks[BLOCK_MAX] = { NULL };
   BITMAP* buffer = NULL;
   uint8_t grid[GRID_H][GRID_W];

   memset( blocks, '\0', sizeof( blocks ) );
   memset( grid, '\0', GRID_W * GRID_H );

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

      blit( buffer, screen, 0, 0, 0, 0, SCREEN_W, SCREEN_H );

      if( mouse_b & 0x01 ) {
         /* printf( "%d, %d\n", mouse_x, mouse_y ); */
         /* putpixel( screen, mouse_x, mouse_y, makecol( 0xff, 0xff, 0xff ) ); */

         /* Handle viewport dragging if we're not clicking on anything else. */
         if( mouse_last_x < mouse_x ) {
            view_x += DRAG_VIEW_INC;
         } else if( mouse_last_x > mouse_x ) {
            view_x -= DRAG_VIEW_INC;
         }
         if( mouse_last_y < mouse_y ) {
            view_y += DRAG_VIEW_INC;
         } else if( mouse_last_y > mouse_y ) {
            view_y -= DRAG_VIEW_INC;
         }
      }

      mouse_last_x = mouse_x;
      mouse_last_y = mouse_y;

      /* Finish loop. */
      if( keypressed() ) {
         running = 0;
      }
      release_screen();
      show_mouse( screen );
      vsync();

   } while( running );

   save_screenshot( "out.bmp" );

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

