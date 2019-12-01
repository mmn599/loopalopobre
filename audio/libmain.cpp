#include "Pedal.h"

#include <thread>

Pedal* Pedal::pedal_instance = nullptr;

extern "C" void begin() 
{
    std::thread pedal_thread(Pedal::begin);
    pedal_thread.detach();
}

extern "C" void btn_pressed(int btn_id)
{
    auto pedal = Pedal::get_instance();
    pedal->notify_btn_pressed(btn_id);
}


