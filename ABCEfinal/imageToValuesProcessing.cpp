#include <iostream>
#include <fstream>
#include <cstdint>
#include "imageToValuesProcessing.h"

#pragma pack(push, 1)
struct BMPHeader {
    char signature[2];
    uint32_t fileSize;
    uint16_t reserved1;
    uint16_t reserved2;
    uint32_t dataOffset;
};

struct BMPInfoHeader {
    uint32_t headerSize;
    int32_t width;
    int32_t height;
    uint16_t planes;
    uint16_t bitsPerPixel;
    uint32_t compression;
    uint32_t imageSize;
    int32_t xPixelsPerMeter;
    int32_t yPixelsPerMeter;
    uint32_t totalColors;
    uint32_t importantColors;
};
#pragma pack(pop)

void piss() {
    const char* filename = "your_image.bmp";

    std::ifstream file(filename, std::ios::binary);
    if (!file.is_open()) {
        std::cerr << "Error opening file: " << filename << std::endl;
        return;
    }

    BMPHeader header;
    file.read(reinterpret_cast<char*>(&header), sizeof(BMPHeader));

    if (header.signature[0] != 'B' || header.signature[1] != 'M') {
        std::cerr << "Not a valid BMP file" << std::endl;
        return;
    }

    BMPInfoHeader infoHeader;
    file.read(reinterpret_cast<char*>(&infoHeader), sizeof(BMPInfoHeader));

    if (infoHeader.bitsPerPixel != 24) {
        std::cerr << "Only 24-bit BMP images are supported" << std::endl;
        return;
    }

    file.seekg(header.dataOffset, std::ios::beg);

    for (int y = infoHeader.height - 1; y >= 0; --y) {
        for (int x = 0; x < infoHeader.width; ++x) {
            uint8_t blue, green, red;
            file.read(reinterpret_cast<char*>(&blue), sizeof(uint8_t));
            file.read(reinterpret_cast<char*>(&green), sizeof(uint8_t));
            file.read(reinterpret_cast<char*>(&red), sizeof(uint8_t));

            // Now you have the RGB values for each pixel
            // Do something with the values (e.g., print them)
            std::cout << "Pixel at (" << x << ", " << y << "): "
                << "R=" << static_cast<int>(red) << " G=" << static_cast<int>(green) << " B=" << static_cast<int>(blue) << std::endl;
        }

        // Skip padding (if any)
        file.seekg(infoHeader.width % 4, std::ios::cur);
    }

    file.close();

    return;
}