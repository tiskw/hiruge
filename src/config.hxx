////////////////////////////////////////////////////////////////////////////////////////////////////
/// C++ header file: config.hxx                                                                  ///
///                                                                                              ///
/// This file provides:                                                                          ///
///   - the global variable `config` which stores configuration values for HiRuGe.               ///
///   - the function `load_config` which reads config file and update `config` variable.         ///
////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef CONFIG_HXX
#define CONFIG_HXX

// Include the headers of STL.
#include <cstdint>
#include <map>
#include <string>
#include <vector>

////////////////////////////////////////////////////////////////////////////////////////////////////
// Data type declaration
////////////////////////////////////////////////////////////////////////////////////////////////////

typedef struct
{
    // [GENERAL] settings.
    int32_t     window_width;
    int32_t     window_height;
    int32_t     window_border;
    std::string window_title;
    int32_t     text_left_margin;
    int32_t     text_top1_margin;
    int32_t     text_top2_margin;
    std::string xft_fontname;
    double      xft_fontsize;

    // [ALIAS] settings.
    std::map<std::string, std::string> aliases;
}
Config;

////////////////////////////////////////////////////////////////////////////////////////////////////
// Prototype declaration
////////////////////////////////////////////////////////////////////////////////////////////////////

extern Config config;
// Config values.

////////////////////////////////////////////////////////////////////////////////////////////////////
// Public functions
////////////////////////////////////////////////////////////////////////////////////////////////////

void
load_config(const std::string filepath) noexcept;
// [Abstract]
//   Load config file written in TOML format.
//   The result will be stored in the global variable `config` that is declared in `config.cxx`.
//
// [Args]
//   filepath (const std::string): [IN] Path to TOML file.

#endif

// vim: expandtab shiftwidth=4 shiftwidth=4 fdm=marker
