////////////////////////////////////////////////////////////////////////////////////////////////////
/// C++ source - main.cxx                                                                        ///
////////////////////////////////////////////////////////////////////////////////////////////////////

// Include standard libraries.
#include <cstdint>

// Include X11 headers.
#include <X11/Xlib.h>
#include <X11/Xft/Xft.h>
#include <X11/Xutil.h>
#include <X11/Xatom.h>

// Include custom headers.
#include "complete.hxx"
#include "config.hxx"
#include "window.hxx"

////////////////////////////////////////////////////////////////////////////////////////////////////
// Main function
////////////////////////////////////////////////////////////////////////////////////////////////////

int32_t
main(int32_t argc, char *argv[])
{   // {{{

    // Load config file.
    load_config("auto");

    // Initialize command complete module.
    Complete complete;

    // Start window.
    MainWindow window;
    window.start(argc, argv, complete);

    return EXIT_SUCCESS;

}   // }}}

// vim: expandtab tabstop=4 shiftwidth=4 fdm=marker
