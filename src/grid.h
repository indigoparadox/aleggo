
#ifndef GRID_H
#define GRID_H

#define GRID_TILE_W 32
#define GRID_TILE_H 32
#define GRID_TILE_D 32

#define grid_idx( z, y, x ) ((z * (GRID_TILE_W * GRID_TILE_H)) + (y * GRID_TILE_H) + x)

/*! \brief Increment by which dragging modifies the viewport. */
#define GRID_DRAG_VIEW_INC         5

/**
 * \brief Given mouse-on-screen pixel coordinates, return the isometric tile
 *        the coordinates are within.
 */
void grid_from_screen_coords(
   int* tile_x, int* tile_y, int mouse_c_x, int mouse_c_y,
   int view_x, int view_y );

void grid_to_screen_coords(
   int* screen_px_x, int* screen_px_y, int grid_px_x, int grid_px_y,
   int view_x, int view_y );

/**
 * \brief Handle mouse-based dragging of the grid in the screen viewport.
 */
void grid_drag( int* view_x, int* view_y, int mouse_c_x, int mouse_c_y );

int grid_place(
   unsigned char block_idx, int tile_x, int tile_y,
   unsigned char* grid, int grid_tw, int grid_th, int grid_td );

int grid_remove(
   int tile_x, int tile_y,
   unsigned char* grid, int grid_tw, int grid_th, int grid_td );

#endif /* !GRID_H */

