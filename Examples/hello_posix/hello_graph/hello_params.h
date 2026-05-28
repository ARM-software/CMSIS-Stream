#ifndef HELLO_PARAMS_H
#define HELLO_PARAMS_H

#include <stdint.h>

typedef struct {
    uint32_t delay_ms;
} hardwareParams_t;

typedef struct {
    hardwareParams_t hw_;
    int ticks;
    float val;
} emptySourceParams_t;

typedef struct {
    emptySourceParams_t src;
} helloParams_t;

extern helloParams_t helloParams;

#endif
