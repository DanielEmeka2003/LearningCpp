#ifndef FILEDISCOVERYINPUT_H
#define FILEDISCOVERYINPUT_H

#pragma once

#include <string>
#include <string_view>
#include <istream>
#include <stdexcept>
#include <map>
#include <cstdint>

#include "stream/stringinputstream.h"
#include "stream/stringoutputstream.h"


namespace filediscovery
{
    class FileDiscoveryInputError final: public std::runtime_error
    {
    public:
        explicit FileDiscoveryInputError(const std::string& what) noexcept: std::runtime_error{what}
        {}

        explicit FileDiscoveryInputError(const char* what) noexcept: std::runtime_error{what}
        {}

        ~FileDiscoveryInputError() = default;
    };



    class FileDiscoveryInput final
    {
    public:

        enum class Indicator: char {option = '%', optionsubset = '-'};

        /*Non modifying options*/
    
        static constexpr std::string_view find_option{"find"};
        static constexpr std::string_view findinpwd_option{"findinpwd"};
        static constexpr std::string_view new_option{"new"};
        static constexpr std::string_view ls_option{"ls"};
        static constexpr std::string_view cd_option{"cd"};
        static constexpr std::string_view remove_option{"remove"};

        /*Location modifying options*/

        static constexpr std::string_view copy_option{"copy"};
        static constexpr std::string_view move_option{"move"};
        static constexpr std::string_view rename_option{"rename"};


        /*Special modifying options*/

        static constexpr std::string_view pwd_option{"pwd"};
        static constexpr std::string_view cls_option{"cls"};
        static constexpr std::string_view clear_option{"clear"};
        static constexpr std::string_view help_option{"help"};


        /*Option subsets*/

        static constexpr std::string_view optionsubset_dir{"dir"};
        static constexpr std::string_view optionsubset_file{"file"};
        #ifdef __linux__
        static constexpr std::string_view optionsubset_dirsym{"dirsym"};
        static constexpr std::string_view optionsubset_filesym{"filesym"};
        #endif
        static constexpr std::string_view optionsubset_sub{"sub"};
        static constexpr std::string_view optionsubset_sub_file{"sub+file"};
        static constexpr std::string_view optionsubset_sub_dir{"sub+dir"};
        static constexpr std::string_view optionsubset_generic{"generic"};
        static constexpr std::string_view optionsubset_plain{"plain"};
        static constexpr std::string_view optionsubset_detail{"detail"};
        static constexpr std::string_view optionsubset_rec_plain{"rec+plain"};
        static constexpr std::string_view optionsubset_rec_detail{"rec+detail"};
        static constexpr std::string_view optionsubset_hor{"hor"};
    private:
        
        //for storing file discovery input
        std::string m_input{};
        //for extracting file discovery input feature by feature
        Streams::StringInputStream<char> m_inputparser{std::istringstream{}};
        std::string m_option{};
        std::string m_optionsubset{};
        std::map<std::int16_t, std::string> m_filesystemobj{};
        //for writing messages to give to the FileDiscoveryInputError exception in the case of an input error
        Streams::StringOutputStream<char> m_exception_message{std::ostringstream{}};
    public:
        FileDiscoveryInput() = default;

        FileDiscoveryInput(const FileDiscoveryInput&) = default;
        FileDiscoveryInput(FileDiscoveryInput&&) = default;

        FileDiscoveryInput& operator=(const FileDiscoveryInput&) = default;
        FileDiscoveryInput& operator=(FileDiscoveryInput&&) = default;
        
        ~FileDiscoveryInput() = default;

        /*Returns a const reference to the string holding input feature [option]*/
        const std::string& getOption() const;
        /*Returns a const reference to the string holding input feature [option_subset]*/
        const std::string& getOptionSubset() const;
        /*Returns a const reference to the string holding input feature [filesystem_object]*/
        const std::map<std::int16_t, std::string>& getFileSystemObject() const;

        bool isOption(); //is overall an option

        bool isNonModOption(); //is non-modifying option
        bool isLocationModOption(); //is location modifying option
        bool isSpecialOption(); //is special option
        
        bool isFindOption();
        bool isFindInPwdOption();
        bool isNewOption();
        bool isLsOption();
        bool isCdOption();
        bool isRemoveOption();

        bool isCopyOption();
        bool isMoveOption();
        bool isRenameOption();

        bool isPwdOption();
        bool isClsOption();
        bool isClearOption();
        bool isHelpOption();

        bool isNonModOptionSubset(); //is overall an option subset
        bool isFindOptionSubset(); //is non-modifying option find subset
        bool isFindInPwdOptionSubset(); //is non-modifying option findinpwd subset
        bool isNewOptionOptionSubset(); //is non-modifying option new subset
        bool isLsOptionOptionSubset(); //is non-modifying option ls subset
    private:
        
        void parse_input();
        
        void parse_for_option();
        void parse_for_optionsubset();
        void parse_for_filesystemobject();
        

        void _nonModOptions(const size_t totalNumberOfStaticTableLines);
        void _locationModOptions(const size_t totalNumberOfStaticTableLines);
        void _specialOptions(const size_t totalNumberOfStaticTableLines);

        void _findOptionSubset(const size_t totalNumberOfStaticTableLines);
        void _findInPwdOptionSubset(const size_t totalNumberOfStaticTableLines);
        void _newOptionSubset(const size_t totalNumberOfStaticTableLines);
        void _lsOptionSubset(const size_t totalNumberOfStaticTableLines);
        

        void _falseNotAnOption_errorhandling();
        void _falseNotANonModOptionSubset_errorhandling();
        void _falseShouldNotHaveOptionSubset_errorhandling();
        void _falseNullFilesystemObjectForNonModAndSpecialOption_errorhandling(){}
        void _falseNotInNonModFilesystemObjectFormat_errorhandling(){}
        void _falseNotInLocationModFilesystemObjectFormat_errorhandling(){}
        void _falseSpecialOptionShouldNotHaveFilesystemObject_errorhandling(){}

        void dynamicTableLine(bool up_down, size_t nStaticChars, size_t nDynamicChars);
        void dynamicTableSpace(size_t nStaticChars, size_t nDynamicChars);
        void dynamicTableMiddleLine(size_t nStaticChars, size_t nDynamicChars);

        friend std::istream& operator>>(std::istream& in, FileDiscoveryInput& input);
    };
}
#endif