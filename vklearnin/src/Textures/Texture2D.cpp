#include "vklearnin/common.hpp"
#include "vklearnin/Textures/Texture2D.hpp"

#include "vklearnin/Buffers/StagingBuffer.hpp"

#include "stb/stb_image.h"

void Texture2D::load_file(const char *filepath) {
    int width;
    int height;
    int channels;

    stbi_uc *buffer = stbi_load(
        filepath,
        &width,
        &height,
        &channels,
        0
    );

    if(buffer == nullptr) {
		CONSOLE_WARN("Failed to load image {}\n\t"
                        "Size/Channels: {}x{}@{}\n\t"
                        "Error: '{}'",
                        filepath, width, height, channels,
						stbi_failure_reason());
    }
}