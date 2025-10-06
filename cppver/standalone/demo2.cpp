/* 1. opens the microphone, //ffmpeg* or portaudio
   2. calls the whisper-server with a POST request*/ //curl or httplib*
   // guess: curl request will be easier since we dun need to convert to text then post.

#include <iostream>
#include <cstdlib>
#include <fstream>
#include <vector>
#include "httplib.h"

// convert .wav(nontext) to text
std::string read_file_binary(const std::string& path) {
    // read into memory
        // open .wav
    std::ifstream ifs(path, std::ifstream::binary);
    if (!ifs) {
        std::cerr << "Failed to open file: " << path << std::endl;
        return "";
    }
        // get length of the file
    ifs.seekg(0, ifs.end); // move ptr to end
    int length = ifs.tellg();
    ifs.seekg(0, ifs.beg); // move ptr back
        // allocate memory
    char* buffer = new char[length];
        // read as text
    ifs.read(buffer, length);
        // close .wav
    ifs.close();

    std::string result = std::string(buffer, length);

    delete[] buffer;
    return result;
}

void record_audio_with_ffmpeg() {
#ifdef _WIN32
    std::string cmd = "ffmpeg -f dshow -i audio=\"Microphone (Realtek Audio)\" -t 5 -ac 1 -ar 16000 -sample_fmt s16 audio.wav";
#elif __APPLE__
    std::string cmd = "ffmpeg -f avfoundation -i \":0\" -t 5 -ac 1 -ar 16000 -sample_fmt s16 audio.wav";
#else
    std::string cmd = "ffmpeg -f alsa -i default -t 5 -ac 1 -ar 16000 -sample_fmt s16 audio.wav";
#endif

    std::cout << "Recording... Please say something\n";
    int result = std::system(cmd.c_str()); // use ffmpeg by command line
    if (result != 0) {
        std::cerr << "Failed to record audio\n";
    } else {
        std::cout << "Audio recorded: audio.wav\n"; // save as audio.wav // see path?
    }
}

void post_to_whisper_server() {
    httplib::Client cli("localhost", 8080);

    std::string audio_data = read_file_binary("audio.wav");
    if (audio_data.empty()) return;

    httplib::MultipartFormDataItems items = {
        { "audio_file",   // field name
            audio_data,   // content
            "audio.wav",  // just a name 
            "audio/wav" } // content type
    };

    auto res = cli.Post("/inference", items);
    if (res && res->status == 200) {
        std::cout << "Whisper Transcription:\n" << res->body << std::endl;
    } else {
        std::cerr << "Failed to POST to whisper-server\n";
        if (res) std::cerr << "Status: " << res->status << std::endl;
    }
}

int main() {
    record_audio_with_ffmpeg();
    post_to_whisper_server();
    return 0;
}



