#ifndef CONFIG_EVENTS_H_
#define CONFIG_EVENTS_H_


// Enable CG_EVENTS to use the event system
// When disabled, events are not sent
#define CG_EVENTS

// Enable threaded events
// Posix implementation provided as example
// An RTOS can provide its own implementation
#define CG_EVENTS_MULTI_THREAD

// Example of communication with the HOST
// Multi threading must be enabled
// The files for host communications are only part of this
// example and are not provided in the CMSIS-Stream python
// package
#define CG_HOST


#endif