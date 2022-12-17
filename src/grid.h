
#ifndef GRID_H
#define GRID_H

#define GRID_TILE_W 64
#define GRID_TILE_H 64

/*! \brief Increment by which dragging modifies the viewport. */
#define GRID_DRAG_VIEW_INC         5

/**
 * \brief Given mouse-on-screen coordinates, return the isometric tile that
 *        the coordinates are within.
 */
void grid_trans_coords(
   int* tile_x, int* tile_y, int mouse_c_x, int mouse_c_y,
   int view_x, int view_y );

/**
 * \brief Handle mouse-based dragging of the grid in the screen viewport.
 */
void grid_drag( int* view_x, int* view_y, int mouse_c_x, int mouse_c_y );

#endif /* !GRID_H */

