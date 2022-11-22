# miniaudio-xm 🎵

An xm module data source for [miniaudio](https://github.com/mackron/miniaudio).

Uses [jar](https://github.com/kd7tck/jar) for xm decoding. (Here's a [C++-compatible jar fork](https://github.com/bhallstein/jar).)


### Compile and run the example

- Clone [miniaudio](https://github.com/mackron/miniaudio) and [jar](https://github.com/kd7tck/jar) into the top level directory
  - `git clone https://github.com/mackron/miniaudio.git`
  - `git clone https://github.com/kd7tck/jar.git`
- `cd example`
- `bash compile.sh`
  - Or: `clang main.c -I .. -I ../miniaudio -I ../jar -o xmds`
- `./xmds happy-shopping.xm` (or any .xm file)

(NB: If you need C++ compatibility, clone jar from `https://github.com/bhallstein/jar.git` instead.)


### Example code

```c
ma_result result;
xm_data_source xmds;

result = xm_data_source_init_file("music.xm", &xmds);
if (result != MA_SUCCESS) {
  return 1;
}

result = ma_sound_init_from_data_source(&engine, &xmds, 0, NULL, &sound);
if (result != MA_SUCCESS) {
  return 1;
}

ma_sound_start(&sound);
```

Miniaudio, jar, and miniaudio-xm are header-only. **Exactly one** .c (or .cpp) file must include the implementation directives:

```c
#define MINIAUDIO_IMPLEMENTATION
#define JAR_XM_IMPLEMENTATION
#define XMDS_IMPLEMENTATION
```

Code review/suggestions/pulls welcome :)

