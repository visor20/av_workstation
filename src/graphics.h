#ifndef GRAPHICS_H
#define GRAPHICS_H

#include <stdio.h>
#include <stdint.h>
#include <SDL.h>
#include "main.h"
#include "item_list.h"

/* limiting colors for a simple and clean design
 * 5 colors I picked from the RAL classic palette
 * https://www.ralcolor.com/
 */
#define RED 0xF71027
#define MAGENTA 0xAC3B71
#define GRAY 0x67657A
#define WHITE 0xEFF0EB
#define BLACK 0x1E222C
#define ATLAS_ELEM_H 32
#define ATLAS_ELEM_W 32

uint8_t get_r(uint32_t hex);
uint8_t get_g(uint32_t hex);
uint8_t get_b(uint32_t hex);

void graphics_update(vidaw_state *p_vidaw_state);
void graphics_init_background(vidaw_state *p_vidaw_state);

#endif

