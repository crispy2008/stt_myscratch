#include <iostream>
#include <vector>
#include <string>
// its a speech-to-text demo with whisper.cpp

// all path invalid here since i didnt put whisper.cpp in this workspace
#include "whisper.h" // whisper.cpp/include/whisper.h

#include "common.h"
#include "common-whisper.h"
#include "httplib.h"
#include "json.hpp"
// whisper.cpp/examples/server/server.cpp

// dummy
std::vector<float> load_audio(const std::string &path) {
    // TODO: Load a 16-bit PCM mono 16kHz WAV file and return as float array
    // Here we just return an empty placeholder
    return std::vector<float>();
}
// use ffmpeg?

int main() {
    // Load model
    const char* model_path = "models/ggml-base.en.bin";
    struct whisper_context* ctx = whisper_init_from_file(model_path);
    if (!ctx) {
        std::cerr << "Failed to load model!" << std::endl;
        return 1;
    }

    // Load audio???? 
    std::vector<float> audio = load_audio("audio.wav");
    if (audio.empty()) {
        std::cerr << "Failed to load audio!" << std::endl;
        whisper_free(ctx);
        return 1;
    }

    // Setup transcription parameters
    whisper_full_params params = whisper_full_default_params();

    // Run api
    if (whisper_full(ctx, params, audio.data(), audio.size()) != 0) { // audio in pcmf32
        std::cerr << "Failed to process audio" << std::endl;
        whisper_free(ctx);
        return 7;
    }

    int n_segments = whisper_full_n_segments(ctx);
    for (int i = 0; i < n_segments; ++i) {
        const char* text = whisper_full_get_segment_text(ctx, i);
        std::cout << text << std::endl;
    }

    whisper_free(ctx);
    return 0;
}



