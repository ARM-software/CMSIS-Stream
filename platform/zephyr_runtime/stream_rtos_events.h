#ifndef STREAM_RTOS_EVENTS_H
#define STREAM_RTOS_EVENTS_H

/* Messages to stream and event threads */
#define STREAM_PAUSE_EVENT (1<<0)
#define STREAM_RESUME_EVENT (1<<1)
#define EVENT_RESUME_EVENT (1<<2)
#define STREAM_DONE_EVENT (1<<3)

/* Messages from stream and event threads*/
#define STREAM_PAUSED_EVENT (1<<0)
#define STREAM_RESUMED_EVENT (1<<1)
#define EVENT_PAUSED_EVENT (1<<2)
#define EVENT_RESUMED_EVENT (1<<3)
#endif