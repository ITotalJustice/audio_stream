#include <cstdint>
#include <cstdio>
#include <cstring>
#include <array>
#include <string_view>
#include <optional>
#include <SDL.h>

#include <decoders/flac/flac.hpp>
#include <decoders/mp3/mp3.hpp>
#include <decoders/ogg/ogg.hpp>
#include <decoders/wav/wav.hpp>

namespace {

constexpr auto FREQ = 48000;
constexpr auto CHANNELS = 2;
constexpr auto SAMPLES = 4096;

enum class FileType {
    FLAC,
    MP3,
    WAV,
    OGG,
};

constexpr std::array VALID_MUSIC_EXTENSIONS{
    std::pair{std::string_view{".flac"}, FileType::FLAC},
    std::pair{std::string_view{".mp3"}, FileType::MP3},
    std::pair{std::string_view{".mpg"}, FileType::MP3},
    std::pair{std::string_view{".mpeg"}, FileType::MP3},
    std::pair{std::string_view{".mad"}, FileType::MP3},
    std::pair{std::string_view{".wav"}, FileType::WAV},
    std::pair{std::string_view{".ogg"}, FileType::OGG},
};

constexpr auto get_file_type(const std::string_view name) noexcept -> std::optional<FileType> {
    for (const auto& [ext, type] : VALID_MUSIC_EXTENSIONS) {
        if (name.ends_with(ext)) {
            return type;
        }
    }
    return {};
}

auto open_stream(const std::string_view name) -> std::unique_ptr<audio::Stream> {
    if (auto type = get_file_type(name); type.has_value()) {
        switch (type.value()) {
        case FileType::FLAC: return std::make_unique<audio::decoder::Flac>(FREQ, CHANNELS);
        case FileType::MP3: return std::make_unique<audio::decoder::Mp3>(FREQ, CHANNELS);
        case FileType::WAV: return std::make_unique<audio::decoder::Wav>(FREQ, CHANNELS);
        case FileType::OGG: return std::make_unique<audio::decoder::Ogg>(FREQ, CHANNELS);
        }
    }

    return nullptr;
}

auto mixer(void* user, std::uint8_t* data, int len) -> void {
    if (len <= 0) { return; }
    std::memset(data, 0, len); // all audio has to be set in sdl2
    auto stream = static_cast<audio::Stream*>(user);
    stream->Resample(data, len);
}

} // namespace

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::printf("missing args\n");
        return -1;
    }

    // open music stream
    auto stream = open_stream(argv[1]);
    if (!stream) {
        std::printf("failed to open file %s\n", argv[1]);
        return -1;
    }

    if (!stream->Setup(argv[1])) {
        std::printf("failed to setup stream %s\n", argv[1]);
        return -1;
    }

    // init sdl and audio
    SDL_Init(SDL_INIT_AUDIO | SDL_INIT_EVENTS);
    SDL_AudioSpec wanted_spec{};
    SDL_AudioSpec got_spec{};
    wanted_spec.freq = FREQ;
    wanted_spec.format = AUDIO_S16;
    wanted_spec.channels = CHANNELS;
    wanted_spec.samples = SAMPLES;
    wanted_spec.callback = mixer;
    wanted_spec.userdata = stream.get();
    SDL_AudioDeviceID audio_device = SDL_OpenAudioDevice(nullptr, 0, &wanted_spec, &got_spec, 0);
    SDL_PauseAudioDevice(audio_device, 0);

    bool quit = false;
    while (!quit) {
        if (stream->IsDone()) {
            break;
        }

        SDL_Event e;
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) {
                quit = true;
            }
        }

        SDL_Delay(1000/60);
    }

    SDL_CloseAudioDevice(audio_device);
    SDL_Quit();

    return 0;
}
