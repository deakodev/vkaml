#ifndef VKAML_PLATFORM_H
#define VKAML_PLATFORM_H

// Platform detection
#if defined(__linux__)
#    define VKAML_LINUX 
#else
#    error "Unsupported platform!"
#endif

#endif // VKAML_PLATFORM_H