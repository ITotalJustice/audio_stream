#pragma once

#include "resamplers/SDL_audioEX.h"

#include <cstdint>
#include <vector>
#include <string>
#include <mutex>
#include <memory>
#include <utility>

namespace audio {

class Stream {
public:
    enum class [[nodiscard]] State { PLAYING, DONE, ERROR };

public:
    Stream(int sample_rate, int channel_count);
    virtual ~Stream() = default;

    auto IsDone() noexcept -> bool;
    auto Resample(std::uint8_t* out, std::size_t size) -> Stream::State;

    auto GetSampleRate() const noexcept -> int;
    auto GetChannelCount() const noexcept -> int;

    // pure. need to overriden
    // should be [[nodiscard]] but this is not inherited sadly...
    virtual auto Setup(const std::string& path) -> bool = 0;
    virtual auto Decode(std::size_t sample_count, std::int16_t *data) -> std::size_t = 0;
    virtual auto Tell() noexcept -> std::pair<std::uint32_t, std::uint32_t> = 0;
    virtual auto Seek(std::uint64_t target) -> bool = 0;
    virtual auto GetStreamSampleRate() const noexcept -> int = 0;
    virtual auto GetStreamChannelCount() const noexcept -> int = 0;

protected:
    // SOURCE: https://dev.krzaq.cc/post/you-dont-need-a-stateful-deleter-in-your-unique_ptr-usually/
    // used for deconstructor for smart pointers.
    template<auto func>
    struct FunctionCaller {
        template<typename... Us>
        auto operator()(Us&&... us) const {
            return func(std::forward<Us...>(us...));
        }
    };
    template<auto func>
    using Deleter = FunctionCaller<func>;

protected:
    [[nodiscard]]
    auto Setup() -> bool;

protected:
    std::vector<std::int16_t> buffer;
    std::mutex mutex{};
    bool is_init{false};

private:
    using UniqueAudioStream = std::unique_ptr<SDL_AudioStream, Deleter<&SDL_FreeAudioStreamEX>>;
    UniqueAudioStream sdl_stream{nullptr};
    int sample_rate{48000};
    int channel_count{2};
};

} // namespace audio
