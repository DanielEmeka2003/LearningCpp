#include <utility>
#include <iomanip>
#include <cstdint>
#include <algorithm>
#include <regex>

#include "filediscoveryinput.h"
#include "stream/output.h"

namespace filediscovery
{
    const std::string& FileDiscoveryInput::getOption() const
    { return m_option; }

    const std::string& FileDiscoveryInput::getOptionSubset() const
    { return m_optionsubset; }

    const std::map<std::int16_t, std::string>& FileDiscoveryInput::getFileSystemObject() const
    { return m_filesystemobj; }

    bool FileDiscoveryInput::isOption()
    {
        return (isNonModOption() or isLocationModOption() or isSpecialOption());
    }

    bool FileDiscoveryInput::isNonModOption()
    {
        return (isFindOption() or isFindInPwdOption() or isNewOption() or isLsOption() or isCdOption() or isRemoveOption());
    }
    bool FileDiscoveryInput::isLocationModOption()
    {
        return (isCopyOption() or isMoveOption() or isRenameOption());
    }
    bool FileDiscoveryInput::isSpecialOption()
    {
        return (isPwdOption() or isClsOption() or isClearOption() or isHelpOption());
    }


    bool FileDiscoveryInput::isFindOption()
    {
        std::regex reg{static_cast<std::string>(find_option), std::regex_constants::icase};
        return std::regex_match(m_option, reg);
    }
    bool FileDiscoveryInput::isFindInPwdOption()
    {
        std::regex reg{static_cast<std::string>(findinpwd_option), std::regex_constants::icase};
        return std::regex_match(m_option, reg);
    }
    bool FileDiscoveryInput::isNewOption()
    {
        std::regex reg{static_cast<std::string>(new_option), std::regex_constants::icase};
        return std::regex_match(m_option, reg);
    }
    bool FileDiscoveryInput::isLsOption()
    {
        std::regex reg{static_cast<std::string>(ls_option), std::regex_constants::icase};
        return std::regex_match(m_option, reg);
    }
    bool FileDiscoveryInput::isCdOption()
    {
        std::regex reg{static_cast<std::string>(cd_option), std::regex_constants::icase};
        return std::regex_match(m_option, reg);
    }
    bool FileDiscoveryInput::isRemoveOption()
    {
        std::regex reg{static_cast<std::string>(remove_option), std::regex_constants::icase};
        return std::regex_match(m_option, reg);
    }

    bool FileDiscoveryInput::isCopyOption()
    {
        std::regex reg{static_cast<std::string>(copy_option), std::regex_constants::icase};
        return std::regex_match(m_option, reg);
    }
    bool FileDiscoveryInput::isMoveOption()
    {
        std::regex reg{static_cast<std::string>(move_option), std::regex_constants::icase};
        return std::regex_match(m_option, reg);
    }
    bool FileDiscoveryInput::isRenameOption()
    {
        std::regex reg{static_cast<std::string>(rename_option), std::regex_constants::icase};
        return std::regex_match(m_option, reg);
    }

    bool FileDiscoveryInput::isPwdOption()
    {
        std::regex reg{static_cast<std::string>(pwd_option), std::regex_constants::icase};
        return std::regex_match(m_option, reg);
    }
    bool FileDiscoveryInput::isClsOption()
    {
        std::regex reg{static_cast<std::string>(cls_option), std::regex_constants::icase};
        return std::regex_match(m_option, reg);
    }
    bool FileDiscoveryInput::isClearOption()
    {
        std::regex reg{static_cast<std::string>(clear_option), std::regex_constants::icase};
        return std::regex_match(m_option, reg);
    }
    bool FileDiscoveryInput::isHelpOption()
    {
        std::regex reg{static_cast<std::string>(help_option), std::regex_constants::icase};
        return std::regex_match(m_option, reg);
    }


    bool FileDiscoveryInput::isNonModOptionSubset()
    {
        return (isFindOptionSubset() or isFindInPwdOptionSubset() or isNewOptionOptionSubset() or isLsOptionOptionSubset());
    }
    bool FileDiscoveryInput::isFindOptionSubset()
    {
        std::regex reg{"generic|dir|file|sub|sub\\+dir|sub\\+file", std::regex_constants::icase};
        return std::regex_match(m_optionsubset, reg);
    }
    bool FileDiscoveryInput::isFindInPwdOptionSubset()
    {
        return isFindOptionSubset();
    }
    bool FileDiscoveryInput::isNewOptionOptionSubset()
    {
        #ifdef __linux__
        std::regex reg{"dir|file|dirsym|filesym", std::regex_constants::icase};
        #else
        std::regex reg{"dir|file", std::regex_constants::icase};
        #endif

        return std::regex_match(m_optionsubset, reg);
    }
    bool FileDiscoveryInput::isLsOptionOptionSubset()
    {
        std::regex reg{"plain|detail|rec\\+plain|rec\\+detail|file|dir|hor", std::regex_constants::icase};
        return std::regex_match(m_optionsubset, reg);
    }




    /*------------------------------------------------------Private-----------------------------------------------------------*/

    void FileDiscoveryInput::parse_input()
    {
        m_inputparser.insert_str(m_input);

        //if the input starts with the option_indicator, do this
        if ( m_input.starts_with(std::to_underlying(Indicator::option)) )
        parse_for_option();
        else
        m_option = find_option;
        
        parse_for_optionsubset();
        parse_for_filesystemobject();
    }

    void FileDiscoveryInput::parse_for_option()
    {   
        //extract the option to m_option
        try
        {
            m_inputparser.read(m_option);
        }
        catch(const std::exception& e)
        {}
        
        //remove the the option indicator character
        m_option.erase(m_option.begin());

        //if the option the user entered is not valid, throw an exception
        if (!isOption())
        _falseNotAnOption_errorhandling();
        
    }

    void FileDiscoveryInput::parse_for_optionsubset()
    {
        /*try to extract a value from the input stream, if it fails, m_option_subset is left with a null string
        and fails the condition for it being a option subset.*/
        try
        {
            m_inputparser.read(m_optionsubset);
        }
        catch(const std::exception& e)
        {}

        /*If nothing was read to m_optionsubset, just return the control flow back to it's parent function and let
        parse_for_filesystemobject handle it*/
        if (m_optionsubset == "")
        { return; }
        
        //first check if the string begins the option_subset_indicator
        if ( isNonModOption() and m_optionsubset.starts_with(std::to_underlying(Indicator::optionsubset)) )
        {
            //remove the optionsubset indicator
            m_optionsubset.erase(m_optionsubset.begin());

            if (isFindOption() and !isFindOptionSubset())
            {
                _falseNotANonModOptionSubset_errorhandling();
            }
            else if (isFindInPwdOption() and !isFindInPwdOptionSubset())
            {
                _falseNotANonModOptionSubset_errorhandling();
            }
            else if (isNewOption() and !isNewOptionOptionSubset())
            {
                _falseNotANonModOptionSubset_errorhandling();
            }
            else if (isLsOption() and !isLsOptionOptionSubset())
            {
                _falseNotANonModOptionSubset_errorhandling();
            }
            else if (isCdOption() or isRemoveOption())
            {
                _falseNotANonModOptionSubset_errorhandling();
            }
        }
        else if ( (isLocationModOption() or isSpecialOption()) and m_optionsubset.starts_with(std::to_underlying(Indicator::optionsubset)) )
        {
            //remove the optionsubset indicator
            m_optionsubset.erase(m_optionsubset.begin());
            _falseShouldNotHaveOptionSubset_errorhandling();
        }
        else 
        {
            /*if it does not start with the option subset indicator, it must be a filesystem object name, so give it to
            m_filesystemobj and clear the m[option_subset] string*/
            m_filesystemobj[1] = m_optionsubset;

            if (m_option == find_option or m_option == findinpwd_option)
            m_optionsubset = optionsubset_generic;
            else if (m_option == new_option)
            m_optionsubset = optionsubset_file;
            else if (m_option == ls_option)
            m_optionsubset = optionsubset_plain;
            else
            m_optionsubset = {};
        }
    }

    void FileDiscoveryInput::parse_for_filesystemobject()
    {        
        // [[maybe_unused]] auto checkFileSysName = [&filesys = m_filesystem_obj1, &except = m_exception_message]
        // {
        //     auto result = std::ranges::remove(filesys, '\"');
        //     filesys.assign(filesys.begin(), result.begin());
            
        //     if ( filesys == "" or std::ranges::all_of(filesys, [](char x){ return std::isspace(x)? true : false; }))
        //     {
        //         except.empty_stream();//empty the string output stream before writting

        //         except.write_endl("┌────────────────────────────────────────────────────────────────────────┐");
        //         except.write_endl("│ Filesystem object name section of input cannot be null or space filled │");
        //         except.write_endl("└────────────────────────────────────────────────────────────────────────┘");

        //         throw FileDiscoveryInputError{except.get_streamdata()};
        //     }
        // };

        // if ( m_filesystemobj == "" )
        // {
            
        //     try
        //     {
        //         m_inputparser.read_str(m_filesystem_obj1);
        //     }
        //     catch([[maybe_unused]] const std::exception& e)
        //     {}
        // }
        // else
        // {
        //     /*Since read_str throws when eof is set, we define the temporary variable before the try block and use it to append
        //     any remaining inputs in the stream to m_filesystem_obj1*/
        //     std::string temp{};
        //     try
        //     {
        //         m_inputparser.read_str(temp, true);
        //     }
        //     catch([[maybe_unused]] const std::exception& e)
        //     {
        //         m_filesystem_obj1 += temp;
        //     }
        // }

        // /*Check if the filesystem name is null*/
        // checkFileSysName();

        
        // if 
        // ( 
        //     m_filesystem_obj1.starts_with(std::to_underlying(option_indicator)) or
        //     m_filesystem_obj1.starts_with(std::to_underlying(option_subset_indicator))
        // )
        // {
        //     m_exception_message.empty_stream();//empty the string output stream before writting

        //     m_exception_message.write_endl("┌───────────────────────────────────────────────────────────────────────────────┐");
        //     m_exception_message.write_endl("│ Filesystem object name should not begin with special character[ % ] or [ / ]  │");
        //     m_exception_message.write_endl("│ It should be surrounded with quotes(either single or double) instead          │");
        //     m_exception_message.write_endl("└───────────────────────────────────────────────────────────────────────────────┘");
        //     throw FileDiscoveryInputError{m_exception_message.get_streamdata()}; 
        // }
    }


    void FileDiscoveryInput::_nonModOptions(const size_t totalNumberOfStaticTableLines)
    {
        m_exception_message.writews("│ (1)", find_option, '-', "for finding filesystem objects[DEFAULT]");
        {
            constexpr size_t nStaticChars{ 49 + find_option.size() };
            const size_t nStaticSpacing{ totalNumberOfStaticTableLines - nStaticChars };
            dynamicTableSpace(nStaticSpacing, m_option.size());
        }
        m_exception_message.writews("│ (2)", findinpwd_option, '-', "for finding filesystem objects in the present working directory");
        {
            constexpr size_t nStaticChars{ 73 + findinpwd_option.size() };
            const size_t nStaticSpacing{ totalNumberOfStaticTableLines - nStaticChars };
            dynamicTableSpace(nStaticSpacing, m_option.size());
        }
        m_exception_message.writews("│ (3)", new_option, '-', "for creating a new filesystem object");
        {
            constexpr size_t nStaticChars{ 46 + new_option.size() };
            const size_t nStaticSpacing{ totalNumberOfStaticTableLines - nStaticChars };
            dynamicTableSpace(nStaticSpacing, m_option.size());
        }
        m_exception_message.writews("│ (4)", ls_option, '-', "for listing the contents in a filesystem object that is a directory");
        {
            constexpr size_t nStaticChars{ 77 + ls_option.size() };
            const size_t nStaticSpacing{ totalNumberOfStaticTableLines - nStaticChars };
            dynamicTableSpace(nStaticSpacing, m_option.size());
        }
        m_exception_message.writews("│ (5)", cd_option, '-', "for changing the present working directory");
        {
            constexpr size_t nStaticChars{ 52 + cd_option.size() };
            const size_t nStaticSpacing{ totalNumberOfStaticTableLines - nStaticChars };
            dynamicTableSpace(nStaticSpacing, m_option.size());
        }
        m_exception_message.writews("│ (6)", remove_option, '-', "for removing filesystem objects");
        {
            constexpr size_t nStaticChars{ 41 + remove_option.size() };
            const size_t nStaticSpacing{ totalNumberOfStaticTableLines - nStaticChars };
            dynamicTableSpace(nStaticSpacing, m_option.size());
        }
    }

    void FileDiscoveryInput::_locationModOptions(const size_t totalNumberOfStaticTableLines)
    {
        m_exception_message.writews("│ (1)", copy_option, '-', "for copying filesystem objects to another location");
        {
            constexpr size_t nStaticChars{ 60 + copy_option.size() };
            const size_t nStaticSpacing{ totalNumberOfStaticTableLines - nStaticChars };
            dynamicTableSpace(nStaticSpacing, m_option.size());
        }
        m_exception_message.writews("│ (2)", move_option, '-', "for moving filesystem objects to another location");
        {
            constexpr size_t nStaticChars{ 59 + move_option.size() };
            const size_t nStaticSpacing{ totalNumberOfStaticTableLines - nStaticChars };
            dynamicTableSpace(nStaticSpacing, m_option.size());
        }
        m_exception_message.writews("│ (3)", rename_option, '-', "for renaming filesystem objects");
        {
            constexpr size_t nStaticChars{ 41 + rename_option.size() };
            const size_t nStaticSpacing{ totalNumberOfStaticTableLines - nStaticChars };
            dynamicTableSpace(nStaticSpacing, m_option.size());
        }
    }

    void FileDiscoveryInput::_specialOptions(const size_t totalNumberOfStaticTableLines)
    {
        m_exception_message.writews("│ (1)", pwd_option, '-', "for displaying the present working directory");
        {
            constexpr size_t nStaticChars{ 54 + pwd_option.size() };
            const size_t nStaticSpacing{ totalNumberOfStaticTableLines - nStaticChars };
            dynamicTableSpace(nStaticSpacing, m_option.size());
        }
        m_exception_message.writews("│ (2)", cls_option, '-', "for clearing the screen[Implementation Defined]");
        {
            constexpr size_t nStaticChars{ 57 + cls_option.size() };
            const size_t nStaticSpacing{ totalNumberOfStaticTableLines - nStaticChars };
            dynamicTableSpace(nStaticSpacing, m_option.size());
        }
        m_exception_message.writews("│ (3)", clear_option, '-', "for clearing the screen[Implementation Defined - same as cls]");
        {
            constexpr size_t nStaticChars{ 71 + clear_option.size() };
            const size_t nStaticSpacing{ totalNumberOfStaticTableLines - nStaticChars };
            dynamicTableSpace(nStaticSpacing, m_option.size());
        }
        m_exception_message.writews("│ (4)", help_option, '-', "for dispalying the help info");
        {
            constexpr size_t nStaticChars{ 38 + help_option.size() };
            const size_t nStaticSpacing{ totalNumberOfStaticTableLines - nStaticChars };
            dynamicTableSpace(nStaticSpacing, m_option.size());
        }
    }



    void FileDiscoveryInput::_findOptionSubset(const size_t totalNumberOfStaticTableLines)
    {
        m_exception_message.writews("│ (1)", optionsubset_generic, '-', "for finding filesystem objects indiscrimnately[DEFAULT]");
        {
            constexpr size_t nStaticChars{ 65 + optionsubset_generic.size() };
            const size_t nStaticSpacing{ totalNumberOfStaticTableLines - nStaticChars };
            dynamicTableSpace(nStaticSpacing, m_optionsubset.size());
        }
        m_exception_message.writews("│ (2)", optionsubset_file, '-', "for finding filesystem objects that are regular files");
        {
            constexpr size_t nStaticChars{ 63 + optionsubset_file.size() };
            const size_t nStaticSpacing{ totalNumberOfStaticTableLines - nStaticChars };
            dynamicTableSpace(nStaticSpacing, m_optionsubset.size());
        }
        m_exception_message.writews("│ (3)", optionsubset_dir, '-', "for finding filesystem objects that are directories");
        {
            constexpr size_t nStaticChars{ 61 + optionsubset_dir.size() };
            const size_t nStaticSpacing{ totalNumberOfStaticTableLines - nStaticChars };
            dynamicTableSpace(nStaticSpacing, m_optionsubset.size());
        }
        m_exception_message.writews("│ (4)", optionsubset_sub, '-', "for finding filesystem objects that are submatches of the input");
        {
            constexpr size_t nStaticChars{ 73 + optionsubset_sub.size() };
            const size_t nStaticSpacing{ totalNumberOfStaticTableLines - nStaticChars };
            dynamicTableSpace(nStaticSpacing, m_optionsubset.size());
        }
        m_exception_message.writews("│ (5)", optionsubset_sub_file, '-', "for finding filesystem objects that are regular file submatches of the input");
        {
            constexpr size_t nStaticChars{ 86 + optionsubset_sub_file.size() };
            const size_t nStaticSpacing{ totalNumberOfStaticTableLines - nStaticChars };
            dynamicTableSpace(nStaticSpacing, m_optionsubset.size());
        }
        m_exception_message.writews("│ (6)", optionsubset_sub_dir, '-', "for finding filesystem objects that are directory submatches of the input");
        {
            constexpr size_t nStaticChars{ 83 + optionsubset_sub_dir.size() };
            const size_t nStaticSpacing{ totalNumberOfStaticTableLines - nStaticChars };
            dynamicTableSpace(nStaticSpacing, m_optionsubset.size());
        }
    }

    void FileDiscoveryInput::_findInPwdOptionSubset(const size_t totalNumberOfStaticTableLines)
    {
        _findOptionSubset(totalNumberOfStaticTableLines);
    }

    void FileDiscoveryInput::_newOptionSubset([[maybe_unused]] const size_t totalNumberOfStaticTableLines)
    {
        m_exception_message.writews("│ (1)", optionsubset_file, '-', "for creating filesystem objects that are regular files[DEFAULT]");
        {
            constexpr size_t nStaticChars{ 64 + optionsubset_sub_dir.size() };
            const size_t nStaticSpacing{ totalNumberOfStaticTableLines - nStaticChars };
            dynamicTableSpace(nStaticSpacing, m_optionsubset.size());
        }
        m_exception_message.writews("│ (2)", optionsubset_dir, '-', "for creating filesystem objects that are directories");
        {
            constexpr size_t nStaticChars{ 62 + optionsubset_sub_dir.size() };
            const size_t nStaticSpacing{ totalNumberOfStaticTableLines - nStaticChars };
            dynamicTableSpace(nStaticSpacing, m_optionsubset.size());
        }
        #ifdef __linux__
        m_exception_message.writews("│ (3)", optionsubset_filesym, '-', "for creating filesystem objects that are symbolic links to a regular file");
        {
            constexpr size_t nStaticChars{ 83 + optionsubset_filesym.size() };
            const size_t nStaticSpacing{ totalNumberOfStaticTableLines - nStaticChars };
            dynamicTableSpace(nStaticSpacing, m_optionsubset.size());
        }
        m_exception_message.writews("│ (4)", optionsubset_dirsym, '-', "for creating filesystem objects that are symbolic links to a directory");
        {
            constexpr size_t nStaticChars{ 83 + optionsubset_dirsym.size() };
            const size_t nStaticSpacing{ totalNumberOfStaticTableLines - nStaticChars };
            dynamicTableSpace(nStaticSpacing, m_optionsubset.size());
        }
        #endif
    }

    void FileDiscoveryInput::_lsOptionSubset([[maybe_unused]] const size_t totalNumberOfStaticTableLines)
    {
        m_exception_message.writews("│ (1)", optionsubset_plain, '-', "for a plain listing of the contents in a filesystem object that is a directory[DEFAULT]");
        {
            constexpr size_t nStaticChars{ 97 + optionsubset_plain.size() };
            const size_t nStaticSpacing{ totalNumberOfStaticTableLines - nStaticChars };
            dynamicTableSpace(nStaticSpacing, m_optionsubset.size());
        }
        m_exception_message.writews("│ (2)", optionsubset_detail, '-', "for a detailed listing of the contents in a filesystem object that is a directory");
        {
            constexpr size_t nStaticChars{ 91 + optionsubset_detail.size() };
            const size_t nStaticSpacing{ totalNumberOfStaticTableLines - nStaticChars };
            dynamicTableSpace(nStaticSpacing, m_optionsubset.size());
        }
        m_exception_message.writews("│ (3)", optionsubset_rec_plain, '-', "for a recursive plain listing of the contents in a filesystem object that is a directory");
        {
            constexpr size_t nStaticChars{ 98 + optionsubset_rec_plain.size() };
            const size_t nStaticSpacing{ totalNumberOfStaticTableLines - nStaticChars };
            dynamicTableSpace(nStaticSpacing, m_optionsubset.size());
        }
        m_exception_message.writews("│ (4)", optionsubset_rec_detail, '-', "for a recursive detailed listing of the contents in a filesystem object that is a directory");
        {
            constexpr size_t nStaticChars{ 101 + optionsubset_rec_detail.size() };
            const size_t nStaticSpacing{ totalNumberOfStaticTableLines - nStaticChars };
            dynamicTableSpace(nStaticSpacing, m_optionsubset.size());
        }
        m_exception_message.writews("│ (5)", optionsubset_file, '-', "for a listing of all regular files in a filesystem object that is a directory");
        {
            constexpr size_t nStaticChars{ 87 + optionsubset_file.size() };
            const size_t nStaticSpacing{ totalNumberOfStaticTableLines - nStaticChars };
            dynamicTableSpace(nStaticSpacing, m_optionsubset.size());
        }
        m_exception_message.writews("│ (6)", optionsubset_dir, '-', "for a listing of all subdirectories in a filesystem object that is a directory");
        {
            constexpr size_t nStaticChars{ 88 + optionsubset_dir.size() };
            const size_t nStaticSpacing{ totalNumberOfStaticTableLines - nStaticChars };
            dynamicTableSpace(nStaticSpacing, m_optionsubset.size());
        }
        m_exception_message.writews("│ (6)", optionsubset_hor, '-', "for a horizontal listing of the contents in a filesystem object that is a directory");
        {
            constexpr size_t nStaticChars{ 93 + optionsubset_hor.size() };
            const size_t nStaticSpacing{ totalNumberOfStaticTableLines - nStaticChars };
            dynamicTableSpace(nStaticSpacing, m_optionsubset.size());
        }
    }


    void FileDiscoveryInput::_falseNotAnOption_errorhandling()
    {
        m_exception_message.emptyStrUsedByStream();//empty the string output stream before writting
        
        constexpr size_t totalNumberOfStaticTableLines{96};

        m_exception_message.write("┌──");
        {
            constexpr size_t nStaticChars{4};
            constexpr size_t nStaticTableLine{ totalNumberOfStaticTableLines - nStaticChars };
            dynamicTableLine(false, nStaticTableLine, m_option.size());
        }
        m_exception_message.writews("│ [", m_option, ']', "is not a valid option");
        {
            const size_t nStaticChars{ 29 + m_option.size() };
            const size_t nStaticSpacing{ totalNumberOfStaticTableLines - nStaticChars };
            dynamicTableSpace(nStaticSpacing, m_option.size());
        }
        m_exception_message.write("│ + List of valid options +");
        {
            constexpr size_t nStaticChars{28};
            constexpr size_t nStaticSpacing{ totalNumberOfStaticTableLines - nStaticChars };
            dynamicTableSpace(nStaticSpacing, m_option.size());
        }
        m_exception_message.write("│");
        {
            constexpr size_t nStaticChars{2};
            constexpr size_t nStaticSpacing{ totalNumberOfStaticTableLines - nStaticChars };
            dynamicTableSpace(nStaticSpacing, m_option.size());
        }

        m_exception_message.write("│ NON-MODIFYING OPTIONS:");
        {
            constexpr size_t nStaticChars{25};
            constexpr size_t nStaticSpacing{ totalNumberOfStaticTableLines - nStaticChars };
            dynamicTableSpace(nStaticSpacing, m_option.size());
        }
        _nonModOptions(totalNumberOfStaticTableLines);

        m_exception_message.write("│ LOCATION MODIFYING OPTIONS:");
        {
            constexpr size_t nStaticChars{30};
            constexpr size_t nStaticSpacing{ totalNumberOfStaticTableLines - nStaticChars };
            dynamicTableSpace(nStaticSpacing, m_option.size());
        }
        _locationModOptions(totalNumberOfStaticTableLines);

        m_exception_message.write("│ SPECIAL OPTIONS:");
        {
            constexpr size_t nStaticChars{19};
            constexpr size_t nStaticSpacing{ totalNumberOfStaticTableLines - nStaticChars };
            dynamicTableSpace(nStaticSpacing, m_option.size());
        }
        _specialOptions(totalNumberOfStaticTableLines);


        m_exception_message.write("├");
        {
            constexpr size_t nStaticChars{2};
            constexpr size_t nStaticSpacing{ totalNumberOfStaticTableLines - nStaticChars };
            dynamicTableMiddleLine(nStaticSpacing, m_option.size());
        }
        m_exception_message.writews("│ [Note] if your intention was to input a filesystem object that starts with [", std::to_underlying(Indicator::option), "] character,");
        {
            constexpr size_t nStaticChars{ 94 };
            constexpr size_t nStaticSpacing{ totalNumberOfStaticTableLines - nStaticChars };
            dynamicTableSpace(nStaticSpacing, m_option.size());
        }
        m_exception_message.write("│ it should be done surronded with quotes(either double or single),");
        {
            constexpr size_t nStaticChars{68};
            constexpr size_t nStaticSpacing{ totalNumberOfStaticTableLines - nStaticChars };
            dynamicTableSpace(nStaticSpacing, m_option.size());
        }
        m_exception_message.write("│ because it is special to the program.");
        {
            constexpr size_t nStaticChars{ 40 };
            constexpr size_t nStaticSpacing{ totalNumberOfStaticTableLines - nStaticChars };
            dynamicTableSpace(nStaticSpacing, m_option.size());
        }
        m_exception_message.write("└──");
        {
            constexpr size_t nStaticChars{4};
            constexpr size_t nStaticTableLine{ totalNumberOfStaticTableLines - nStaticChars };
            dynamicTableLine(true, nStaticTableLine, m_option.size());
        }

        throw FileDiscoveryInputError{m_exception_message.getStrUsedByStream()}; 
    }

    void FileDiscoveryInput::_falseNotANonModOptionSubset_errorhandling()
    {
        m_exception_message.emptyStrUsedByStream();//empty the string output stream before writting

        constexpr size_t totalNumberOfStaticTableLines{110};

        m_exception_message.write("┌──");
        {
            constexpr size_t nStaticChars{4};
            constexpr size_t nStaticTableLine{ totalNumberOfStaticTableLines - nStaticChars };
            dynamicTableLine(false, nStaticTableLine, m_optionsubset.size());
        }
        m_exception_message.writews("│ [", m_optionsubset, ']', "is not a valid option subset of option [", m_option, ']');
        {
            const size_t nStaticChars{ 51 + m_optionsubset.size() + m_option.size() };
            const size_t nStaticSpacing{ totalNumberOfStaticTableLines - nStaticChars };
            dynamicTableSpace(nStaticSpacing, m_optionsubset.size());
        }   
        m_exception_message.writews("│", m_option, "subsets:");
        {
            const size_t nStaticChars{ 12 + m_option.size() };
            const size_t nStaticSpacing{ totalNumberOfStaticTableLines - nStaticChars };
            dynamicTableSpace(nStaticSpacing, m_optionsubset.size());
        }
        m_exception_message.write("│");
        {
            constexpr size_t nStaticChars{2};
            constexpr size_t nStaticSpacing{ totalNumberOfStaticTableLines - nStaticChars };
            dynamicTableSpace(nStaticSpacing, m_optionsubset.size());
        }

        if ( isFindOption() )
        {
            _findOptionSubset(totalNumberOfStaticTableLines);
        }
        else if ( isFindInPwdOption() )
        {
            _findInPwdOptionSubset(totalNumberOfStaticTableLines);
        }
        else if ( isNewOption() )
        {
            _newOptionSubset(totalNumberOfStaticTableLines);
        }
        else if ( isLsOption() )
        {
            _lsOptionSubset(totalNumberOfStaticTableLines);
        }
        else
        {
            m_exception_message.writews("│ Does not have any subsets");
            {
                const size_t nStaticChars{28};
                const size_t nStaticSpacing{ totalNumberOfStaticTableLines - nStaticChars };
                dynamicTableMiddleLine(nStaticSpacing, m_optionsubset.size());
            }
        }


        m_exception_message.write("├");
        {
            constexpr size_t nStaticChars{2};
            constexpr size_t nStaticSpacing{ totalNumberOfStaticTableLines - nStaticChars };
            dynamicTableMiddleLine(nStaticSpacing, m_optionsubset.size());
        }
        m_exception_message.writews("│ [Note] if your intention was to input a filesystem object that starts with [", std::to_underlying(Indicator::optionsubset), "] character,");
        {
            constexpr size_t nStaticChars{94};
            constexpr size_t nStaticSpacing{ totalNumberOfStaticTableLines - nStaticChars };
            dynamicTableSpace(nStaticSpacing, m_optionsubset.size());
        }
        m_exception_message.write("│ it should be done surronded with quotes(either double or single),");
        {
            constexpr size_t nStaticChars{68};
            constexpr size_t nStaticSpacing{ totalNumberOfStaticTableLines - nStaticChars };
            dynamicTableSpace(nStaticSpacing, m_optionsubset.size());
        }
        m_exception_message.write("│ because it is special to the program.");
        {
            constexpr size_t nStaticChars{40};
            constexpr size_t nStaticSpacing{ totalNumberOfStaticTableLines - nStaticChars };
            dynamicTableSpace(nStaticSpacing, m_optionsubset.size());
        }
        m_exception_message.write("└──");
        {
            constexpr size_t nStaticChars{4};
            constexpr size_t nStaticTableLine{ totalNumberOfStaticTableLines - nStaticChars };
            dynamicTableLine(true, nStaticTableLine, m_optionsubset.size());
        }

        throw FileDiscoveryInputError{m_exception_message.getStrUsedByStream()};
    }

    void FileDiscoveryInput::_falseShouldNotHaveOptionSubset_errorhandling()
    {
        m_exception_message.emptyStrUsedByStream();//empty the string output stream before writting

        constexpr size_t totalNumberOfStaticTableLines{95};

        m_exception_message.write("┌──");
        {
            constexpr size_t nStaticChars{4};
            constexpr size_t nStaticTableLine{ totalNumberOfStaticTableLines - nStaticChars };
            dynamicTableLine(false, nStaticTableLine, m_option.size());
        }
        m_exception_message.writews("│ option [", m_option, ']', "should not have a subset");
        {
            const size_t nStaticChars{ 39 + m_option.size() };
            const size_t nStaticSpacing{ totalNumberOfStaticTableLines - nStaticChars };
            dynamicTableSpace(nStaticSpacing, m_option.size());
        }

        m_exception_message.write("├");
        {
            constexpr size_t nStaticChars{2};
            constexpr size_t nStaticSpacing{ totalNumberOfStaticTableLines - nStaticChars };
            dynamicTableMiddleLine(nStaticSpacing, m_option.size());
        }

        m_exception_message.writews("│ [Note] if your intention was to input a filesystem object that starts with [", std::to_underlying(Indicator::optionsubset), "] character,");
        {
            constexpr size_t nStaticChars{ 94 };
            constexpr size_t nStaticSpacing{ totalNumberOfStaticTableLines - nStaticChars };
            dynamicTableSpace(nStaticSpacing, m_option.size());
        }

        m_exception_message.write("│ it should be done surronded with quotes(either double or single),");
        {
            constexpr size_t nStaticChars{68};
            constexpr size_t nStaticSpacing{ totalNumberOfStaticTableLines - nStaticChars };
            dynamicTableSpace(nStaticSpacing, m_option.size());
        }

        m_exception_message.write("│ because it is special to the program.");
        {
            constexpr size_t nStaticChars{ 40 };
            constexpr size_t nStaticSpacing{ totalNumberOfStaticTableLines - nStaticChars };
            dynamicTableSpace(nStaticSpacing, m_option.size());
        }

        m_exception_message.write("└──");
        {
            constexpr size_t nStaticChars{4};
            constexpr size_t nStaticTableLine{ totalNumberOfStaticTableLines - nStaticChars };
            dynamicTableLine(true, nStaticTableLine, m_option.size());
        }

        throw FileDiscoveryInputError{m_exception_message.getStrUsedByStream()}; 
    }


    void FileDiscoveryInput::dynamicTableLine(bool up_down, size_t nStaticChars, size_t nDynamicChars)
    {
        for (size_t i = 0; i < (nStaticChars + nDynamicChars); i++)
        {
            m_exception_message.write("─");
        }
        if (!up_down)
        m_exception_message.write_endl("┐");
        else
        m_exception_message.write_endl("┘");
    }

    void FileDiscoveryInput::dynamicTableSpace(size_t nStaticChars, size_t nDynamicChars)
    {
        for (size_t i = 0; i < (nStaticChars + nDynamicChars); i++)
        {
            m_exception_message.write(' ');
        }
        m_exception_message.write_endl("│");
    }

    void FileDiscoveryInput::dynamicTableMiddleLine(size_t nStaticChars, size_t nDynamicChars)
    {
        for (size_t i = 0; i < (nStaticChars + nDynamicChars); i++)
        {
            m_exception_message.write("─");
        }
        m_exception_message.write_endl("┤");
    }



    std::istream& operator>>(std::istream& in, FileDiscoveryInput& input)
    {
        while (true)
        {
            try
            {
                std::getline(in >> std::ws, input.m_input);

                if (in.eof())
                return in;
                
                input.parse_input(); //throws an exception when an input is invalid

                break; //This never executes if FileDiscoveryInput::parse_input throws an exception
            }
            catch(const FileDiscoveryInputError& e)
            {
                System::cerror.write_endl("");
                System::cerror.writews_endl(e.what());
                System::cerror.write_endl("");

                input.m_option = {};
                input.m_optionsubset = {};
                input.m_filesystemobj = {};
            }
        }
        return in;
    }

}