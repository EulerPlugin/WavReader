#pragma once
#include <string>
#include <filesystem>
#include <array>
#include <vector>
#include <functional>

namespace wav {

namespace{
    float convertSample(uint16_t sample, uint16_t bitsPerSample) {
        if(bitsPerSample == 8u) {
            // sample [1, 255]
            constexpr auto INT_8_MAX = static_cast<float>(std::numeric_limits<int8_t>::max());
            constexpr auto LPCM_VALUE_OF_0_SAMPLE = INT_8_MAX + 1.f;
            
            // signed sample [-127, 127]]
            const auto signedSample = (static_cast<float>(sample) - LPCM_VALUE_OF_0_SAMPLE);

            // [-1, 1]
            return  signedSample / INT_8_MAX;
        }else if (bitsPerSample == 16u) {
            // we assume little-indian byte order on the host machine

            // [-32 767, +32 767 ]
            const auto* reinterpretedSample = reinterpret_cast<int16_t*>(&sample);

            // [-1 , 1]
            return static_cast<float>(*reinterpretedSample) / 
                    std::numeric_limits<int16_t>::max();
        }
        return {};
    }
}
    void analyzeWav(const std::string& pathToWav);


    class WavReader{
        public:
            WavReader(std::filesystem::path pathToWav);

            void read();
            void printInfo();
            void printAsAscii();
            void printAsHex();
            void saveAsBinaryFile(const std::filesystem::path& pathToBinary);
            
        private:

            struct  WavHeader{
                static_assert(sizeof(char) == sizeof(uint8_t));
                /* 'RIFF' in ASCII */
                std::array<char, 4> chunkId;

                uint32_t chunkSize;
                
                /* 'WAVE' in ASCII */
                std::array<char, 4> format;

                std::array<char, 4> subChunk1Id;

                uint32_t subChunk1Size;

                /* 1 for LPCM */
                uint16_t audioFormat;

                /* Channel Count */
                uint16_t chanelCount;

                uint32_t sampleRate;

                uint32_t byteRate;

                uint16_t blockAlign;

                uint16_t bitsperSample;
            };

            struct DataHeader {
                /* "data" in ASCII */
                std::array<char, 4> subChunk2Id;

                uint32_t subChunk2Size;
            };

            void printUsing(std::function<void(char)> printingFunction);

            std::filesystem::path pathToWav_;
            std::uintmax_t fileSize_;
            WavHeader header_;
            DataHeader dataHeader_;
            
            std::size_t frameCount_{};
            std::vector<float> firstChannelSamples_;
            



    };
}