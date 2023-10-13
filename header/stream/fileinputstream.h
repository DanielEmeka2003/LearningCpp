#ifndef FILEINPUTSTREAM
#define FILEINPUTSTREAM

#pragma once

#include <fstream>
#include <chrono>
#include <utility>
#include <filesystem>
#include <stdexcept>

namespace Streams
{
    template <typename CharT>
    class FileInputStream
    {
    private:
        //Holds the input stream object.
        std::basic_ifstream<CharT>&& m_ifstream;

        //A flag used to signal if function read_str has been called or not.
        //Used in conjunction with m_readEndTime data member.
        bool m_is_read_fcn_used{};

        std::chrono::sys_time<std::chrono::nanoseconds> m_readEndTime{};
    public:
        static constexpr const char* e_eofmessage{"[End of File encountered]"};

        using StringType = std::basic_string<CharT>;
        
        constexpr FileInputStream(std::basic_ifstream<CharT>&& ifstream) noexcept;

        //Copying is not allowed.
        FileInputStream(const FileInputStream&) = default;
        FileInputStream(FileInputStream&&) = default;

        //Copy assigning is not allowed.
        FileInputStream& operator=(const FileInputStream&) = delete;
        FileInputStream& operator=(FileInputStream&&) = default;

        constexpr ~FileInputStream() = default;

        /*
        Takes in a reference to a std::string and boolean is_leadingWhiteSpace_valid(set to false by default).
        If is_leadingWhiteSpace_valid is set to true, std::getline(m_in, arg1) is used to read input stream data to the string parameter,
        else std::getline(m_in >> std::ws, arg1) is used.
        Because basic_istream objects or objects inherited from basic_istream ignores leading whitespace by default, 
        leaving parameter is_leadingWhiteSpace_valid set to false is advisable.

        [Note] This function reads data from the input stream and moves the input stream indicator, 
        which means it is dependant on the position of the input stream indicator.

        Since checking for end of file is generally used as a condition to stop reading from a file,
        this function throws exception std::runtime_error when end of file is reached, so you could do something like this:
        {
            FileInputStream file{std::ifstream{"sample.txt"}};

            std::string str{};
            try
            {
                while (true)
                {
                    std::string temp{};
                    file.read_str(temp, true);//extract with leading space
                    str += temp;
                }
            }
            catch([[maybe_unused]] const std::exception& e)
            {}
        }
        
        [Miscellany] This class only supports reading to a std::string, as i strongly discourage reading to any other data type.
        If you have a record of numbers in a file for instance, it would generally be a good idea to read to a std::string first and then parse
        the std::string for your record of number.
        Nevertheless, if reading to other data types is still required, then a simple call to function FileInputStream::get(operator*)
        would suffice.*/
        void read_str(StringType& arg1, bool is_leadingWS_valid = false);

        std::basic_istream<CharT>& 
        moveIndicator
        (std::basic_ifstream<CharT>::off_type number_of_bytes, std::basic_ifstream<CharT>::seekdir offset_direction = std::ios_base::beg);        

        std::basic_ifstream<CharT>::pos_type getIndicatorPos();

        /*Returns a reference to the file input stream object given.(Preferred)*/
        constexpr std::basic_ifstream<CharT>& get() noexcept;

        /*Returns a reference to the file input stream object given.(Not Preferred)
        This method is not preferred because the underlying concept of deferencing a class object whoose data member is not a pointer is flawed.
        It is only defined here out of convenience - meaning it is easier to write this '*object' than use the preferred version 'object.get()'.*/
        constexpr std::basic_ifstream<CharT>& operator*() noexcept;

        /*Returns the time read_str function finished its operation.
        The timezone_name parameter takes in the name of a time zone you wish the time to be in, default is "ETC/UTC".
        If no read function has been called prior to a function call to public member endtime_of_read_fcn,
        the function returns the epoch time of std::chrono::system_clock.*/
        std::chrono::zoned_time<std::chrono::nanoseconds> endtime_of_read_fcn(const char* timezone_name = "Etc/UTC") const noexcept;

        //opens a regular file in a filesystem
        //Throws expection std::filesytem::filesystem_error if the expected file to read from is not open or is a directory.
        void openRegularFile(const std::filesystem::path& file);
    };

    template <typename CharT>
    constexpr FileInputStream<CharT>::FileInputStream(std::basic_ifstream<CharT>&& ifstream) noexcept
    :m_ifstream{ std::forward<std::basic_ifstream<CharT>>(ifstream) }
    {}

    template <typename CharT>
    void FileInputStream<CharT>::read_str(StringType& arg1, bool is_leadingWS_valid)
    {
        if (!m_ifstream.is_open())
        throw std::runtime_error{"No file is open"};

        if (is_leadingWS_valid)
        std::getline(m_ifstream, arg1);
        else //std::ws to ignore leading whitespace.
        std::getline(m_ifstream >> std::ws, arg1);

        if (m_ifstream.eof())
        throw std::runtime_error{e_eofmessage};

        m_readEndTime = std::chrono::system_clock::now();
        m_is_read_fcn_used = true;
    }

    template <typename CharT>
    std::basic_istream<CharT>&
    FileInputStream<CharT>::moveIndicator(std::basic_ifstream<CharT>::off_type number_of_bytes, std::basic_ifstream<CharT>::seekdir offset_direction)
    { return m_ifstream.seekp(number_of_bytes, offset_direction); }   

    template <typename CharT>
    std::basic_ifstream<CharT>::pos_type FileInputStream<CharT>::getIndicatorPos()
    { return m_ifstream.tellp(); }

    template <typename CharT>
    constexpr std::basic_ifstream<CharT>& FileInputStream<CharT>::get() noexcept
    { return m_ifstream; }

    template <typename CharT>
    constexpr std::basic_ifstream<CharT>& FileInputStream<CharT>::operator*() noexcept
    { return m_ifstream; }

    template <typename CharT>
    std::chrono::zoned_time<std::chrono::nanoseconds> 
    FileInputStream<CharT>::endtime_of_read_fcn(const char* timezone_name) const noexcept
    {
        if (!m_is_read_fcn_used)
        return std::chrono::zoned_time<std::chrono::nanoseconds>{timezone_name};

        return std::chrono::zoned_time{timezone_name, m_readEndTime};
    }

    template <typename CharT>
    void FileInputStream<CharT>::openRegularFile(const std::filesystem::path& file)
    {
        if (std::filesystem::exists(file))
        {
            if (std::filesystem::is_directory(file))
            throw std::filesystem::filesystem_error{"File is a Directory", file, {}};
            
            m_ifstream.open(file);
        }
        else
        throw std::filesystem::filesystem_error{"Cannot open regular file", file, {0x2, std::generic_category()}};
    }

} // namespace Streams


#endif