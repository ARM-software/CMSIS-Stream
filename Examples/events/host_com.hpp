#pragma once

#include "StreamNode.hpp"
#include <iostream>

#define MAX_NB_HOST_MSGS 50

#if defined(CG_HOST)
extern void listen_to_host();
extern void close_host();
extern void send_data_to_host(int nodeid, const char *data, int size);
extern void send_event_to_host(int nodeid,const arm_cmsis_stream::Event &evt);

extern std::ostream &operator<<(std::ostream &os, const arm_cmsis_stream::Event &obj);

struct Host
{
    template <typename... Args>
    static void send(int nodeid,uint32_t selector,Args &&...args)
    {
        arm_cmsis_stream::Event evt(selector, kNormalPriority,std::forward<Args>(args)...);
        send_event_to_host(nodeid,std::move(evt));
    }
};

#endif