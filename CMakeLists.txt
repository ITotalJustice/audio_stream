cmake_minimum_required(VERSION 3.18.0)


project(stream
    VERSION 0.0.1
    DESCRIPTION ""
    HOMEPAGE_URL ""
    LANGUAGES C CXX
)

# option(FLAC "build with flac support" ON)
# option(MP3 "build with mp3 support" ON)
# option(OGG "build with ogg support" ON)
# option(WAV "build with wav support" ON)

add_subdirectory(src)
add_subdirectory(examples)
