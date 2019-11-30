#pragma once

#include "../Sample.h"

class IEffect 
{
public:
    virtual int get_num_btns_required() const = 0;
    virtual void notify_btn_event(int internal_btn_index) = 0;
    virtual sample_t generate_output(sample_t input_sample) = 0;
};