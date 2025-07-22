#ifndef CONFIG_EVENTS_H_
#define CONFIG_EVENTS_H_


// Enable CG_EVENTS to use the event system
// When disabled, events are not sent
#define CG_EVENTS


// Enable threaded events
// Posix implementation provided as example
// An RTOS can provide its own implementation
#define CG_EVENTS_MULTI_THREAD


#endif