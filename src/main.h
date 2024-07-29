#ifndef MAIN_H
#define MAIN_H

#include <stdint.h>
#include <SDL.h>
#include "item_list.h"

// constants for main setup 
#define WINDOW_WIDTH 640
#define WINDOW_HEIGHT 480
#define MAX_NUM_AUDIO_FILES 4
#define MAX_CL_ARG 8191
#define OUTPUT_DIR "usr"

// constants for audio settings
#define MAIN_AUDIO_FORMAT AUDIO_F32
#define MAIN_NUM_CHANNELS 2
#define MAIN_SAMPLE_RATE 44100
#define MAIN_AUDIO_BUF_SIZE 4096

/* structure representing the overall state of the program
 * SDL_Window *p_win - pointer to the window initialized in main.c 
 * SDL_Renderer *p_ren - pointer to the renderer initialized in main.c
 * item **head - head of a link list of items (see item_list.h for more details)
 * 
 */
typedef struct vidaw_state {
	SDL_Window   *p_win;
	int          window_w;
	int          window_h;
	SDL_Renderer *p_ren;
	item         **head;
	Uint8        *ids;
} vidaw_state; 

/* The file_state struct holds the # of audio / video files 
 * and any relevant data to be processed in the proc. block
 */
typedef struct file_state {
	size_t cur_num_audio_files;
	uint8_t *main_buf;
	uint32_t main_pos;
	SDL_AudioSpec wav_spec[MAX_NUM_AUDIO_FILES];
	uint32_t wav_len[MAX_NUM_AUDIO_FILES];
	uint32_t max_wav_len;
	uint8_t *wav_buf[MAX_NUM_AUDIO_FILES];
} file_state;

// main function for initializing window 
void init_vidaw_state(vidaw_state *p_vidaw_state);

// cleans up memory and quits SDL
void close_vidaw_state(vidaw_state *p_vidaw_state);

// initializes audio and video file information
void init_file_state(file_state *p_file_state);

// cleans up memory for the file state
void close_file_state(file_state *p_file_state);

#endif 
