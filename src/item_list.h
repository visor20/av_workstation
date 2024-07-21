#ifndef ITEM_LIST_H
#define ITEM_LIST_H

#include <stdio.h>
#include <stdint.h>

typedef enum item_type {
	ITEM_TYPE_ERROR,
	RECTANGLE, 
	CIRCLE,
	PLAY_BUTTON,
	PAUSE_BUTTON
} item_type;

// forward declaration 
typedef struct item item;

struct item
{
	uint8_t id;
	uint8_t write; // 1 if write allowed, 0 otherwise
	item_type type;
	int x, y, w, h;
	uint32_t color; // one of the colors defined in graphics.h
	uint8_t alpha; // transparency
	item *next;
};


void append_item(item **head, 
		 const uint8_t write, 
		 item_type type, 
		 uint32_t color, 
		 uint8_t alpha, 
		 int x, 
		 int y, 
		 int w, 
		 int h
		 );

item_type get_type_from_id(uint8_t id, item** head);

void free_item_list(item **head);

void print_item_list(item **head);

#endif
