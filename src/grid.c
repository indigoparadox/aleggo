
#include "grid.h"

#include "blocks.h"

void grid_from_screen_coords(
   int* tile_x, int* tile_y, int mouse_c_x, int mouse_c_y,
   int view_x, int view_y
) {
   /* Not 100% confident in these, kind of arrived by trial-and-error. */
   mouse_c_x = (mouse_c_x - view_x) / (BLOCK_PX_W / 2);
   mouse_c_y = (mouse_c_y - view_y) / (-1 * (BLOCK_PX_H / 4));
   *tile_x = ((mouse_c_x + mouse_c_y) / 2) + 1;
   *tile_y = ((mouse_c_x - mouse_c_y) / 2) - 1;
}

void grid_to_screen_coords(
   int* screen_px_x, int* screen_px_y, int grid_px_x, int grid_px_y,
   int view_x, int view_y
) {
   /* Not 100% confident in these, kind of arrived by trial-and-error. */
   *screen_px_x =
      view_x + ((grid_px_x * BLOCK_PX_W / 2) + (grid_px_y * BLOCK_PX_W / 2));
   *screen_px_y = 
      view_y + ((grid_px_y * BLOCK_PX_OFFSET / 2) -
      (grid_px_x * BLOCK_PX_OFFSET / 2) );
}

void grid_drag( int* view_x, int* view_y, int mouse_c_x, int mouse_c_y ) {
   static int mouse_last_x = 0; 
   static int mouse_last_y = 0;

   /* Adjust the viewport by however much the mouse was dragged. */
   if( mouse_last_x < mouse_c_x ) {
      *view_x += GRID_DRAG_VIEW_INC;
   } else if( mouse_last_x > mouse_c_x ) {
      *view_x -= GRID_DRAG_VIEW_INC;
   }
   if( mouse_last_y < mouse_c_y ) {
      *view_y += GRID_DRAG_VIEW_INC;
   } else if( mouse_last_y > mouse_c_y ) {
      *view_y -= GRID_DRAG_VIEW_INC;
   }

   /* Keep the mouse coords to compare on next loop. */
   mouse_last_x = mouse_c_x;
   mouse_last_y = mouse_c_y;
}

int grid_place(
   unsigned char block_idx, int tile_x, int tile_y,
   unsigned char* grid, int grid_tw, int grid_th, int grid_td
) {
   int tile_z = 0;

   if(
      0 <= tile_x && grid_tw > tile_x &&
      0 <= tile_y && grid_th > tile_y
   ) {
      /* Click was inside the grid! */
      tile_z = 0;
      while(
         grid_td > tile_z &&
         0 != grid[grid_idx( tile_z, tile_y, tile_x )]
      ) {
         tile_z++;
      }
      grid[grid_idx( tile_z, tile_y, tile_x )] = block_idx;

      /* TODO: Make sure Z doesn't extend out of bounds! */

      return tile_z;
   } else {
      return -1;
   }
}

int grid_remove(
   int tile_x, int tile_y,
   unsigned char* grid, int grid_tw, int grid_th, int grid_td
) {
   int tile_z = 0;

   if(
      0 <= tile_x && grid_tw > tile_x &&
      0 <= tile_y && grid_th > tile_y
   ) {
      tile_z = grid_td - 1;
      while(
         0 <= tile_z &&
         0 == grid[grid_idx( tile_z, tile_y, tile_x )]
      ) {
         /* Descend until we find the top-most block. */
         tile_z--;
      }
      if( 0 <= tile_z ) {
         grid[grid_idx( tile_z, tile_y, tile_x )] = 0;
         return tile_z;
      }
   }

   return -1;
}

