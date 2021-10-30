#pragma once

#include "stream.hpp"
#include "dr_wav.h"

namespace audio::decoder {

class Wav final : public Stream {
public:
    using Stream::Stream;
    ~Wav();

    auto Setup(const std::string& path) -> bool override;
    auto Decode(std::size_t sample_count, std::int16_t *data) -> std::size_t override;
    auto Tell() noexcept -> std::pair<std::uint32_t, std::uint32_t> override;
    auto Seek(std::uint64_t target) -> bool override;
    auto GetStreamSampleRate() const noexcept -> int override;
    auto GetStreamChannelCount() const noexcept -> int override;

private:
    drwav dr{};
    std::uint32_t bytes_per_pcm_frame{};
};

} // namespace audio
