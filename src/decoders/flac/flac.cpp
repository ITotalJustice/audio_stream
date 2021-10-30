#include "flac.hpp"
#define DR_FLAC_IMPLEMENTATION
#include "dr_flac.h"

namespace audio::decoder {

auto Flac::Setup(const std::string& path) -> bool {
    this->dr = UniqueDrFlac{drflac_open_file(path.c_str(), nullptr)};
    if (!this->dr) {
        return false;
    }
    return this->is_init = Stream::Setup();
}

auto Flac::Decode(std::size_t sample_count, std::int16_t *data) -> std::size_t {
    std::scoped_lock lock{this->mutex};
    return drflac_read_pcm_frames_s16(this->dr.get(), sample_count / this->GetChannelCount(), data) * this->GetChannelCount() * sizeof(std::int16_t);
}

auto Flac::Tell() noexcept -> std::pair<std::uint32_t, std::uint32_t> {
    std::scoped_lock lock{this->mutex};
    return {this->dr->currentPCMFrame, this->dr->totalPCMFrameCount};
}

auto Flac::Seek(std::uint64_t target) -> bool {
    std::scoped_lock lock{this->mutex};
    return drflac_seek_to_pcm_frame(this->dr.get(), target);
}

auto Flac::GetStreamSampleRate() const noexcept -> int {
    return this->dr->sampleRate;
}

auto Flac::GetStreamChannelCount() const noexcept -> int {
    return this->dr->channels;
}

} // namespace audio
