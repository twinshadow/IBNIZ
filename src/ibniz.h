#include <stdint.h>

#ifndef IBNIZ_H
#define IBNIZ_H

#ifdef IBNIZ_MAIN
#define GLOBAL
#else
#define GLOBAL extern
#endif

#define IBNIZ_VERSION "1.1C00-NORELEASE"

#ifdef VERBOSE
#define DEBUG(__msg, ...) fprintf(stderr, "DEBUG: %s:%d: " __msg "\n", \
		__FILE__, __LINE__, ##__VA_ARGS__)
#else
#define DEBUG(__msg, ...)
#endif

#include "vm.h"

#define WIDTH 256
#define EDITBUFSZ (65536*2)

#ifndef WIN32
#define PLAYBACKGAP 4
#else
#define PLAYBACKGAP 16
#endif

/* i/o stuff used by vm */
uint32_t gettimevalue();
int getuserchar();
void waitfortimechange();

/* vm-specific */
void vm_compile(char *src);
void vm_init();
int vm_run();
void switchmediacontext();

/* compiler */
void compiler_parse(char *src);
int compiler_compile();

/* clipboard */
GLOBAL char *clipboard;
void clipboard_load();
void clipboard_store();

#if defined(WIN32)
#define  CLIPBOARD_WIN32
#elif defined(X11)
#define  CLIPBOARD_X11
#include <SDL/SDL.h>
void clipboard_handlesysreq(SDL_Event * e);

#else
#define  CLIPBOARD_NONE
#endif

void compiler_parser(char *src);

#endif
