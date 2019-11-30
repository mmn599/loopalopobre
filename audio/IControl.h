#pragma once

#include "Sample.h"
#include <mutex>

class IControl
{

public:
    virtual void start() = 0;

    virtual std::mutex& get_mutex() = 0;

    // Takes in a live sample, generates next output
    virtual sample_t get_next_output(sample_t live_sample) = 0;

    virtual void btn_pressed(int btn_id) = 0;
};
