#ifndef FILEOUTPUTSTREAM
#define FILEOUTPUTSTREAM

#pragma once

#include <fstream>
#include <chrono>
#include <utility>
#include <type_traits>
#include <filesystem>

#include "outputstream.h"

namespace Streams
{
    template <typename CharT>
    class FileOutputStream: public OutputStream<CharT>
    {
    private:
        //Holds the file output stream object.
        std::basic_ofstream<CharT> m_ofstream;

    public:
        FileOutputStream(std::basic_ofstream<CharT>&& ofstream) noexcept;

        FileOutputStream(const FileOutputStream&) = delete;
        FileOutputStream(FileOutputStream&&) = delete;

        FileOutputStream& operator=(const FileOutputStream&) = delete;
        FileOutputStream& operator=(FileOutputStream&&) = delete;

        ~FileOutputStream() noexcept = default;

        /*Returns a reference to the file output stream object given.(Preferred)*/
        constexpr std::basic_ofstream<CharT>& get() noexcept;

        constexpr std::basic_ofstream<CharT>& operator*() noexcept;

        void open(const std::filesystem::path::value_type* filename, std::ios_base::openmode mode = std::ios_base::out);
        void open(const std::string& filename, std::ios_base::openmode mode = std::ios_base::out);
        void open(const std::filesystem::path& filename, std::ios_base::openmode mode = std::ios_base::out);

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
    FileOutputStream<CharT>::FileOutputStream(std::basic_ofstream<CharT>&& ofstream) noexcept
    :OutputStream<CharT>{}, 
    m_ofstream{ std::forward<std::basic_ofstream<CharT>>(ofstream) }
    {
        OutputStream<CharT>::set_m_out(&m_ofstream);
    }

    template <typename CharT>
    constexpr std::basic_ofstream<CharT>& FileOutputStream<CharT>::get() noexcept
    { return m_ofstream; }

    template <typename CharT>
    constexpr std::basic_ofstream<CharT>& FileOutputStream<CharT>::operator*() noexcept
    { return m_ofstream; }

    template <typename CharT>
    void FileOutputStream<CharT>::open( const std::filesystem::path::value_type* filename, std::ios_base::openmode mode)
    { m_ofstream.is_open()? (m_ofstream.close(), m_ofstream.open(filename, mode)) : (m_ofstream.open(filename, mode)); }

    template <typename CharT>
    void FileOutputStream<CharT>::open( const std::string& filename, std::ios_base::openmode mode)
    { m_ofstream.is_open()? (m_ofstream.close(), m_ofstream.open(filename, mode)) : (m_ofstream.open(filename, mode)); }

    template <typename CharT>
    void FileOutputStream<CharT>::open(const std::filesystem::path& filename, std::ios_base::openmode mode)
    { m_ofstream.is_open()? (m_ofstream.close(), m_ofstream.open(filename, mode)) : (m_ofstream.open(filename, mode)); }

    template <typename CharT>
    bool FileOutputStream<CharT>::is_open() const
    { return m_ofstream.is_open(); }

    template <typename CharT>
    void FileOutputStream<CharT>::close()
    { if (m_ofstream.is_open()) m_ofstream.close(); }
} // namespace Streams


#endif