////////////////////////////////////////////////////////////////////////////////////////////////////
/// C++ header - complete.hxx                                                                    ///
///                                                                                              ///
/// This file provides the "Complete" class that manages the command name completion from        ///
/// a given user input in a real time speed.                                                     ///
////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef COMPLETE_HXX
#define COMPLETE_HXX

// Include standard libraries.
#include <string>
#include <vector>

////////////////////////////////////////////////////////////////////////////////////////////////////
// Class definition
////////////////////////////////////////////////////////////////////////////////////////////////////

class Complete
{
    public:

        ////////////////////////////////////////////////////////////////////////////////////////////
        // Constructors and descructors
        ////////////////////////////////////////////////////////////////////////////////////////////

        Complete(void);

        ////////////////////////////////////////////////////////////////////////////////////////////
        // Member functions
        ////////////////////////////////////////////////////////////////////////////////////////////

        int32_t
        exec(const std::string& input) const noexcept;
        // [Abstract]
        //   Complete the given user input and execute it.
        //
        // [Args]
        //   input (const std::string&): [IN] User input.
        //
        // [Returns]
        //   (int32_t): Returns value of the executed command.

        const std::string&
        get(const size_t index, const std::string& default_value) const noexcept;
        // [Abstract]
        //   Returns a candidate of the given index. This function returns
        //   the given default value if the given index is out of range.
        //
        // [Args]
        //   index         (const size_t) [IN] Index of the candidate to be returned.
        //   default_value (const size_t) [IN] Defalut value.
        //
        // [Returns]
        //   (const std::string&): Candidate at the given index, or the default value.

        void
        update(const std::string& input) noexcept;
        // [Abstract]
        //   Update the candidate based on the given user input.
        //
        // [Args]
        //   input (const std::string&): [IN] User input.

    private:

        ////////////////////////////////////////////////////////////////////////////////////////////
        // Private member variables
        ////////////////////////////////////////////////////////////////////////////////////////////

        std::vector<std::string> commands;
        // List of possible command names.

        std::vector<const std::string*> candidates;
        // List of candidates.
        // The elements of this vector is a pointer of strings in "this->commands".
};

#endif

// vim: expandtab shiftwidth=4 shiftwidth=4 fdm=marker
