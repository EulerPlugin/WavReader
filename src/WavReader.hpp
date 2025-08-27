#pragma once
#include <string>
#include <filesystem>
#include <array>


namespace wav {

    void analyzeWav(const std::string& pathToWav);


    class WavReader{
        public:
            WavReader(std::filesystem::path pathToWav);

            void read();
            void printInfo();
            
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

            std::filesystem::path pathToWav_;
            std::uintmax_t fileSize_;
            WavHeader header_;
            DataHeader dataHeader_;

    };
}