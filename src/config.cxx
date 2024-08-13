////////////////////////////////////////////////////////////////////////////////////////////////////
/// C++ source file: config.cxx                                                                  ///
////////////////////////////////////////////////////////////////////////////////////////////////////

// Include the primary header.
#include "config.hxx"

// Include the headers of STL.
#include <algorithm>
#include <filesystem>
#include <iostream>

// Include the header of the toml++ library.
#define TOML_EXCEPTIONS 0
#include <toml.hpp>

////////////////////////////////////////////////////////////////////////////////////////////////////
// Global variable
////////////////////////////////////////////////////////////////////////////////////////////////////

Config config;

////////////////////////////////////////////////////////////////////////////////////////////////////
// Static functions
////////////////////////////////////////////////////////////////////////////////////////////////////

static void
show_error_message(const toml::key& section, const toml::key& value) noexcept
// [Abstract]
//   Show error message and quit this software.
//
// [Args]
//   section (const toml::key&): [IN] Section name.
//   value   (const toml::key&): [IN] Value name.
//
{   // {{{

    std::cout << "\033[33m";
    std::cout << "NiShiKi: Error occured while parsing config file\n";
    std::cout << "NiShiKi: Undefined entry name: " << section << "." << value;
    std::cout << "\033[m" << std::endl;

};  // }}}

static void
set_config(const toml::table& table, const toml::key& section, const toml::key& value) noexcept
// [Abstract]
//   Read one config item to the global variable `config`.
//
// [Args]
//   table   (const toml::table&): [IN] Top node of the config file.
//   section (const toml::key&)  : [IN] Section name.
//   value   (const toml::key&)  : [IN] Value name.
//
{   // {{{

    // Get target config item.
    toml::node_view node = table[section][value];

    ////////////////////////////////////////////////////////////////////////////////////////////////
    // Read the [GENERAL] section.
    ////////////////////////////////////////////////////////////////////////////////////////////////

    if      ((section == "GENERAL") and (value == "window_width"    )) config.window_width     = node.value_or(config.window_width);
    else if ((section == "GENERAL") and (value == "window_height"   )) config.window_height    = node.value_or(config.window_height);
    else if ((section == "GENERAL") and (value == "window_border"   )) config.window_border    = node.value_or(config.window_border);
    else if ((section == "GENERAL") and (value == "window_title"    )) config.window_title     = node.value_or(config.window_title);
    else if ((section == "GENERAL") and (value == "text_left_margin")) config.text_left_margin = node.value_or(config.text_left_margin);
    else if ((section == "GENERAL") and (value == "text_top1_margin")) config.text_top1_margin = node.value_or(config.text_top1_margin);
    else if ((section == "GENERAL") and (value == "text_top2_margin")) config.text_top2_margin = node.value_or(config.text_top2_margin);
    else if ((section == "GENERAL") and (value == "xft_fontname"    )) config.xft_fontname     = node.value_or(config.xft_fontname);
    else if ((section == "GENERAL") and (value == "xft_fontsize"    )) config.xft_fontsize     = node.value_or(config.xft_fontsize);
    else if ((section == "GENERAL")                                  ) show_error_message(section, value);

    ////////////////////////////////////////////////////////////////////////////////////////////////
    // Read the [ALIAS] section.
    ////////////////////////////////////////////////////////////////////////////////////////////////

    else if (section == "ALIAS")
    {
        // Get key and value, and registers them to the member variable.
        const std::string key = std::string(value.str());
        const std::string val = node.value_or("");

        config.aliases[key] = val;
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////
    // Otherwise, show error message and exit.
    ////////////////////////////////////////////////////////////////////////////////////////////////

    else show_error_message(section, value);

}   // }}}

////////////////////////////////////////////////////////////////////////////////////////////////////
// Public functions
////////////////////////////////////////////////////////////////////////////////////////////////////

void
load_config(std::string filepath) noexcept
{   // {{{

    ////////////////////////////////////////////////////////////////////////////////////////////////
    // Set default values
    ////////////////////////////////////////////////////////////////////////////////////////////////

    // The [GENERAL] settings.
    config.window_width     = 400;
    config.window_height    = 80;
    config.window_border    = 0;
    config.window_title     = "HiRuGe: software launcher";
    config.text_left_margin = 10;
    config.text_top1_margin = 30;
    config.text_top2_margin = 60;
    config.xft_fontname     = "DejaVu Sans Mono";
    config.xft_fontsize     = 14.0;

    // The [ALIAS] settings.
    config.aliases.clear();

    ////////////////////////////////////////////////////////////////////////////////////////////////
    // Determine config file path
    ////////////////////////////////////////////////////////////////////////////////////////////////

    // Compute the filepath to the config file if "auto" is specified.
    //
    // Priority rule of the loading target is:
    //   1. ./config.toml
    //   2. ~/.config/nishiki/config.toml
    //   3. ~/.local/nishiki/config.toml
    //
    if (filepath == "auto")
    {
        std::filesystem::path home  = std::filesystem::path(getenv("HOME"));
        std::filesystem::path path1 = std::filesystem::path(".") / "config.toml";
        std::filesystem::path path2 = home / ".config" / "nishiki" / "config.toml";
        std::filesystem::path path3 = home / ".local"  / "nishiki" / "config.toml";

        if      (std::filesystem::exists(path1)) { filepath = std::filesystem::canonical(path1).string(); }
        else if (std::filesystem::exists(path2)) { filepath = std::filesystem::canonical(path2).string(); }
        else if (std::filesystem::exists(path3)) { filepath = std::filesystem::canonical(path3).string(); }
        else                                     { filepath = "config.toml";                              }
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////
    // Read confg file
    ////////////////////////////////////////////////////////////////////////////////////////////////

    // Read specified TOML file.
    toml::parse_result result = toml::parse_file(filepath);
    if (not result)
    {
        std::cout << "\033[33mHiRuGe: Error occured while parsing config file: " << filepath << "\033[m\n";
        std::cout << "\033[33mHiRuGe: " << result.error() << "\033[m" << std::endl;
        return;
    }

    // Steal the table from the result.
    toml::table table = std::move(result).table();

    ////////////////////////////////////////////////////////////////////////////////////////////////
    // Parse config contents
    ////////////////////////////////////////////////////////////////////////////////////////////////

    for (const auto& node_section : table)
    {
        // If the node is a table then read the values contained in the table.
        if (node_section.second.is_table())
            for (auto node_value : *node_section.second.as_table())
                set_config(table, node_section.first, node_value.first);
    }

}   // }}}

// vim: expandtab shiftwidth=4 shiftwidth=4 fdm=marker
