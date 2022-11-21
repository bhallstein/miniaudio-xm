#define MINIAUDIO_IMPLEMENTATION
#define JAR_XM_IMPLEMENTATION
#define XM_DATA_SOURCE_IMPLEMENTATION
#include "xm-data-source.h"

#include <stdio.h>

int main(int argc, char** argv) {
  ma_result result;
  ma_engine engine;
  ma_sound sound;

  if (argc < 2) {
    printf("No input file\n");
    return 1;
  }
  printf("Playing file: %s\n", argv[1]);

  result = ma_engine_init(NULL, &engine);
  if (result != MA_SUCCESS) {
    printf("Failed to initialize audio engine\n");
    return 1;
  }

  xm_data_source xmds;
  result = xm_data_source_init_file(argv[1], &xmds);
  if (result != MA_SUCCESS) {
    printf("x1\n");
    return 1;
  }

  result = ma_sound_init_from_data_source(&engine, &xmds, 0, NULL, &sound);
  if (result != MA_SUCCESS) {
    printf("x2\n");
    return 1;
  }

  ma_sound_start(&sound);

  do {
    sleep(1);
  } while (!ma_sound_at_end(&sound));

  xm_data_source_uninit(&xmds);
  ma_engine_uninit(&engine);

  printf("Bye!\n");
  return 0;
}
