#include "vector.h"
#include "utilities.h"

#include <stdlib.h>
#include <assert.h>
#include <string.h>

#define DEFAULT_CAPACITY 10
#define CAPACITY_RATE 1.5f

void vector_new(vector* v) { LOG
    assert(v);

    v->count = 0;
    v->capacity = 0;
    v->buffer = NULL;
}

void vector_free(vector* v) { LOG
    assert(v);

    free(v->buffer);
}

int vector_size(vector* v) { LOG
    assert(v);

    return v->count;
}

int vector_capacity(vector* v) { LOG
    assert(v);

    return v->capacity;
}

void vector_push_back(vector* v, void* data) { LOG
    assert(v);
    assert(data);

    if (!v->capacity) {
        v->capacity = DEFAULT_CAPACITY;
        v->buffer = (void**)malloc(v->capacity * sizeof(void*));
        memset(v->buffer, 0, v->capacity * sizeof(void*));
    }

    if (v->capacity == v->count) {
        v->capacity *= CAPACITY_RATE;
        v->buffer = (void**)realloc(v->buffer, v->capacity * sizeof(void*));
        assert(v->buffer);
    }

    v->buffer[v->count] = data;
    v->count++;
}

void vector_insert(vector* v, void* data, int index) { LOG
    assert(v);
    assert(index >= 0);
    assert(index < v->count);

    if (v->capacity == v->count + 1) {
        v->capacity *= CAPACITY_RATE;
        v->buffer = realloc(v->buffer, v->capacity * sizeof(void*));
        assert(v->buffer);
    }

    int i;
    for (i = v->count; i >= index; --i) {
        v->buffer[i + 1] = v->buffer[i];
    }

    v->buffer[index] = data;
    v->count++;
}

void* vector_get(vector* v, int index) { LOG
    assert(v);
    assert(index >= 0);
    assert(index < v->count);

    return v->buffer[index];
}

void vector_erase(vector* v, int index) { LOG
    assert(v);
    assert(index >= 0);
    assert(index < v->count);

    if (index != v->count -1) {
        int i;
        for (i = index; i < v->count - 1; ++i)
            v->buffer[i] = v->buffer[i+1];
    }
    
    v->buffer[v->count - 1] = NULL;
    v->count --;
}
