#pragma once

#include "stream.hpp"
#define STB_VORBIS_HEADER_ONLY
#include "stb_vorbis.h"

#include <memory>

namespace audio::decoder {

class Ogg final : public Stream {
public:
    using Stream::Stream;

    auto Setup(const std::string& path) -> bool override;
    auto Decode(std::size_t sample_count, std::int16_t *data) -> std::size_t override;
    auto Tell() noexcept -> std::pair<std::uint32_t, std::uint32_t> override;
    auto Seek(std::uint64_t target) -> bool override;
    auto GetStreamSampleRate() const noexcept -> int override;
    auto GetStreamChannelCount() const noexcept -> int override;

private:
    using UniqueStbVorbis = std::unique_ptr<stb_vorbis, Deleter<&stb_vorbis_close>>;
    UniqueStbVorbis stream{nullptr};
    stb_vorbis_info info{};
};

} // namespace audio
