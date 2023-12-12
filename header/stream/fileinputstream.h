#ifndef FILEINPUTSTREAM
#define FILEINPUTSTREAM

#pragma once

#include <fstream>
#include <chrono>
#include <utility>
#include <filesystem>

#include "inputstream.h"

namespace Streams
{
    template <typename CharT>
    class FileInputStream: public InputStream<CharT>
    {
    private:
        //Holds the input stream object.
        std::basic_ifstream<CharT> m_ifstream{};

    public:
        FileInputStream(std::basic_ifstream<CharT>&& ifstream) noexcept;

        //Copying is not allowed.
        FileInputStream(const FileInputStream&) = delete;
        FileInputStream(FileInputStream&&) = delete;

        //Copy assigning is not allowed.
        FileInputStream& operator=(const FileInputStream&) = delete;
        FileInputStream& operator=(FileInputStream&&) = delete;

        ~FileInputStream() noexcept = default;

        /*Returns a reference to the file input stream object given.(Preferred)*/
        constexpr std::basic_ifstream<CharT>& get() noexcept;

        constexpr std::basic_ifstream<CharT>& operator*() noexcept;

        void open(const char* filename, std::ios_base::openmode mode = std::ios_base::in);
        void open(const std::filesystem::path::value_type* filename, std::ios_base::openmode mode = std::ios_base::in);
        void open(const std::string& filename, std::ios_base::openmode mode = std::ios_base::in);
        void open(const std::filesystem::path& filename, std::ios_base::openmode mode = std::ios_base::in);

        bool is_open() const;

        /*Closes the file
        Behaves exactly like this:
        {
            std::ifstream if{"nothing.txt"};
            if.close();
        }*/
        void close();
    };

    template <typename CharT>
    FileInputStream<CharT>::FileInputStream(std::basic_ifstream<CharT>&& ifstream) noexcept
    :InputStream<CharT>{},
    m_ifstream{ std::forward<std::basic_ifstream<CharT>>(ifstream) }
    {
        InputStream<CharT>::set_m_in(&m_ifstream);
    }

    template <typename CharT>
    constexpr std::basic_ifstream<CharT>& FileInputStream<CharT>::get() noexcept
    { return m_ifstream; }

    template <typename CharT>
    constexpr std::basic_ifstream<CharT>& FileInputStream<CharT>::operator*() noexcept
    { return m_ifstream; }

    template <typename CharT>
    void FileInputStream<CharT>::open( const char* filename, std::ios_base::openmode mode)
    { m_ifstream.is_open()? (m_ifstream.close(), m_ifstream.open(filename, mode)) : (m_ifstream.open(filename, mode)); }

    template <typename CharT>
    void FileInputStream<CharT>::open( const std::filesystem::path::value_type* filename, std::ios_base::openmode mode)
    { m_ifstream.is_open()? (m_ifstream.close(), m_ifstream.open(filename, mode)) : (m_ifstream.open(filename, mode)); }

    template <typename CharT>
    void FileInputStream<CharT>::open( const std::string& filename, std::ios_base::openmode mode)
    { m_ifstream.is_open()? (m_ifstream.close(), m_ifstream.open(filename, mode)) : (m_ifstream.open(filename, mode)); }

    template <typename CharT>
    void FileInputStream<CharT>::open(const std::filesystem::path& filename, std::ios_base::openmode mode)
    { m_ifstream.is_open()? (m_ifstream.close(), m_ifstream.open(filename, mode)) : (m_ifstream.open(filename, mode)); }

    template <typename CharT>
    bool FileInputStream<CharT>::is_open() const
    { return m_ifstream.is_open(); }

    template <typename CharT>
    void FileInputStream<CharT>::close()
    { if (m_ifstream.is_open()) m_ifstream.close(); }

} // namespace Streams


#endif