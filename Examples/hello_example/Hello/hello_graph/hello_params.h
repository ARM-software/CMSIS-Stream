#ifndef HELLO_PARAMS_H
#define HELLO_PARAMS_H

typedef struct {
    float val;
} emptySourceParams_t;


typedef struct {
    emptySourceParams_t src;
} helloParams_t;

extern helloParams_t helloParams;

#endif