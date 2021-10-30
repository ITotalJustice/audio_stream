#include "wav.hpp"
#define DR_WAV_IMPLEMENTATION
#include "dr_wav.h"

namespace audio::decoder {

Wav::~Wav() {
    if (this->is_init) {
        drwav_uninit(&this->dr);
    }
}

auto Wav::Setup(const std::string& path) -> bool {
    if (!drwav_init_file(&this->dr, path.c_str(), nullptr)) {
        return false;
    }
    this->bytes_per_pcm_frame = drwav_get_bytes_per_pcm_frame(&this->dr);
    return this->is_init = Stream::Setup();
}

auto Wav::Decode(std::size_t sample_count, std::int16_t *data) -> std::size_t {
    std::scoped_lock lock{this->mutex};
    return drwav_read_pcm_frames_s16(&this->dr, sample_count / this->GetChannelCount(), data) * this->GetChannelCount() * sizeof(std::int16_t);
}

auto Wav::Tell() noexcept -> std::pair<std::uint32_t, std::uint32_t> {
    std::scoped_lock lock{this->mutex};
    const std::uint64_t byte_position = this->dr.dataChunkDataSize - this->dr.bytesRemaining;
    return {byte_position / this->bytes_per_pcm_frame, this->dr.totalPCMFrameCount};
}

auto Wav::Seek(std::uint64_t target) -> bool {
    std::scoped_lock lock{this->mutex};
    return drwav_seek_to_pcm_frame(&this->dr, target);
}

auto Wav::GetStreamSampleRate() const noexcept -> int {
    return this->dr.sampleRate;
}

auto Wav::GetStreamChannelCount() const noexcept -> int {
    return this->dr.channels;
}

// constexpr auto test(std::uint8_t* dst, const std::uint8_t* src, int volume, int len) -> void {
//     constexpr int max_audioval = ((1 << (16 - 1)) - 1);
//     constexpr int min_audioval = -(1 << (16 - 1));
//     std::int16_t src1{}, src2{};
//     int dst_sample{};

//     len /= 2;
//     while (len--) {
//         src1 = ((src[1]) << 8 | src[0]);
//         // ADJUST_VOLUME(src1, volume);
//         src2 = ((dst[1]) << 8 | dst[0]);
//         src += 2;
//         dst_sample = src1 + src2;
//         if (dst_sample > max_audioval) {
//             dst_sample = max_audioval;
//         } else if (dst_sample < min_audioval) {
//             dst_sample = min_audioval;
//         }
//         dst[0] = dst_sample & 0xFF;
//         dst_sample >>= 8;
//         dst[1] = dst_sample & 0xFF;
//         dst += 2;
//     }
// }

} // namespace audio
