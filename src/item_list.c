#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include "item_list.h"


static item *create_item(uint8_t id, const uint8_t write, item_type type, uint32_t color, uint8_t alpha, int x, int y, int w, int h)
{
	item *new = (item *)malloc(sizeof(item));
	if (new == NULL)
	{
		fprintf(stderr, "cannot allocate item memory");
		exit(1);
	}
	new->id = id;
	new->write = write;
	new->type = type;
	new->color = color;
	new->alpha = alpha;
	new->x = x;
	new->y = y;
	new->w = w;
	new->h = h;
	new->next = NULL;
	return new;
}


void append_item(item **head, const uint8_t write, item_type type, uint32_t color, uint8_t alpha, int x, int y, int w, int h)
{
	item *new = create_item(0, write, type, color, alpha, x, y, w, h);
	if (*head == NULL)
	{
		*head = new;
		return;
	}

	item *tmp = *head;
	while(tmp->next != NULL)
	{
		tmp = tmp->next;
	}

	// assign an original id
	new->id = (tmp->id) + 1;

	// will change in the future
	assert(new->id < 50);

	tmp->next = new;
}

item_type get_type_from_id(uint8_t id, item** head)
{
	item *tmp = *head;
	while (tmp != NULL)
	{
		if (tmp->id == id)
		{
			return tmp->type;
		}
		tmp = tmp->next;
	}
	return ITEM_TYPE_ERROR;
}

void free_item_list(item **head)
{
	item *head_d = *head;
	item *tmp;
	while(head_d != NULL)
	{
		tmp = head_d;
		head_d = head_d->next;
		free(tmp);
	}
}

// only for debug 
void print_item_list(item **head)
{
	item *tmp = *head;
	while(tmp != NULL)
	{
		printf("id: %u, write: %d\n", tmp->id, tmp->write);
		tmp = tmp->next;
	}
	printf("NULL\n");
}
