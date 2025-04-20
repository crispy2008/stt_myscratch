#include <iostream>
#include <cstdlib>
#include <cstdio>
#include <string>
#include "json.hpp"

using json = nlohmann::json;

void record_audio_with_ffmpeg() { // doesnt work: we use provided sample here to test
    std::string cmd =
        "ffmpeg -y -f alsa -i default -t 5 -ac 1 -ar 16000 -sample_fmt s16 audio.wav";
    std::cout << "Recording audio...\n";
    int result = std::system(cmd.c_str());
    if (result != 0) {
        std::cerr << "FFmpeg failed.\n";
    }
}

std::string run_command(const std::string& cmd) {
    std::string result;
    char buffer[4096];
    FILE* pipe = popen(cmd.c_str(), "r");
    if (!pipe) {
        std::cerr << "Failed to run command.\n";
        return "";
    }
    while (fgets(buffer, sizeof(buffer), pipe)) {
        result += buffer;
    }
    pclose(pipe);
    return result;
}

json get_whisper_json() {
    std::string curl_cmd =
        "curl -s -X POST http://localhost:9999/inference "
        // "-F \"file=@audio.wav\" "
        "-F \"file=@samples/jfk.wav\" "
        "-F response_format=json "
        "-H \"Expect:\"";
    std::string response = run_command(curl_cmd);
    return json::parse(response);
}

json get_llama_response(const std::string& prompt) {
    json request = {
        {"model", "llama3"},
        {"prompt", prompt},
        {"stream", false}
    };

    std::string curl_cmd =
        "curl -s http://127.0.0.1:8080/completion "
        "-H \"Content-Type: application/json\" "
        "-d '" + request.dump() + "'";
    std::string response = run_command(curl_cmd);
    return json::parse(response);
}

int main() {
    record_audio_with_ffmpeg();

    json whisper_json = get_whisper_json();
    std::string transcript = whisper_json["text"];
    std::cout << "Transcript: " << transcript << std::endl;

    json llama_json = get_llama_response(transcript);
    std::cout << "LLaMA response:\n" << llama_json["content"] << std::endl;

    return 0;
}  