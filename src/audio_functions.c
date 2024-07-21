#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <stdint.h>
#include <SDL.h> 
#include "audio_functions.h"

static uint8_t *resize_wav_buf(uint8_t **buf, uint32_t *buf_len, int resize_factor)
{
	// the original buf len is # of bytes
	uint32_t new_len = *buf_len * (uint32_t)resize_factor;
	uint8_t *new_buf = (uint8_t *)malloc(new_len);

	// zero out data
	memset(new_buf, 0, new_len);

	//copy
	memcpy(new_buf, *buf, *buf_len);

	// free old buffer, update len, and return new
	SDL_FreeWAV(*buf);
	*buf_len = new_len;
	return new_buf;
}

bool convert_audio_buf(SDL_AudioSpec *wav_spec, SDL_AudioSpec *main_spec, uint8_t **buf, uint32_t *buf_len)
{
	SDL_AudioCVT cvt;
	SDL_BuildAudioCVT(
		&cvt,
		wav_spec->format,
		wav_spec->channels,
		wav_spec->freq,
		main_spec->format,
		main_spec->channels,
		main_spec->freq
	);

	if (!cvt.needed)
	{ 
		//debug
		printf("conversion not possible");
		return false;
	}

	// original buffer size = len
	cvt.len = *buf_len;

	// if more space needed, resize the buffer 
	if (cvt.len_mult > 1)
	{
		*buf = resize_wav_buf(buf, buf_len, cvt.len_mult);
	}

	cvt.buf = *buf;

	SDL_ConvertAudio(&cvt);
	return true;
}
