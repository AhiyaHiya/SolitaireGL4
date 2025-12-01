CPMAddPackage("gh:Dav1dde/glad#v2.0.6")

add_subdirectory("${glad_SOURCE_DIR}/cmake" glad_cmake)
glad_add_library(glad
    REPRODUCIBLE
    EXCLUDE_FROM_ALL
    LOADER
    API gl:core=4.6
    EXTENSIONS
        GL_ARB_bindless_texture
        GL_EXT_texture_compression_s3tc
)
