#include <stdio.h>
#include <string.h>
#include <SDL.h>
#include "graphics.h"
#include "item_list.h"

// gets red value from hex
uint8_t get_r(uint32_t hex) { return ((hex >> 16) & 0xFF); }

// gets green value from hex
uint8_t get_g(uint32_t hex) { return ((hex >> 8) & 0xFF); }

// gets blue value from hex 
uint8_t get_b(uint32_t hex) { return (hex & 0xFF); }

static void fill_index_buffer(vidaw_state *p_vidaw_state, item *p_item)
{
	uint8_t *ids = p_vidaw_state->ids;
	int window_w = p_vidaw_state->window_w;
	int window_h = p_vidaw_state->window_h;
	for (int i = 0; i < p_item->h; i++)
	{
		/* ((p_item->y + i) * window_w) determines the correct row
		 * ... + p_item->x determines the correct column
		 */
		uint8_t *row_start = ids + ((p_item->y + i) * window_w) + p_item->x;
		memset(row_start, p_item->id, p_item->w);
	}
}

static void draw_item(vidaw_state *p_vidaw_state, item *p_item)
{
	if (p_item->type == RECTANGLE || p_item->type == PAUSE_BUTTON)
	{
		SDL_Rect rect = {
			p_item->x, 
			p_item->y, 
			p_item->w,
			p_item->h
		};
		
		SDL_SetRenderDrawColor(
			p_vidaw_state->p_ren,
			get_r(p_item->color),
			get_g(p_item->color),
			get_b(p_item->color),
			p_item->alpha
		);
		
		// update id buffer 
		fill_index_buffer(p_vidaw_state, p_item);

		// finish displaying drawing
		SDL_RenderFillRect(p_vidaw_state->p_ren, &rect);
	}
	if (p_item->type == PLAY_BUTTON)
	{
		SDL_Color tri_color = {
			get_r(p_item->color),
			get_g(p_item->color),
			get_b(p_item->color),
			255
		};

		SDL_Vertex vertex[3] = {
			{{(float)p_item->x, (float)p_item->y}, tri_color, {0, 0}},
			{{(float)p_item->x, (float)(p_item->y + p_item->h)}, tri_color, {0, 0}},
			{{(float)(p_item->x + p_item->w), (float)(p_item->y + p_item->h / 2)}, tri_color, {0, 0}}
		};

		fill_index_buffer(p_vidaw_state, p_item);
		SDL_RenderGeometry(p_vidaw_state->p_ren, NULL, vertex, 3, NULL, 0);
	}
	// present 
	SDL_RenderPresent(p_vidaw_state->p_ren);
}

static void draw_item_list(vidaw_state* p_vidaw_state)
{
	item *tmp = *p_vidaw_state->head;
	while(tmp != NULL)
	{
		draw_item(p_vidaw_state, tmp);
		tmp = tmp->next;
	}
}

void graphics_update(vidaw_state *p_vidaw_state)
{
	return;
}

void graphics_init_background(vidaw_state *p_vidaw_state)
{	
	item **head = p_vidaw_state->head;
	int window_w = p_vidaw_state->window_w;
	int window_h = p_vidaw_state->window_h;

	// background
	append_item(head, 1, RECTANGLE, GRAY, 255, 0, 0, window_w, window_h);

	// top bar (top bar and its buttons not writable by user)
	append_item(head, 0, RECTANGLE, BLACK, 255, 0, 0, window_w, (window_h / 8));

	// play button is left most button on the nav bar
	append_item(head, 0, PLAY_BUTTON, MAGENTA, 255, (int)(window_w / 2.2), (window_h / 32), (window_h / 16), (window_h / 16));

	// pause button
	append_item(head, 0, PAUSE_BUTTON, MAGENTA, 255, (int)(window_w / 2.2) + (window_h / 8), (window_h / 32), (window_h / 16), (window_h / 16));

	// draw all items up to this point
	draw_item_list(p_vidaw_state);
}
