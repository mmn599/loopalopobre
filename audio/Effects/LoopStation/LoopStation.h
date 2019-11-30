#pragma once

#include "../../Sample.h"
#include "../IEffect.h"
#include "Track.h"
#include <vector>
#include <numeric>
#include <iostream>
#include <cmath>

class LoopStation: public IEffect
{
public:
    LoopStation(int num_tracks): 
        m_num_tracks(num_tracks), 
        m_num_btns_required(num_tracks * 2),
        master_track_size(-1),
        master_track_id(-1)
    {
        for(int i=0;i<num_tracks;i++)
            m_tracks.push_back(Track(i));
    }

    void notify_btn_event(int internal_btn_index)
    {
        int track_num = internal_btn_index >> 1;
        auto& track = m_tracks[track_num];
        bool stop = (internal_btn_index & 0x1); 

        auto track_state = stop ? track.btn_stop() : track.btn_play_record();

        if (track_state != TrackState::RECORDING && master_track_id == -1) // Check if first recording is being saved
        {
            master_track_size = track.get_size();
            master_track_id = track_num;
            std::cout << "Master track id: " << master_track_id << "\n";
        }
        else if (master_track_id != -1 && track_state == TrackState::PLAYING) // Check if a track needs to be resized to master
        {
            auto& master_track = m_tracks[master_track_id];
            track.resize(master_track.get_size());
            track.set_sample_index(master_track.get_sample_index());
            std::cout << "Resizing to master track.\n";
        }
    }

    sample_t generate_output(sample_t input_sample)
    {
        std::vector<sample_t> track_outputs(m_num_tracks);

        for(int track_num=0;track_num<m_tracks.size();track_num++)
            track_outputs[track_num] = m_tracks[track_num].notify_input(input_sample);

        sample_t all_tracks = get_tracks_sample(track_outputs);
        return combine_output(input_sample, all_tracks);
    }

    int get_num_btns_required() const
    {
        return m_num_btns_required;
    }

private:
    sample_t get_tracks_sample(std::vector<sample_t> track_outputs) const
    {
        // TODO: adjustable     
        sample_t sum = 0;
        for(sample_t sample: track_outputs)
            sum += sample / track_outputs.size();
        return sum;
    } 

    sample_t combine_output(sample_t input_sample, sample_t tracks_output) const
    {
        return input_sample / 2.0 + tracks_output / 2.0;
    }

    int m_num_btns_required;
    int m_num_tracks;
    int master_track_size;
    int master_track_id;
    std::vector<Track> m_tracks;
};
