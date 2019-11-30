#pragma once

#include "Control.h"
#include "Audio.h"
#include "Effects/IEffect.h"
#include "Effects/LoopStation/LoopStation.h"

class Pedal
{
    public:

        static void begin()
        {
            LoopStation loop_station(2);

            Control control;
            control.add_effect(&loop_station);

            Audio audio(control);

            Pedal pedal(control, audio);
            set_instance(&pedal);

            // This will sleep this thread forever and wait for button events
            pedal.start();
        }

        void start()
        {
            m_audio.start();
            m_control.start();
        }

        void notify_btn_pressed(int btn_id)
        {
            m_control.btn_pressed(btn_id);
        }

        static Pedal* get_instance()
        {
            if(!m_pedal_instance)
                throw std::runtime_error("Pedal does not exist!");

            return m_pedal_instance;
        }

        static Pedal* m_pedal_instance;
        
    private:

        Pedal(IControl& control, IAudio& audio) : m_control(control), m_audio(audio) 
        {
        }

        static void set_instance(Pedal* pedal)
        {
            m_pedal_instance = pedal;
        }

        IControl& m_control;
        IAudio& m_audio;
};

