#[[
this is here because of a small bug in the TIGCC frontend.
it does not parse file names like modern GNU GCC frontend does...
only files of certain extension (determined by strrchr) are accepted.
even specify -c or whatever. if the file extension doesn't match, it will bail!

CMake likes to use .obj by default... tigcc will say screw you! it should be .o!
In reality, it doesnt even tell you that much....
It errors out because it interprets the filename as an option (lol) if the extension
does not match .o, .c, .a, etc...

So we specify CMake to use .o instead of .obj. Even this is a kinda bad workaround,
its much better than rewriting all of their argparsing code (which should be done!)

and no, this cannot be set in the toolchain file!
i wish it could! but alas ... we have to do this override stuff. 
declaring CMAKE_C_OUTPUT_EXTENSION it in the toolchain file is both:
    against the spirit of cmake (https://gitlab.kitware.com/cmake/cmake/-/merge_requests/1941#note_398139)
    and does not work (https://gitlab.kitware.com/cmake/cmake/-/issues/18713)

thx to https://cmake.org/pipermail/cmake/2008-April/021337.html
- cd 6.15.2025
#]]

set(CMAKE_C_OUTPUT_EXTENSION .o)