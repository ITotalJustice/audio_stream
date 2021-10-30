#pragma once

#include "stream.hpp"
#include "dr_mp3.h"

namespace audio::decoder {

class Mp3 final : public Stream {
public:
    using Stream::Stream;
    ~Mp3();

    auto Setup(const std::string& path) -> bool override;
    auto Decode(std::size_t sample_count, std::int16_t *data) -> std::size_t override;
    auto Tell() noexcept -> std::pair<std::uint32_t, std::uint32_t> override;
    auto Seek(std::uint64_t target) -> bool override;
    auto GetStreamSampleRate() const noexcept -> int override;
    auto GetStreamChannelCount() const noexcept -> int override;

private:
    drmp3 dr{};
    std::uint64_t pcm_frame_count{};
};

} // namespace audio
