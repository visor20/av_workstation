#ifndef AUDIO_FUNCTIONS_H
#define ADUIO_FUNCTIONS_H

#include <stdio.h>
#include <stdint.h>
#include <SDL.h>

/* This function mainly uses the SDL_AudioCVT struct 
 * it converts the data in "buf" from "wav_spec" to the "main_spec"
 */
bool convert_audio_buf(SDL_AudioSpec *wav_spec, SDL_AudioSpec *main_spec, uint8_t **buf, uint32_t *buf_len);

#endif // AUDIO_FUNCTIONS_H
