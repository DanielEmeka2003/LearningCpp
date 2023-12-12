#ifndef READFILE_H
#define READFILE_H

#pragma once

#include <filesystem>

namespace Myfcn
{
    class ReadFile
    {
    private:
        std::filesystem::path m_filePath{};
        std::string m_buffer{};
    public:
        ReadFile(const std::filesystem::path& filePath);
        ~ReadFile() = default;
        
        ReadFile& storeFileContents();
        std::string& retrieveBuffer();
        const std::string& retrieveBuffer() const;
        ReadFile& emptyBuffer();
        bool isBufferEmpty();
    };

} // namespace MyFcn










#endif