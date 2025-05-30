#ifndef GRUPP11_GUARD
#define GRUPP11_GUARD

#include <stdint.h>
#include <stdbool.h>
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>

#include <nrf5340_application.h>
#include <nrfx.h>
#include <nrf_gpio.h>
#include <nrfx_uarte.h>
#include <nrfx_rtc.h>
#include <nrfx_config.h>
#include <nrfx_systick.h>

void start_game_grupp11(void);

//////////////////////////////////////
////////////// 11 ////////////////////
//////////////////////////////////////

#define TERMINAL_WIDTH 200
#define TERMINAL_HEIGHT 100

#define RAND(min, max) (rand() % (max - min)) + min

//////////////////////////////////////
////////// BoundingBox ///////////////
//////////////////////////////////////

typedef struct {
  int x;
  int y;
  int width;
  int height;
} BoundingBox;

BoundingBox globbo_bb = {
  .x = 2,
  .y = 4,
  .width = 12,
  .height = 9
};

BoundingBox pipe_top_up_bb = {
  .x = 3,
  .y = 7,
  .width = 10,
  .height = 9
};

BoundingBox pipe_body_bb = {
  .x = 3,
  .y = 0,
  .width = 10,
  .height = 16
};

BoundingBox pipe_top_down_bb = {
  .x = 3,
  .y = 0,
  .width = 10,
  .height = 9
};

//////////////////////////////////////
//////////// SPRITES /////////////////
//////////////////////////////////////

// Group pixels into groups of 2
// Allows for more compact memory usage
// Currently cuts all sizes in half
typedef struct {
  uint8_t left:3;
  uint8_t right:3;
} Pixels;

typedef struct {
  Pixels data[64];
} Sprite_8x8;

typedef struct {
  Pixels data[128];
} Sprite_16x16;

typedef struct {
  Pixels data[1250];
} Sprite_50x50;

// Better than having to write 300+ long lines
#define SPRITE_8x8(name) const Sprite_8x8 name = { .data = {
#define SPRITE_16x16(name) const Sprite_16x16 name = { .data = {
#define SPRITE_50(name) const Sprite_50x50 name = { .data = {

#define GROUP(a,b) { .left=a, .right=b }

#define ROW_8(a,b,c,d,e,f,g,h) GROUP(a,b), GROUP(c,d), GROUP(e,f), GROUP(g,h),
#define ROW_16(a,b,c,d,e,f,g,h,i,j,k,l,m,n,o,p) GROUP(a,b), GROUP(c,d), GROUP(e,f), GROUP(g,h), GROUP(i,j), GROUP(k,l), GROUP(m,n), GROUP(o,p),
#define ROW_50(_1,_2,_3,_4,_5,_6,_7,_8,_9,_10,_11,_12,_13,_14,_15,_16,_17,_18,_19,_20,_21,_22,_23,_24,_25,_26,_27,_28,_29,_30,_31,_32,_33,_34,_35,_36,_37,_38,_39,_40,_41,_42,_43,_44,_45,_46,_47,_48,_49,_50) GROUP(_1,_2),GROUP(_3,_4),GROUP(_5,_6),GROUP(_7,_8),GROUP(_9,_10),GROUP(_11,_12),GROUP(_13,_14),GROUP(_15,_16),GROUP(_17,_18),GROUP(_19,_20),GROUP(_21,_22),GROUP(_23,_24),GROUP(_25,_26),GROUP(_27,_28),GROUP(_29,_30),GROUP(_31,_32),GROUP(_33,_34),GROUP(_35,_36),GROUP(_37,_38),GROUP(_39,_40),GROUP(_41,_42),GROUP(_43,_44),GROUP(_45,_46),GROUP(_47,_48),GROUP(_49,_50),

#define END_SPRITE() }};

// Group pixels into groups of 2
// Allows for more compact memory usage
// Currently cuts all sizes in half
typedef struct {
  uint8_t left:3;
  uint8_t right:3;
} Pixels;

typedef struct {
  Pixels data[64];
} Sprite_8x8;

typedef struct {
  Pixels data[128];
} Sprite_16x16;

typedef struct {
  Pixels data[1250];
} Sprite_50x50;

// Better than having to write 300+ long lines
#define SPRITE_8x8(name) const Sprite_8x8 name = { .data = {
#define SPRITE_16x16(name) const Sprite_16x16 name = { .data = {
#define SPRITE_50(name) const Sprite_50x50 name = { .data = {

#define GROUP(a,b) { .left=a, .right=b }

#define ROW_8(a,b,c,d,e,f,g,h) GROUP(a,b), GROUP(c,d), GROUP(e,f), GROUP(g,h),
#define ROW_16(a,b,c,d,e,f,g,h,i,j,k,l,m,n,o,p) GROUP(a,b), GROUP(c,d), GROUP(e,f), GROUP(g,h), GROUP(i,j), GROUP(k,l), GROUP(m,n), GROUP(o,p),
#define ROW_50(_1,_2,_3,_4,_5,_6,_7,_8,_9,_10,_11,_12,_13,_14,_15,_16,_17,_18,_19,_20,_21,_22,_23,_24,_25,_26,_27,_28,_29,_30,_31,_32,_33,_34,_35,_36,_37,_38,_39,_40,_41,_42,_43,_44,_45,_46,_47,_48,_49,_50) GROUP(_1,_2),GROUP(_3,_4),GROUP(_5,_6),GROUP(_7,_8),GROUP(_9,_10),GROUP(_11,_12),GROUP(_13,_14),GROUP(_15,_16),GROUP(_17,_18),GROUP(_19,_20),GROUP(_21,_22),GROUP(_23,_24),GROUP(_25,_26),GROUP(_27,_28),GROUP(_29,_30),GROUP(_31,_32),GROUP(_33,_34),GROUP(_35,_36),GROUP(_37,_38),GROUP(_39,_40),GROUP(_41,_42),GROUP(_43,_44),GROUP(_45,_46),GROUP(_47,_48),GROUP(_49,_50),

#define END_SPRITE() }};

//////////////////////////////////////
///////////// SPRITE /////////////////
//////////////////////////////////////

#define CSI "\e[" // \033[


//////////////////////////////////////
////////////// PIPE //////////////////
//////////////////////////////////////

// How many cells up, minimum
#define MIN_PIPE_Y 20
// How many cells down, minimum
#define MAX_PIPE_Y 60

// How big the gap is, minimum
#define MIN_PIPE_GAP_SIZE 10
// How big the gap is, maximum
#define MAX_PIPE_GAP_SIZE 25

typedef struct {
  int x;
  int y;
  int gap_size;
  bool scored;
} Pipe;

//////////////////////////////////////
////////////// GAME //////////////////
//////////////////////////////////////

#define GLOBBO_X 20
#define GLOBBO_START_Y 40

#define PIPES 5
#define PIPE_MIN_DELAY 10
#define PIPE_MAX_DELAY 25


#endif