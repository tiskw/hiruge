////////////////////////////////////////////////////////////////////////////////////////////////////
/// C++ source - window.cxx                                                                      ///
////////////////////////////////////////////////////////////////////////////////////////////////////

// Include primary header.
#include "window.hxx"

// Include standard libraries.
#include <string>

// Include custom headers.
#include "config.hxx"
#include "complete.hxx"

////////////////////////////////////////////////////////////////////////////////////////////////////
// Constants
////////////////////////////////////////////////////////////////////////////////////////////////////

// Message that is shown if no matched command found.
#define STR_COMMAND_NOT_FOUND ("Command not found")

////////////////////////////////////////////////////////////////////////////////////////////////////
// Static functions
////////////////////////////////////////////////////////////////////////////////////////////////////

static bool
is_num_or_alph(const char c)
// [Abstract]
//   Returns true if the given character is an alphabet.
//
// [Args]
//   c (const char): [IN] Input character.
//
// [Returns]
//   (bool): True or false.
//
{   // {{{

    return ((32 <= (c)) && ((c) <= 126));

}   // }}}

static Window
init_window(Display *display)
// [Abstract]
//   Initialize a new window and return it.
//
// [Args]
//   display (Display): [IN] X11 display.
//
// [Returns]
//   (Window): Initialized new window.
//
{   // {{{

    // Get a root window.
    Window root = RootWindow(display, 0);

    // Get the size of screen.
    int root_width  = DisplayWidth(display, 0);
    int root_height = DisplayHeight(display, 0);

    // Get the black/white color.
    unsigned long black = BlackPixel(display, 0);
    unsigned long white = WhitePixel(display, 0);

    // Create a new window.
    Window window = XCreateSimpleWindow(display, root,
            (root_width - config.window_width) / 2, (root_height - config.window_height) / 2,
            config.window_width, config.window_height, config.window_border, white, black);

    // Set window size.
    XSizeHints hints;
    hints.flags  = PPosition | PSize;
    hints.x      = config.window_width;
    hints.y      = config.window_height;
    hints.width  = (root_width  - config.window_width)  / 2;
    hints.height = (root_height - config.window_height) / 2;
    XSetNormalHints(display, window, &hints);

    return window;

}   // }}}

static XftColors
init_xft_colors(Display* display, const Colormap cmap)
// [Abstract]
//   Initialize colors.
//
// [Args]
//   display (Display)       : [IN] X11 display.
//   cmap    (const Colormap): [IN] Color map of X11.
//
// [Returns]
//   (XftColors): Collection of colors.
//
{   // {{{

    XftColors colors;

    XftColorAllocName(display, DefaultVisual(display, 0), cmap, "white", &colors.white);
    XftColorAllocName(display, DefaultVisual(display, 0), cmap, "black", &colors.black);
    XftColorAllocName(display, DefaultVisual(display, 0), cmap, "red",   &colors.red);
    XftColorAllocName(display, DefaultVisual(display, 0), cmap, "green", &colors.green);
    XftColorAllocName(display, DefaultVisual(display, 0), cmap, "blue",  &colors.blue);

    return colors;

}   // }}}

////////////////////////////////////////////////////////////////////////////////////////////////////
// Constructors and descructors
////////////////////////////////////////////////////////////////////////////////////////////////////

MainWindow::MainWindow(void)
{   // {{{

    // Initialize member variables.
    this->display  = XOpenDisplay(0);
    this->window   = init_window(this->display);
    this->xft_font = XftFontOpen(this->display, 0, XFT_FAMILY, XftTypeString, config.xft_fontname.c_str(), XFT_SIZE, XftTypeDouble, config.xft_fontsize, NULL);
    this->cmap     = DefaultColormap(this->display, 0);
    this->colors   = init_xft_colors(this->display, this->cmap);
    this->draw     = XftDrawCreate(this->display, this->window, DefaultVisual(this->display, 0), this->cmap);

    // Create and initialize X Window.
    XMapWindow(this->display, this->window);
    XFlush(this->display);

    // Define raised event from X window.
    XSelectInput(this->display, this->window, KeyPressMask | KeyReleaseMask | ExposureMask);

}   // }}}

MainWindow::~MainWindow(void)
{   // {{{

    XftDrawDestroy(this->draw);
    XCloseDisplay(this->display);

}   // }}}

////////////////////////////////////////////////////////////////////////////////////////////////////
// Public functions
////////////////////////////////////////////////////////////////////////////////////////////////////

void
MainWindow::start(int argc, char *argv[], Complete& complete)
{   // {{{

    // Set window title.
    XTextProperty win_prop;
    win_prop.value    = (unsigned char*) config.window_title.c_str();
    win_prop.encoding = XA_STRING;
    win_prop.format   = 8;
    win_prop.nitems   = strlen((char*) win_prop.value);
    XSetWMProperties(display, window, &win_prop, NULL, argv, argc, NULL, NULL, NULL);

    // User input.
    std::string input;

    //
    XEvent event;
    KeySym sym;
    char   key;

    while (true)
    {
        XNextEvent(this->display, &event);

        switch (event.type)
        {
            // Expose Event
            case Expose:
                redraw_window(input, complete);
                break;

            // KeyPress Event
            case KeyPress:

                // Get pressed key
                sym = XLookupKeysym(&event.xkey, 0);
                key = (char) sym;

                // RETURN key: Execute command and close window
                if (key == '\r' || key == '\n')
                {
                    complete.exec(input);
                    return;
                }

                // Printable key: Add typed key to input string
                else if (is_num_or_alph(key))
                {
                    input.push_back(sym);
                    complete.update(input);
                    redraw_window(input, complete);
                }

                // ESCAPE key: Close window
                else if (key == 27)
                    return;

                // BACKSPACE key: Erase the last one character from input string if exists
                else if (key == 8) // Backspace
                {
                    if (input.size() > 0)
                        input.pop_back();

                    complete.update(input);
                    redraw_window(input, complete);
                }

                // Do nothing for other keys
                break;

            // KeyRelease Event
            case KeyRelease: break;

            // Others
            default: break;
        }
    }

}   // }}}

////////////////////////////////////////////////////////////////////////////////////////////////////
// Private functions
////////////////////////////////////////////////////////////////////////////////////////////////////

void
MainWindow::redraw_window(const std::string& input, Complete& complete)
{   // {{{

    XClearArea(this->display, this->window, 0, 0, config.window_width, config.window_height, false);

    const std::string msg1 = "Command  : " + input;
    const std::string msg2 = "Candidate: " + complete.get(0, STR_COMMAND_NOT_FOUND);

    XftDrawStringUtf8(this->draw, &this->colors.white, this->xft_font, config.text_left_margin, config.text_top1_margin, (FcChar8*) msg1.c_str(), msg1.size());
    XftDrawStringUtf8(this->draw, &this->colors.green, this->xft_font, config.text_left_margin, config.text_top2_margin, (FcChar8*) msg2.c_str(), msg2.size());

}   // }}}

// vim: expandtab tabstop=4 shiftwidth=4 fdm=marker
