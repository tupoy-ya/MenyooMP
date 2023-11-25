include(FetchContent)

set(DIRENT_BUILD_TESTS OFF CACHE INTERNAL "")

FetchContent_Declare(
    shv
    GIT_REPOSITORY https://github.com/pongo1231/shv_mingw.git
    GIT_TAG        f3ba0f21a46217ab3dad2e8fae8edb9db58e67b0
    GIT_PROGRESS TRUE
) 
message("shv")
FetchContent_MakeAvailable(shv)
