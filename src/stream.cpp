#include "stream.hpp"

namespace audio {

Stream::Stream(int _sample_rate, int _channel_count) {
    this->sample_rate = _sample_rate;
    this->channel_count = _channel_count;
}

auto Stream::IsDone() noexcept -> bool {
    const auto [current, total] = this->Tell();
    return current >= total;
}

auto Stream::Resample(std::uint8_t* out, std::size_t size) -> State {
    if (!out || !size) { return State::ERROR; }
    while (size > 0) {
        if (auto sz = SDL_AudioStreamGetEX(this->sdl_stream.get(), out, size); sz != 0) {
            size -= sz;
            out += sz;
        } else {
            const auto dec_got = this->Decode(this->buffer.size(), this->buffer.data());
            if (dec_got == 0) {
                return State::DONE;
            }
            if (0 != SDL_AudioStreamPutEX(this->sdl_stream.get(), this->buffer.data(), dec_got)) {
                return State::ERROR;
            }
        }
    }
    return State::PLAYING;
}

auto Stream::GetSampleRate() const noexcept -> int {
    return this->sample_rate;
}

auto Stream::GetChannelCount() const noexcept -> int {
    return this->channel_count;
}

auto Stream::Setup() -> bool {
    this->buffer.resize(this->GetSampleRate() * this->GetChannelCount());

    this->sdl_stream = UniqueAudioStream{
        SDL_NewAudioStreamEX(
        AUDIO_S16, this->GetStreamChannelCount(), this->GetStreamSampleRate(),
        AUDIO_S16, this->GetChannelCount(), this->GetSampleRate())
    };

    if (!this->sdl_stream) {
        return false;
    }
    return true;
}

} // namespace audio
