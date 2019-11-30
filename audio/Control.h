#pragma once

#include "IControl.h"

#include <vector>
#include <mutex>
#include <map>
#include <chrono>

#include <thread>

#include "Effects/IEffect.h"

class Control : public IControl
{
    public:

        Control() : m_btn_index(0)
        {
        }

        Control(const Control& that) = delete;

        void btn_pressed(int btn_id)
        {
            std::lock_guard<std::mutex> lock(m_mutex);
            std::map<int, IEffect*>::iterator it = m_btn_map.find(btn_id);
            if(it != m_btn_map.end())
            {
                IEffect* effect = it->second;
                int internal_btn_index = btn_id - m_first_btn_index[effect];
                effect->notify_btn_event(internal_btn_index);
            }
        }

        void start()
        {
            std::this_thread::sleep_for(std::chrono::hours(std::numeric_limits<int>::max()));
        }

        void add_effect(IEffect* effect)
        {
            int num_btns_requested = effect->get_num_btns_required();
            m_effects.push_back(effect);
            m_first_btn_index[effect] = m_btn_index;
            for(int i=0;i<num_btns_requested;i++)
            {
                m_btn_map[m_btn_index++] = effect;
            }
        }

        sample_t get_next_output(sample_t live_sample)
        {
            sample_t current_output = live_sample;
            for(IEffect* effect: m_effects)
            {
                current_output = effect->generate_output(current_output);
            }
            return current_output;
        }

        std::mutex& get_mutex()
        {
            return m_mutex;
        }

    private:

        std::mutex m_mutex;

        std::vector<IEffect*> m_effects;

        std::map<int, IEffect*> m_btn_map;
        std::map<IEffect*, int> m_first_btn_index;
        int m_btn_index;

};
