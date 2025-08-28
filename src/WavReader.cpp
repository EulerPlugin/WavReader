#include "WavReader.hpp"
#include <fstream>
#include <format>
#include <iostream>
#include <ranges>

namespace wav{

void analyzeWav(const std::string& pathToWav) 
{
    std::filesystem::path path{pathToWav};
    WavReader wavReader(path);
    wavReader.read();
    wavReader.printInfo();
    wavReader.printAsAscii();
    wavReader.printAsHex();

    const auto binaryPath = path.parent_path() / (path.stem().string() + "bin");
    wavReader.saveAsBinaryFile(binaryPath);
}

WavReader::WavReader(std::filesystem::path pathToWav)
:pathToWav_(std::move(pathToWav))
{
    
}

void WavReader::read()
{
    std::ifstream file(pathToWav_, std::ios::binary);

    if(!file)
    {
        throw std::runtime_error{"failed to open file"};
    }

    fileSize_ = std::filesystem::file_size(pathToWav_);

    file.read(header_.chunkId.data(), header_.chunkId.size());

    auto readField = [&file](auto& feild){
        file.read(reinterpret_cast<char*>(&feild), sizeof(feild));
    };

    readField(header_.chunkSize); 

    file.read(header_.format.data(), header_.format.size());

    file.read(header_.subChunk1Id.data(), header_.subChunk1Id.size());

    readField(header_.subChunk1Size);

    readField(header_.audioFormat);

    readField(header_.chanelCount);

    readField(header_.sampleRate);

    readField(header_.byteRate);

    readField(header_.blockAlign);

    readField(header_.bitsperSample);

    file.read(dataHeader_.subChunk2Id.data(), dataHeader_.subChunk2Id.size());

    readField(dataHeader_.subChunk2Size);

    frameCount_ = dataHeader_.subChunk2Size / header_.blockAlign;

    firstChannelSamples_.reserve(frameCount_);

    const auto sampleSize = header_.bitsperSample / 8u;
    for(auto i: std::views::iota(0u, frameCount_)){
        uint16_t sample = 0u;
        file.read(reinterpret_cast<char*>(&sample), sampleSize);

        firstChannelSamples_.push_back(convertSample(sample, header_.bitsperSample));

        for(auto channel: std::views::iota(1u, header_.chanelCount))    // Discarding the useless channel data
        {
            file.read(reinterpret_cast<char*>(&sample), sampleSize); 
        }
    }
}

void WavReader::printAsAscii()
{
    printUsing([](char c){
        const auto unsignedC = static_cast<unsigned char>(c);
        if(std::isprint(unsignedC)) {
            std::cout << unsignedC;
        } else {
            std::cout << ".";
        }
    });

    std::cout << std::endl;
}

void WavReader::printAsHex() {
    printUsing([](char c){
        std::cout << std::hex << std::setw(2) << std::setfill('0') << 
        std::uppercase << static_cast<int>(*reinterpret_cast<unsigned char*>(&c)) << " ";
    }
    );
}

void WavReader::printUsing(std::function<void(char)> printingFunction) {

    std::ifstream file(pathToWav_, std::ios::binary);

    if(!file)
    {
        throw std::runtime_error{"failed to open file"};
    }

    char readByte;
    int byteCount = 0;

    while(file.read(&readByte, sizeof(readByte))) {
        printingFunction(readByte);
        constexpr auto BYTE_PER_LINE = 10;
        ++byteCount;
        if (byteCount % BYTE_PER_LINE == 0) {
            std::cout << std:: endl;
        }

    }
}


void WavReader::printInfo()
{
    if(fileSize_ == 0)
    {
        throw std::runtime_error("read() the fiel before printing info");
    }

    auto printLine =[](const auto& label, const auto& value){

        std::cout << std::format("{:>20} : {}", label, value) << std::endl;
    };
   
    auto printArray = [printLine](const auto& label, 
                                  const std::array<char, 4>& array)  {

        printLine(label, std::string_view(array.data(), array.size()));
    };

    printLine("File Size", fileSize_);

    printArray("chunkID", header_.chunkId);

    printLine("chunkSize",  header_.chunkSize);

    printArray("format", header_.format);

    printArray("SubChunk 1 Id", header_.subChunk1Id);

    printLine("SubChunk 1 Size", header_.subChunk1Size);

    printLine("Audio Format", header_.audioFormat);

    printLine("Channel Count", header_.chanelCount);

    printLine("SampleRate", header_.sampleRate);

    printLine("Byte Rate", header_.byteRate);
    
    printLine("Block Align", header_.blockAlign);

    printLine("Bits Per Sample", header_.bitsperSample);
    
    printArray("SubChunk 2 Id", dataHeader_.subChunk2Id);

    printLine("SubChunk 2 Size", dataHeader_.subChunk2Size);

    printLine("Frame Count", frameCount_);
}



// Save wave data as a floating-point pattern
void WavReader::saveAsBinaryFile(const std::filesystem::path& pathToBinary){

    std::filesystem::create_directories(pathToBinary.parent_path());

    std::ofstream file(pathToBinary, 
                       std::ios::binary | std::ios::out | std::ios::trunc);
    
    if(!file){
        throw std::runtime_error("fail to open output file for wiriting");
    }

    file.write(reinterpret_cast<char*>(firstChannelSamples_.data()), 
                                       sizeof(decltype(firstChannelSamples_)::value_type) * firstChannelSamples_.size());

    
}

}

