#pragma once

#include "Sample.h"
#include "IControl.h"
#include "IAudio.h"
#include "portaudio.h"
#include <stdexcept>
#include <iostream>
#include <mutex>

class Audio: public IAudio
{
    public:

        Audio(IControl& control) : m_control(control), m_samples_per_buffer(128), 
                        m_sample_rate(44100), m_num_pa_channels(1), 
                        m_max_loop_seconds(120)
        {
        }

        void start()
        {
            // TODO: clean this up
            auto error = Pa_Initialize();
            if (error != paNoError) 
                throw std::runtime_error("Cannot init stream.");

            setup_input();
            setup_output();

            error = Pa_OpenStream(&m_pa_stream, &m_input_params, 
                                    &m_output_params, m_sample_rate, 
                                    m_samples_per_buffer, paClipOff,
                                    &pa_callback, &m_control);
            if(error != paNoError) 
                throw std::runtime_error("Cannot open stream.");

            error = Pa_StartStream(m_pa_stream);
            if(error != paNoError) 
                throw std::runtime_error("Cannot start stream.");
        }

        void stop()
        {
            auto error = Pa_StopStream( m_pa_stream );
            if(error != paNoError) 
                throw std::runtime_error("Cannot stop stream.");

            error = Pa_CloseStream( m_pa_stream );
            if(error != paNoError) 
                throw std::runtime_error("Cannot close stream.");

            Pa_Terminate();
        }

    private:

        static sample_t get_sample(const sample_t* input_buffer, int frame_num)
        {
            // Offset of 0 is left channel.
            // TODO: left channel is useless. need to make the stream mono.
            return input_buffer[(frame_num << 1) + 1];
        }

        static void set_output(sample_t* output_buffer, int frame_num, sample_t value)
        {
            output_buffer[frame_num << 1] = value;
            output_buffer[(frame_num << 1) + 1] = value;
        }

        void setup_input()
        {
            m_input_params.device = Pa_GetDefaultInputDevice();
            PaDeviceInfo* input_info = (PaDeviceInfo*) Pa_GetDeviceInfo(m_input_params.device);
            m_input_params.channelCount = m_num_pa_channels;
            m_input_params.sampleFormat = paFloat32;
            m_input_params.suggestedLatency = input_info->defaultLowInputLatency;
            m_input_params.hostApiSpecificStreamInfo = 0;
        }

        void setup_output()
        {
            m_output_params.device = Pa_GetDefaultOutputDevice();
            PaDeviceInfo* output_info = (PaDeviceInfo*) Pa_GetDeviceInfo(m_output_params.device);
            m_output_params.channelCount = m_num_pa_channels;
            m_output_params.sampleFormat = paFloat32;
            m_output_params.suggestedLatency = output_info->defaultLowOutputLatency;
            m_output_params.hostApiSpecificStreamInfo = 0;
        }

        PaStreamParameters m_input_params;
        PaStreamParameters m_output_params;
        PaStream* m_pa_stream;
        int m_samples_per_buffer;
        int m_sample_rate;
        int m_num_pa_channels;
        int m_max_loop_seconds;

        static int pa_callback(const void* input_buffer, void* output_buffer,
                                    unsigned long frames_per_buffer,
                                    const PaStreamCallbackTimeInfo* time_info,
                                    PaStreamCallbackFlags status_flags,
                                    void* user_data)
        {
            auto control = (IControl*) user_data;

            std::lock_guard<std::mutex> lock(control->get_mutex());

            for(int frame_num=0; frame_num<frames_per_buffer; frame_num++)
            {
                auto live_sample = get_sample((sample_t*)input_buffer, frame_num);
                auto output_sample = 5 * control->get_next_output(live_sample);
                set_output((sample_t*) output_buffer, frame_num, output_sample);
            }

            return paContinue;
        }

        IControl& m_control;

};
