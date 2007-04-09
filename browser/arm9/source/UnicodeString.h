#ifndef UnicodeString_h_seen
#define UnicodeString_h_seen
#include <string>

/** This typedef is used to represent unicode strings. Unicode character
 * values are up to 32 bit so an unsigned int is used.
 */
typedef std::basic_string<unsigned int> UnicodeString;

#endif
