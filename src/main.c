#include <windows.h>
#include <io.h>
#include <direct.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <SDL.h>
#include "main.h"
#include "item_list.h"
#include "graphics.h"
#include "audio_functions.h"

static void vidaw_error(const char* title, const char* text, bool abort) 
{
	if (abort)
	{	
		fprintf(stderr, "ABORT: %s ... %s\n", title, text);
		SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, title, text, NULL);
		SDL_Quit();
		exit(1);
	}
	// no exit necessary
	fprintf(stderr, "VIDAW error: %s ... %s\n", title, text);
}

static char *convert_windows_error()
{
	size_t error_msg_len;
	LPVOID error_msg;
	DWORD code = GetLastError();
	DWORD flags = FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS;

	FormatMessage(
		flags,
		NULL,
		code,
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPSTR)&error_msg,
		0,
		NULL
	);

	error_msg_len = strlen((char *)error_msg);
	char *out = (char *)malloc(error_msg_len * sizeof(char));

	if (out == NULL)
	{
		vidaw_error("Windows Error Conversion", "Could not allocated string", true);
	}

	strncpy(out, (char *)error_msg, error_msg_len);
	out[error_msg_len] = '\0';

	LocalFree(error_msg);
	return out;
}

static void init_usr_dir()
{
	if (_access(OUTPUT_DIR, 0) == 0)
	{
		SHFILEOPSTRUCT file_op = {0};
		file_op.wFunc = FO_DELETE;
		file_op.pFrom = OUTPUT_DIR;
		file_op.fFlags = FOF_NOCONFIRMATION | FOF_SILENT | FOF_NOERRORUI | FOF_ALLOWUNDO;

		int result = SHFileOperation(&file_op);

		if (result != 0)
		{
			vidaw_error("Remove Dir", "Could not delete", true);
		}
		
	}
	if (_mkdir(OUTPUT_DIR) != 0)
	{
		vidaw_error("USR DIR", convert_windows_error(), true);
	}
}

static void update_max_wav_len(file_state *p_file_state)
{
	uint32_t tmp = 0;
	for (int i = 0; i < p_file_state->cur_num_audio_files; i++)
	{
		if (p_file_state->wav_len[i] > tmp)
		{
			tmp = p_file_state->wav_len[i];
		}
	}
	p_file_state->max_wav_len = tmp;
}

void init_vidaw_state(vidaw_state *p_vidaw_state)
{
	if (SDL_Init(SDL_INIT_EVERYTHING) != 0)
	{
		vidaw_error("SDL init failed.", SDL_GetError(), true);
	}

	p_vidaw_state->p_win = SDL_CreateWindow(
		"VIDAW",
		SDL_WINDOWPOS_CENTERED,
		SDL_WINDOWPOS_CENTERED,
		WINDOW_WIDTH,
		WINDOW_HEIGHT,
		0);
	
	p_vidaw_state->window_h = WINDOW_HEIGHT;
	p_vidaw_state->window_w = WINDOW_WIDTH;

	if (p_vidaw_state->p_win == NULL)
	{
		vidaw_error("SDL_CreateWindow failed.", SDL_GetError(), true);
	}

	p_vidaw_state->p_ren = SDL_CreateRenderer(p_vidaw_state->p_win, -1, 0);

	if (p_vidaw_state->p_ren == NULL)
	{
		vidaw_error("SDL_CreateRenderer failed.", SDL_GetError(), true);
	}
	
	// memory allocation for the pointer to the linked list
	p_vidaw_state->head = (item **)malloc(sizeof(item **));
	if (p_vidaw_state->head == NULL)
	{
		vidaw_error("VIDAW init", "Could not allocote head", true);
	}

	// set first item to NULL
	*p_vidaw_state->head = NULL; 
	
	// memory allocation for id buffer
	p_vidaw_state->ids = (uint8_t *)malloc(WINDOW_WIDTH * WINDOW_HEIGHT * sizeof(uint8_t));
	if (p_vidaw_state->ids == NULL)
	{
		vidaw_error("VIDAW init", "Could not allocate buffers", true);	
	}
}

void close_vidaw_state(vidaw_state *p_vidaw_state)
{
	if (p_vidaw_state == NULL)
	{
		return;
	}

	free_item_list(p_vidaw_state->head);
	free(p_vidaw_state->head);
	free(p_vidaw_state->ids);
	SDL_DestroyRenderer(p_vidaw_state->p_ren);
	SDL_DestroyWindow(p_vidaw_state->p_win);
	SDL_Quit();
	free(p_vidaw_state);
	p_vidaw_state = NULL;
}

void init_file_state(file_state *p_file_state)
{
	// zero out necessary variables
	p_file_state->cur_num_audio_files = 0;
	p_file_state->main_pos = 0;
	p_file_state->max_wav_len = 0;

	// allocate and zero the main buffer for audio playing
	p_file_state->main_buf = (uint8_t *)malloc(MAIN_AUDIO_BUF_SIZE);
	memset(p_file_state->main_buf, 0, MAIN_AUDIO_BUF_SIZE);
}

void close_file_state(file_state *p_file_state)
{
	free(p_file_state->main_buf);

	for (int i = 0; i < MAX_NUM_AUDIO_FILES; i++)
	{
		if (p_file_state->wav_buf[i] != NULL)
		{
			SDL_FreeWAV(p_file_state->wav_buf[i]);
		}
	}

	free(p_file_state);
	p_file_state = NULL;
}

int main(int argc, char** args)
{
	bool run_program = true;
	bool audio_paused = true;
	vidaw_state *p_vidaw_state;
	file_state *p_file_state;
        SDL_AudioSpec desired_spec;
	SDL_AudioSpec obtained_spec;
	SDL_AudioDeviceID audio_device;

	// creates and manages local usr dir
	init_usr_dir();

	// allocate and init vidaw state
	p_vidaw_state = (vidaw_state *)calloc(1, sizeof(vidaw_state));
	if (p_vidaw_state == NULL)
	{
		vidaw_error("VIDAW", "State memory allocation failed", true);
	}
	init_vidaw_state(p_vidaw_state);

	// allocate and init file state 
	p_file_state = (file_state *)calloc(1, sizeof(file_state));
	if (p_file_state == NULL)
	{ 
		vidaw_error("FILE_STATE", "State memory allocation failed", true);
	}
	init_file_state(p_file_state);

	// Desired audio specification
	SDL_zero(desired_spec);
	desired_spec.freq = MAIN_SAMPLE_RATE;
	desired_spec.format = MAIN_AUDIO_FORMAT;
	desired_spec.channels = MAIN_NUM_CHANNELS;
	desired_spec.samples = MAIN_AUDIO_BUF_SIZE;
	desired_spec.callback = NULL;

	audio_device = SDL_OpenAudioDevice(NULL, 0, &desired_spec, &obtained_spec, 0);
	if (audio_device == 0)
	{
		vidaw_error("AUDIO SETUP", SDL_GetError(), true);
	}

	// call to create the background once before the loop
	graphics_init_background(p_vidaw_state);

	// force drop file to be accepted
	SDL_EventState(SDL_DROPFILE, SDL_ENABLE);

	// loop continues until user prompts exit
	while (run_program)
	{
		SDL_Event e;
		while (SDL_PollEvent(&e))
		{
			switch (e.type)
			{
				case (SDL_QUIT):
					run_program = false;
					break;
				case (SDL_MOUSEBUTTONDOWN):
					uint8_t *ids = p_vidaw_state->ids;
					uint8_t cur_id = *(ids + (e.button.y * WINDOW_WIDTH) + e.button.x);

					// play and pause functionality
					item_type cur_type = get_type_from_id(cur_id, p_vidaw_state->head);
					if (cur_type == PLAY_BUTTON)
					{
						audio_paused = false;
						SDL_PauseAudioDevice(audio_device, 0);
					}
					else if (cur_type == PAUSE_BUTTON)
					{
						audio_paused = true;
						SDL_PauseAudioDevice(audio_device, 1);
					}
					break; 
				case (SDL_DROPFILE):
					if (p_file_state->cur_num_audio_files >= MAX_NUM_AUDIO_FILES)
					{
						SDL_ShowSimpleMessageBox(
							SDL_MESSAGEBOX_ERROR,
							"Drop File",
							"Too many files",
							p_vidaw_state->p_win
						);
						break;
					} 
					
					// increment file #
					p_file_state->cur_num_audio_files++;

					// get basic info for file drop operations
					char *dropped_file_dir = e.drop.file;
					char *last_dot = strrchr(dropped_file_dir, '.');
					char *last_slash = strrchr(dropped_file_dir, '\\');
					
					if (last_dot == NULL || last_slash == NULL)
					{
						SDL_ShowSimpleMessageBox(
						SDL_MESSAGEBOX_ERROR,
						"Drop File",
						"Invalid File",
						p_vidaw_state->p_win
						);
						
						// clean up before break
						p_file_state->cur_num_audio_files--;
						SDL_free(dropped_file_dir);
						break;
					}
					
					// allocated temporary buffer for the file name w/o extensions
					size_t name_len = last_dot - last_slash;
					char *tmp_name_buf = (char *)malloc((name_len + 1) * sizeof(char));
					if (tmp_name_buf == NULL)
					{
						SDL_free(dropped_file_dir);
						vidaw_error("File Drop", "Could not allocated tmp name buf", true);
					}

					// copy and null terminate the tmp name buf
					strncpy(tmp_name_buf, last_slash, name_len);
					tmp_name_buf[name_len] = '\0';

					if (strcmp(last_dot, ".mp3") == 0 || strcmp(last_dot, ".wav") == 0)
					{
						// grab relevant variables from the file state struct
						size_t cur_index = p_file_state->cur_num_audio_files - 1;
						SDL_AudioSpec *cur_wav_spec =  &p_file_state->wav_spec[cur_index];
						uint32_t *cur_wav_len = &p_file_state->wav_len[cur_index];
						uint8_t **cur_wav_buf = &p_file_state->wav_buf[cur_index];

						char output_path[MAX_PATH] = "\0";
						char command[MAX_CL_ARG] = "\0";
						size_t path_len = name_len + strlen("\\") + strlen(OUTPUT_DIR) + strlen(".wav");

						// get current working dir
						if (_getcwd(output_path, (int)(MAX_PATH - path_len)) == NULL)
						{
							vidaw_error("File Drop", "path too long.", true);
						}
						
						strcat_s(output_path, MAX_PATH, "\\");
						strcat_s(output_path, MAX_PATH, OUTPUT_DIR);
						strcat_s(output_path, MAX_PATH, tmp_name_buf);
						strcat_s(output_path, MAX_PATH, ".wav");

						strcat_s(command, MAX_CL_ARG, "ffmpeg -i ");
						strcat_s(command, MAX_CL_ARG, dropped_file_dir);
						strcat_s(command, MAX_CL_ARG, " ");
						strcat_s(command, MAX_CL_ARG, output_path);

						FILE *fp = _popen(command, "r");
						if (fp == NULL)
						{
							vidaw_error("File", "Failed to convert mp3", true);
						}
						_pclose(fp);

						// finally call to the SDL function for loading the WAV
						if (SDL_LoadWAV(output_path, cur_wav_spec, cur_wav_buf, cur_wav_len) == NULL)
						{
							vidaw_error("Local WAV Loading", SDL_GetError(), true);
						}
						
						// NEED TO CONVERT DATA TO MAIN SPECIFICATIONSS
						if (!convert_audio_buf(cur_wav_spec, &obtained_spec, cur_wav_buf, cur_wav_len))
						{
							vidaw_error("AUDIO CONVERSION", "Could not convert wav spec.", true);
						}

						update_max_wav_len(p_file_state);
					}
					else
					{
						SDL_ShowSimpleMessageBox(
							SDL_MESSAGEBOX_ERROR,
							"Drop File",
							"File type not recognized",
							p_vidaw_state->p_win
						);
					}
					
					// finally load the wav file
					free(tmp_name_buf);
					SDL_free(dropped_file_dir);
					break;
			}
		}
		
		// audio process block
		if (p_file_state->cur_num_audio_files > 0 && !audio_paused)
		{
			for (int i = 0; i < p_file_state->cur_num_audio_files; i++)
			{
				for (int j = 0; j < MAIN_AUDIO_BUF_SIZE; j++)
				{	
					if (p_file_state->wav_len[i] >= p_file_state->main_pos + j)
					{
						p_file_state->main_buf[j] += p_file_state->wav_buf[i][p_file_state->main_pos + j];
					}
				}
			}
			if ((p_file_state->main_pos + MAIN_AUDIO_BUF_SIZE) <= p_file_state->max_wav_len)
			{
				p_file_state->main_pos += MAIN_AUDIO_BUF_SIZE;
			}
			else
			{
				p_file_state->main_pos = 0;
			}

			if (SDL_QueueAudio(audio_device, p_file_state->main_buf, MAIN_AUDIO_BUF_SIZE) < 0)
			{
				vidaw_error("AUDIO PROCESS", SDL_GetError(), true);
			}
			memset(p_file_state->main_buf, 0, MAIN_AUDIO_BUF_SIZE);
		}
		//SDL_Delay(1000);
	}

	// clean up and quit 
	close_vidaw_state(p_vidaw_state);
	return 0;
}
