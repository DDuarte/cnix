#ifndef WINDOW_H
#define WINDOW_H

typedef struct {
    char* title;
    int width;
    int height;
} window_t;


int window_draw(window_t* window);
int window_set_title(window_t* window, const char* format, ...);
int window_set_size(window_t* window, int width, int height);

#endif /* WINDOW_H */
