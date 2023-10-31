#include "readfile.h"
#include "stream/fileinputstream.h"
#include "stdexcept"

namespace Myfcn
{
        ReadFile::ReadFile(const std::filesystem::path& filePath): m_filePath{filePath}
    {
        if (!std::filesystem::exists(m_filePath))
        throw std::filesystem::filesystem_error{"Invalid", std::error_code{0x2, std::generic_category()}};
        else if (std::filesystem::file_size(m_filePath) == 0)
        throw std::runtime_error{"A Null file is not allowed"};
    }

    ReadFile& ReadFile::storeFileContents()
    {
        using namespace Streams;
        FileInputStream fis{std::ifstream{m_filePath}};

        while (true)
        {
            std::string temp{};
            try
            {
                fis.read_str(temp, true);
                temp.push_back('\n');

                m_buffer += temp;
            }
            catch(const std::exception&)
            { 
                m_buffer += temp; //get the last contents
                break; 
            }
        }
        return *this;
    }

    const std::string& ReadFile::retrieveBuffer() const
    { return m_buffer; }

    ReadFile& ReadFile::emptyBuffer()
    { return (m_buffer.clear(), *this); }

} // namespace MyFcn
