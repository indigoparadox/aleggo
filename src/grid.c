
#include "grid.h"

#include "block.h"

void grid_trans_coords(
   int* tile_x, int* tile_y, int mouse_c_x, int mouse_c_y,
   int view_x, int view_y
) {
   mouse_c_x = (mouse_c_x - view_x) / (BLOCK_PX_W / 2);
   mouse_c_y = (mouse_c_y - view_y) / (-1 * (BLOCK_PX_H / 4));
   *tile_x = ((mouse_c_x + mouse_c_y) / 2) + 1;
   *tile_y = ((mouse_c_x - mouse_c_y) / 2) - 1;
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

