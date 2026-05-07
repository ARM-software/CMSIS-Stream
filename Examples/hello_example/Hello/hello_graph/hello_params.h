#ifndef HELLO_PARAMS_H
#define HELLO_PARAMS_H

#include "RTE_Components.h"
#include CMSIS_device_header
#include "cmsis_os2.h"

typedef struct {
  osTimerId_t timer_id;
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