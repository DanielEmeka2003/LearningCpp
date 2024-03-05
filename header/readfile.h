#ifndef READFILE_H
#define READFILE_H

#pragma once

#include <filesystem>
#include <stdexcept>

#include "myfunctions1.h"
#include "stream/fileinputstream.h"

namespace Myfcn
{
    class ReadFile
    {
    private:
        std::filesystem::path m_filePath{};
        U8string m_buffer{};

    public:
        ~ReadFile() = default;

        ReadFile(const std::filesystem::path& filePath): m_filePath{filePath}
        {
            if (!std::filesystem::exists(m_filePath))
            throw std::filesystem::filesystem_error{"Invalid", std::error_code{0x2, std::generic_category()}};
            else if (std::filesystem::file_size(m_filePath) == 0)
            throw std::runtime_error{"Null file is not allowed"};
        }
        
        ReadFile& storeFileContents()
        {
            if (m_filePath.empty())
            return *this;

            using namespace Streams;
            FileInputStream fis{std::ifstream{m_filePath}};
            std::string buffer{};

            while (true)
            {
                std::string temp{};
                try
                {
                    fis.read_str(temp, true);
                    temp.push_back('\n');

                    buffer += temp;
                }
                catch(const std::exception&)
                { 
                    buffer += temp; //get the last contents
                    break; 
                }
            }
            m_buffer = convertAsciiStrToU8Str(buffer);
            return *this;
        }

        ReadFile& setFilePath(const std::filesystem::path& filePath)
        {
            m_filePath = filePath;

            if (!std::filesystem::exists(m_filePath))
            throw std::filesystem::filesystem_error{"Invalid", std::error_code{0x2, std::generic_category()}};
            else if (std::filesystem::file_size(m_filePath) == 0)
            throw std::runtime_error{"Null file is not allowed"};
        }

        U8string& retrieveBuffer()
        { return m_buffer; }

        const U8string& retrieveBuffer() const
        { return m_buffer; }

        ReadFile& emptyBuffer()
        { return (m_buffer.clear(), *this); }

        bool isBufferEmpty()
        { return m_buffer.empty(); }
    };

} // namespace MyFcn

#endif