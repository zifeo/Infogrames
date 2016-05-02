#pragma once

#define WINDOW_WIDTH 1280
#define WINDOW_HEIGHT 720

#define CHUNKS 2
#define DELTA 0.02
#define MAX_DIST 4294967296
#define VIEW_DIST 0 // in chunk

#define CAMERA_SPEED 0.04
#define MOUSE_SENSIBILTY 0.002

#ifndef M_PI
#define M_PI 3.14159265
#endif
#define FULLHDFTW 0 // set to 1 if you have a retina screen

//Tree defines
#define TURNING_TREES 0 // 0 if you want TREE_PLANE_COUNT planes trees, 1 if you want 1 plane which rotates towards the camera
#define TREE_PLANE_COUNT 3 // not used if TURNING_TREES is 1

#define TREE_HEIGHT 1/32.0

#define MAX_TREES_PER_CHUNK 60 // the number of trees will be from MAX_TREES_PER_CHUNK/2 to MAX_TREES_PER_CHUNK (counting the trees that will be discarded later)
#define MAX_TREE_ALT 0.3 // trees that spawn outside of this range will be discarded
#define MIN_TREE_ALT 0

