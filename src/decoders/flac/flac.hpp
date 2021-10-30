#pragma once

#include <stream.hpp>
#include "dr_flac.h"

#include <memory>

namespace audio::decoder {

class Flac final : public Stream {
public:
    using Stream::Stream;

    auto Setup(const std::string& path) -> bool override;
    auto Decode(std::size_t sample_count, std::int16_t *data) -> std::size_t override;
    auto Tell() noexcept -> std::pair<std::uint32_t, std::uint32_t> override;
    auto Seek(std::uint64_t target) -> bool override;
    auto GetStreamSampleRate() const noexcept -> int override;
    auto GetStreamChannelCount() const noexcept -> int override;

private:
    using UniqueDrFlac = std::unique_ptr<drflac, Deleter<&drflac_close>>;
    UniqueDrFlac dr{nullptr};
};

} // namespace audio
