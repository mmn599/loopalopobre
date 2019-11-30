#include "Pedal.h"

#include <thread>

extern "C" void begin() 
{
    std::thread pedal_thread(Pedal::begin);
    pedal_thread.detach();
}

extern "C" void btn_pressed(int btn_id)
{
    Pedal* instance = Pedal::get_instance();
    instance->notify_btn_pressed(btn_id);
}


