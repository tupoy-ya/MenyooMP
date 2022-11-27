include(FetchContent)

FetchContent_Declare(
    shv
    GIT_REPOSITORY https://github.com/pongo1231/shv_mingw.git
    GIT_TAG        f3ba0f21a46217ab3dad2e8fae8edb9db58e67b0
    GIT_PROGRESS TRUE
    CONFIGURE_COMMAND ""
    BUILD_COMMAND ""
)
message("shv")
FetchContent_MakeAvailable(shv)