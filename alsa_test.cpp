/*
Project Name: PoDAW - Portable Digital Audio Workstation
File Name: als_test.cpp
File Description: A test of the ALSA audio library, using initial button to create sound.
Developer Name(s): Jonathan Xue

*/

#include <alsa/asoundlib.h>
#include <gpiod.hpp>
#include <iostream>
#include <vector>

// Configuration
const char* PCM_DEVICE = "default"; // Change to "hw:1,0" or similar for your specific Audio HAT
const std::string CHIP_NAME = "gpiochip4";
const int BUTTON_PIN = 17;

int main() {
    int err;
    snd_pcm_t *pcm_handle;
    
    // 1. Initialize ALSA PCM Device for Playback
    if ((err = snd_pcm_open(&pcm_handle, PCM_DEVICE, SND_PCM_STREAM_PLAYBACK, 0)) < 0) {
        std::cerr << "Cannot open audio device: " << snd_strerror(err) << std::endl;
        return 1;
    }

    // Set hardware parameters (44.1kHz, 16-bit, Mono)
    snd_pcm_set_params(pcm_handle,
                       SND_PCM_FORMAT_S16_LE,
                       SND_PCM_ACCESS_RW_INTERLEAVED,
                       1,      // Channels
                       44100,  // Sample Rate
                       1,      // Allow software resampling
                       50000); // Latency requirement in microseconds (50ms)

    // 2. Pre-load your audio into memory (Simulated here with a 1-second silent buffer)
    // In reality, you'd parse a .wav file or synthesize a sine wave into this vector.
    int frames = 44100; // 1 second of audio at 44.1kHz
    std::vector<int16_t> audio_buffer(frames, 0); 

    // 3. Initialize GPIO Button
    gpiod::chip chip(CHIP_NAME);
    auto line = chip.get_line(BUTTON_PIN);
    gpiod::line_request config;
    config.request_type = gpiod::line_request::EVENT_FALLING_EDGE;
    config.consumer = "piano_key";
    line.request(config);

    std::cout << "Digital Piano Ready. Press the button to play." << std::endl;

    // 4. Main Event Loop
    while (true) {
        // Wait for key press
        line.event_wait(std::chrono::seconds(10));
        auto event = line.event_read();

        if (event.event_type == gpiod::line_event::FALLING_EDGE) {
            std::cout << "Key pressed! Writing buffer to ALSA..." << std::endl;
            
            // Push the pre-loaded buffer to the Audio HAT
            snd_pcm_sframes_t frames_written = snd_pcm_writei(pcm_handle, audio_buffer.data(), frames);
            
            if (frames_written < 0) {
                // Buffer underrun (XRUN) recovery
                snd_pcm_recover(pcm_handle, frames_written, 0);
            }
        }
    }

    snd_pcm_close(pcm_handle);
    return 0;
}