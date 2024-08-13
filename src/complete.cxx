////////////////////////////////////////////////////////////////////////////////////////////////////
/// C++ source - complete.cxx                                                                    ///
////////////////////////////////////////////////////////////////////////////////////////////////////

// Include primary header.
#include "complete.hxx"

// Include standard libraries.
#include <algorithm>
#include <filesystem>

// Include custom headers.
#include "config.hxx"

////////////////////////////////////////////////////////////////////////////////////////////////////
// Constants
////////////////////////////////////////////////////////////////////////////////////////////////////

// Maxium number of candidates.
#define N_MAX_CANDIDATES (8)

////////////////////////////////////////////////////////////////////////////////////////////////////
// Static functions
////////////////////////////////////////////////////////////////////////////////////////////////////

static bool
is_substr(const std::string& str1, const std::string& str2) noexcept
// [Abstract]
//   Returns true if the "str1" is a substring of "str2".
//
// [Args]
//   str1 (const std::string&): [IN] The 1st string.
//   str2 (const std::string&): [IN] The 2nd string.
//
// [Returns]
//   (bool): True if the "str1" is a substring of "str2".
//
{   // {{{

    // Initialize the string index.
    size_t idx = 0;

    // Increment the index while the character "str1[idx]" and "str2[idx]" are the same.
    while ((idx < str1.size()) and (idx < str2.size()) and (str1[idx] == str2[idx]))
        ++idx;

    // Returns true if the "str1" is used up.
    if (idx >= str1.size()) return true;
    else                    return false;

}   // }}}

static std::vector<std::string>
split(const std::string& str, const std::string& delim) noexcept
// [Abstract]
//   Split the given string using the given delimiter.
//
// [Args]
//   str   (const std::string&): [IN] Target string.
//   delim (const std::string&): [IN] Delimiter string.
//
// [Returns]
//   (std::vector<std::string>): Splitted strings.
//
{   // {{{

    // Initialize the output array.
    std::vector<std::string> result;

    // Do nothing if the delimiter is an empty string.
    if (delim.size() == 0)
    {
        result.emplace_back(str);
        return result;
    }

    // Initialize offset which indicates current position.
    std::string::size_type offset = 0;

    while (true)
    {
        // Find next target.
        const std::string::size_type pos = str.find(delim, offset);

        // Returns if no next target found.
        if (pos == std::string::npos)
        {
            result.emplace_back(str.substr(offset));
            return result;
        }

        // Otherwise, memorize the found target and update the offset value.
        result.emplace_back(str.substr(offset, pos - offset));
        offset = pos + delim.size();
    }

}   // }}}

static void
get_system_commands(std::vector<std::string>& target) noexcept
// [Abstract]
//   Search all command in "PATH" environment variable and store them
//   into the given "target" variable.
//
// [Args]
//    target (std::vector<std::string>&): [OUT] The command name will be stored in this variable.
//
{   // {{{

    // Split PATH by ':'.
    std::vector<std::string> target_paths = split(std::string(std::getenv("PATH")), ":");

    // Remove duplicated taregt path.
    target_paths.erase(std::unique(target_paths.begin(), target_paths.end()), target_paths.end());

    // Search all directories in PATH and get all executable files.
    for (const std::string& path : target_paths)
    {
        // Skip if the path is not a directory.
        if (not std::filesystem::is_directory(path))
            continue;

        // Appand all files in the directory.
        for (const std::filesystem::directory_entry& entry : std::filesystem::directory_iterator(path))
        {
            // Skip if the path is not a regular file or symbolic link.
            if (not (entry.is_regular_file() or entry.is_symlink()))
                continue;

            // Add the command name.
            target.emplace_back(entry.path().filename());
        }
    }

}   // }}}

////////////////////////////////////////////////////////////////////////////////////////////////////
// Constructors and descructors
////////////////////////////////////////////////////////////////////////////////////////////////////

Complete::Complete(void)
{   // {{{

    // Search all command names in "PATH" environment variable
    // and store them into "this->commands".
    get_system_commands(this->commands);

    // Add all aliases to "this->commands".
    for (const auto& item : config.aliases)
        this->commands.emplace_back(item.first);

    // Sort the array of command names.
    std::sort(this->commands.begin(), this->commands.end());

    // Remove duplicated command names.
    this->commands.erase(std::unique(this->commands.begin(), this->commands.end()), this->commands.end());

}   // }}}

////////////////////////////////////////////////////////////////////////////////////////////////////
// Member functions
////////////////////////////////////////////////////////////////////////////////////////////////////

int32_t
Complete::exec(const std::string& input) const noexcept
{   // {{{

    // Get the command name to be executed.
    std::string target = this->get(0, input);

    // If the command name exists in the aliases, then replace to the alias contents.
    if (config.aliases.find(target) != config.aliases.end())
        target = config.aliases[target];

    // Create a command to be executed.
    const std::string cmd = target + " 1>/dev/null 2>/dev/null &";

    // Execite the command.
    return system(cmd.c_str());

}   // }}}

const std::string&
Complete::get(const size_t index, const std::string& default_value) const noexcept
{   // {{{

    // Returns the corresponding candidate if the index is valid.
    if (index < this->candidates.size())
        return *this->candidates[index];

    // Otherwise, returns the default value.
    return default_value;

}   // }}}

void
Complete::update(const std::string& input) noexcept
{   // {{{

    // Clear all candidates.
    this->candidates.clear();

    // Do nothing if the user input is empty.
    if (input.size() == 0)
        return;

    for (const std::string& name : this->commands)
    {
        // Register as a candidate if the user input is a substring of the command name.
        if (is_substr(input, name))
            this->candidates.push_back(&name);

        // Exit the loop if the number of candidates exceeds the max number, because
        // the computation time will unnecessarily increase if the number of candidate is too big.
        if (this->candidates.size() >= N_MAX_CANDIDATES)
            break;

        // Exit the loop if the user input is smaller than the command name in terms of
        // dictionary order, because the rest of command names never match.
        if (input.compare(name) <= 0)
            break;
    }

}   // }}}

// vim: expandtab shiftwidth=4 shiftwidth=4 fdm=marker
