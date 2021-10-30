#include "ogg.hpp"
#undef STB_VORBIS_HEADER_ONLY
#include "stb_vorbis.h"

namespace audio::decoder {

auto Ogg::Setup(const std::string& path) -> bool {
    int error{};
    this->stream = UniqueStbVorbis{stb_vorbis_open_filename(path.c_str(), &error, nullptr)};
    if (!stream || error != VORBIS__no_error) {
        return false;
    }
    this->info = stb_vorbis_get_info(this->stream.get());
    return this->is_init = Stream::Setup();
}

auto Ogg::Decode(std::size_t sample_count, std::int16_t *data) -> std::size_t {
    std::scoped_lock lock{this->mutex};
    return static_cast<std::uint64_t>(stb_vorbis_get_samples_short_interleaved(this->stream.get(), this->info.channels, data, static_cast<int>(sample_count / this->GetChannelCount()))) * this->GetChannelCount() * sizeof(std::int16_t);
}

auto Ogg::Tell() noexcept -> std::pair<std::uint32_t, std::uint32_t> {
    std::scoped_lock lock{this->mutex};
    // todo: normalise
    return {this->stream->channel_buffer_start, this->stream->channel_buffer_end};
}

auto Ogg::Seek(std::uint64_t target) -> bool {
    std::scoped_lock lock{this->mutex};
    stb_vorbis_seek_frame(this->stream.get(), static_cast<unsigned int>(target));
    return true;
}

auto Ogg::GetStreamSampleRate() const noexcept -> int {
    return static_cast<int>(this->info.sample_rate);
}

auto Ogg::GetStreamChannelCount() const noexcept -> int {
    return this->info.channels;
}

} // namespace audio
