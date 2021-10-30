#include "mp3.hpp"
#define DR_MP3_IMPLEMENTATION
#include "dr_mp3.h"

namespace audio::decoder {

Mp3::~Mp3() {
    if (this->is_init) {
        drmp3_uninit(&this->dr);
    }
}

auto Mp3::Setup(const std::string& path) -> bool {
    if (!drmp3_init_file(&this->dr, path.c_str(), nullptr)) {
        return false;
    }
    this->pcm_frame_count =  drmp3_get_pcm_frame_count(&this->dr);
    return this->is_init = Stream::Setup();
}

auto Mp3::Decode(std::size_t sample_count, std::int16_t *data) -> std::size_t {
    std::scoped_lock lock{this->mutex};
    return drmp3_read_pcm_frames_s16(&this->dr, sample_count / this->GetChannelCount(), data) * this->GetChannelCount() * sizeof(std::int16_t);
}

auto Mp3::Tell() noexcept -> std::pair<std::uint32_t, std::uint32_t> {
    std::scoped_lock lock{this->mutex};
    return {this->dr.currentPCMFrame, this->pcm_frame_count};
}

auto Mp3::Seek(std::uint64_t target) -> bool {
    std::scoped_lock lock{this->mutex};
    return drmp3_seek_to_pcm_frame(&this->dr, target);
}

auto Mp3::GetStreamSampleRate() const noexcept -> int {
    return this->dr.sampleRate;
}

auto Mp3::GetStreamChannelCount() const noexcept -> int {
    return this->dr.channels;
}

} // namespace audio
