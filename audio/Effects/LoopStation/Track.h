#pragma once

#include "../../Sample.h"
#include <vector>
#include <iostream>
#include <string>

enum TrackState
{
    STOPPED,
    RECORDING,
    PLAYING,
    OVERDUBBING
};
static const char* STATE_STRINGS[] = {"stopping", "recording", "playing","overdubbing"};

class Track
{
public:
    Track(int track_id) : 
        m_track_id(track_id), 
        m_state(TrackState::STOPPED), 
        m_sample_index(0)
    {
    }

    TrackState btn_stop()
    {
        if(m_state == TrackState::STOPPED)
        {
            state_wipe();
        }
        else if(m_state == TrackState::RECORDING || m_state == TrackState::OVERDUBBING)
        {
            state_play();
        }
        else if(m_state == TrackState::PLAYING)
        {
            state_stop();
        }

        return m_state;
    }

    TrackState btn_play_record()
    {
        if(m_state == TrackState::STOPPED && has_recorded())
        {
            state_play();
        }
        else if(m_state == TrackState::STOPPED && !has_recorded())
        {
            state_record();
        }
        else if(m_state == TrackState::RECORDING || m_state == TrackState::PLAYING)
        {
            state_overdub();
        }

        return m_state;
    }

    TrackState get_state() const
    {
        return m_state;
    }

    // If playing, returns next output.
    // If recording, records input_sample and returns 0.0.
    // If stopped, returns 0.0.
    sample_t notify_input(sample_t input_sample)
    {
        sample_t current_sample = 0.0;

        if(m_state == TrackState::PLAYING)
        {
            current_sample = get_sample();
        }         
        else if(m_state == TrackState::RECORDING)
        {
            record(input_sample);
        }
        else if(m_state == TrackState::OVERDUBBING)
        {
            current_sample = overdub(input_sample);
        }

        return current_sample;
    }

    int get_size()
    {
        return m_samples.size(); 
    }

    int get_sample_index()
    {
        return m_sample_index;
    }

    void set_sample_index(int index)
    {
        m_sample_index = index; 
    }

    void resize(int size)
    {
        m_samples.resize(size);
    }

private:

    void log_change_state()
    {
        std::cout << "Track " << m_track_id << " is " << STATE_STRINGS[m_state] << "\n";
    }

    void change_state(TrackState new_state)
    {
        m_state = new_state;
        log_change_state();
    }

    void state_play()
    {
        change_state(TrackState::PLAYING);
    }

    void state_record()
    {
        change_state(TrackState::RECORDING);
    }

    void state_wipe()
    {
        m_samples.clear();        
        state_stop();
    }

    void state_overdub()
    {
        if(m_samples.size() == 0)
        {
            state_record();
        }
        else
        {
            change_state(TrackState::OVERDUBBING);
        }
    }

    void state_stop()
    {
        m_sample_index = 0;
        change_state(TrackState::STOPPED);
    }

    sample_t overdub(sample_t input_sample)
    {
        sample_t current = m_samples[m_sample_index];
        // TODO: adjustable
        m_samples[m_sample_index] = .75 * current + .5 * input_sample;
        m_sample_index = (m_sample_index + 1) % m_samples.size();
        return current;
    }

    void record(sample_t input_sample)
    {
        m_samples.push_back(input_sample);
    }

    sample_t get_sample()
    {
        sample_t current = m_samples[m_sample_index];
        m_sample_index = (m_sample_index + 1) % m_samples.size();
        return current;
    }

    bool has_recorded()
    {
        return m_samples.size() > 0;
    }

    TrackState m_state;
    int m_track_id;
    std::vector<sample_t> m_samples;
    int m_sample_index;

};
