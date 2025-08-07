#ifndef CONFIG_EVENTS_H_
#define CONFIG_EVENTS_H_

// Enable event system
// Can be used in your main app to set the event queue or
// keep it to a nullptr value
#define CG_EVENTS

// Enable threaded events
// Posix implementation provided as example
// An RTOS can provide its own implementation
#define CG_EVENTS_MULTI_THREAD


#endif