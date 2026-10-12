/* Select the private SDL3 playback device for automated audio recordings. */
#define _GNU_SOURCE
#include <dlfcn.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    unsigned format;
    int channels;
    int freq;
} AudioSpec;

static void *sdl_symbol(const char *name) {
    /* RMG loads SDL locally. Resolve from its library rather than global scope. */
    void *library = dlopen("libSDL3.so.0", RTLD_LAZY | RTLD_NOLOAD);
    void *function = library == NULL ? NULL : dlsym(library, name);
    if (function == NULL) {
        fprintf(stderr, "Silent RMG output cannot resolve %s\n", name);
        abort();
    }
    dlclose(library);
    return function;
}

void *SDL_OpenAudioDeviceStream(uint32_t device, const AudioSpec *spec, void *callback,
                                void *userdata) {
    void *(*open_stream)(uint32_t, const AudioSpec *, void *, void *) =
        sdl_symbol("SDL_OpenAudioDeviceStream");
    const char *required = getenv("SM64_SILENT_AUDIO_DEVICE");
    if (required == NULL || required[0] == '\0') {
        fprintf(stderr, "Silent RMG output requires a private playback device\n");
        return NULL;
    }
    uint32_t *(*get_devices)(int *) = sdl_symbol("SDL_GetAudioPlaybackDevices");
    const char *(*get_name)(uint32_t) = sdl_symbol("SDL_GetAudioDeviceName");
    void (*free_devices)(void *) = sdl_symbol("SDL_free");
    int count = 0;
    uint32_t *devices = get_devices(&count);
    device = 0;
    for (int i = 0; devices != NULL && i < count; i++) {
        const char *name = get_name(devices[i]);
        if (name != NULL && strcmp(name, required) == 0) {
            device = devices[i];
            break;
        }
    }
    free_devices(devices);
    if (device == 0) {
        fprintf(stderr, "Private RMG audio device unavailable: %s; refusing speaker output\n",
                required);
        return NULL;
    }
    fprintf(stderr, "Silent RMG audio device selected: %s\n", required);
    return open_stream(device, spec, callback, userdata);
}
