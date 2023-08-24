#include <cstdio>
#include <cstdint>
#include "static_scheduler.h"
#include "dynamic_scheduler.h"
#include <memory>

int main(int argc, char const *argv[])
{
    std::shared_ptr<float> sp( new float[40], std::default_delete<float[]>() );
    int error;
    printf("Start %zd\n",sizeof(sp));
    uint32_t nbSched=static_scheduler(&error);
    nbSched=dynamic_scheduler(&error);

    return 0;
}