/*
  Copyright (C) 2007,2008 Richard Quirk

  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
#include <set>
#include "config_defs.h"
#include "string_utils.h"
#include "utf8.h"
using std::string;
using std::vector;

#if 0
std::string unicode2string(const UnicodeString & ustr, bool byteencode)
{
  std::string str;
  UnicodeString::const_iterator it(ustr.begin());
  unsigned char encoded[6];
  for ( ; it != ustr.end() ; ++it)
  {
    int used = UTF8::encode(*it, encoded);
    if (used == 1) {
      str += *it;
    }
    else
    {
      char buffer[4];
      for (int i = 0; i < used; ++i)
      {
        if (byteencode)
        {
          str += (char)encoded[i];
        }
        else
        {
          // convert to %hex
          str += '%';
          sprintf_platform(buffer, "%02X", encoded[i]);
          str += buffer;
        }
      }
    }
  }
  return str;
}

UnicodeString string2unicode(const std::string & str)
{
  UnicodeString ustr;
  const char * p = str.c_str();
  size_t len = str.length();
  for (size_t i = 0 ; i < len; )
  {
    unsigned int val;
    unsigned int read = UTF8::decode(p, val);
    ustr += val;
    p += read;
    i += read;
  }
  return ustr;
}
#endif

bool isWhitespace(unsigned int value)
{
  if (value > 0x0020)
    return false;
  return (
         value == 0x0020  // SPACE
      or value == 0x0009  // CHARACTER TABULATION
      or value == 0x000A  // LINE FEED
      or value == 0x000B  // LINE TABULATION
      or value == 0x000C  // FORM FEED
      );
}

void stripWhitespace(std::string & modify)
{
  if (modify.empty())
    return;
  static const std::string delimter(" \r\n	");
  size_t firstNonBlank = modify.find_first_not_of(delimter);
  size_t lastNonBlank = modify.find_last_not_of(delimter);
  if (firstNonBlank != std::string::npos and lastNonBlank != std::string::npos)
    modify = modify.substr(firstNonBlank, (lastNonBlank-firstNonBlank+1));
}

#if 0
void stripWhitespace(UnicodeString & modify)
{
  if (modify.empty())
    return;
  static const UnicodeString delimter(string2unicode(" \r\n	"));
  int firstNonBlank = modify.find_first_not_of(delimter);
  int lastNonBlank = modify.find_last_not_of(delimter);
  modify = modify.substr(firstNonBlank, (lastNonBlank-firstNonBlank+1));
}
#endif
void tokenize(const std::string &str,
              std::vector<std::string> &tokens,
              const std::string &delimiters)
{
  // Skip delimiters at beginning.
  std::string::size_type lastPos = str.find_first_not_of(delimiters, 0);
  // Find first "non-delimiter".
  std::string::size_type pos     = str.find_first_of(delimiters, lastPos);

  while (std::string::npos != pos || std::string::npos != lastPos)
  {
    // Found a token, add it to the vector.
    tokens.push_back(str.substr(lastPos, pos - lastPos));
    // Skip delimiters.  Note the "not_of"
    lastPos = str.find_first_not_of(delimiters, pos);
    // Find next "non-delimiter"
    pos = str.find_first_of(delimiters, lastPos);
  }
}

void split(const std::string &str,
              std::vector<std::string> &tokens,
              const std::string &delimiters)
{
  // pythonic split, includes empties
  // 'this|string||is|split' -> ['this','string','','is','split']

  // Special case, delimiter at start
  std::string::size_type lastPos = 0;
  std::string::size_type pos = str.find_first_of(delimiters, lastPos);
  tokens.push_back(str.substr(lastPos, pos - lastPos));
  lastPos = pos;
  if (lastPos != std::string::npos)
    pos = str.find_first_of(delimiters, lastPos+1);

  while (lastPos != std::string::npos || pos != std::string::npos)
  {
    // skip delimeter (+1) subtract 1 for length. may add empty str
    tokens.push_back(str.substr(lastPos+1, pos - 1 - lastPos));
    lastPos = pos;
    if (lastPos != std::string::npos)
      pos = str.find_first_of(delimiters, lastPos+1);
  }
}

std::string nextWordAdvanceWord(
    std::string::const_iterator *it,
    const std::string::const_iterator &end_it,
    bool parseNewline)
{
  // set up delimeter set
  std::set<uint32_t> delimeters;
  static const char intDelimiters[] = {0x20, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0};
  for (const char *d(intDelimiters); *d != 0; ++d) {
    delimeters.insert(*d);
  }
  std::set<uint32_t>::const_iterator set_end(delimeters.end());

  // skip whitespace at the start of the word
  if (!parseNewline) {
    while (*it != end_it) {
      uint32_t value = utf8::peek_next(*it, end_it);
      if (delimeters.find(value) == set_end) {
        break;
      }
      utf8::next(*it, end_it);
    }
  }
  // find the end of the word
  std::string::const_iterator start(*it);
  while (*it != end_it) {
    uint32_t value = utf8::next(*it, end_it);
    if (delimeters.find(value) != set_end) {
      break;
    }
  }
  return string(start, *it);
}

size_t findLastNotOf(const std::string &str, const std::string &delim) {
  size_t last = str.find_last_not_of(delim);
  std::string::const_iterator it(str.begin());
  it += last;
  if (it == str.end()) {
    return last;
  }
  while (!utf8::is_valid(it, str.end())) {
    ++it;
    ++last;
  }
  return last;
}