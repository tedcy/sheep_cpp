#pragma once
#include <string>
#include <cstring>
namespace small_http_parser{
namespace c_version {
/* from here:
 * http://www.geekhideout.com/urlcode.shtml
 * https://stackoverflow.com/questions/154536/encode-decode-urls-in-c
*/
/* Converts a hex character to its integer value */
inline char from_hex(char ch) {
  return isdigit(ch) ? ch - '0' : tolower(ch) - 'a' + 10;
}

/* Converts an integer value to its hex character*/
inline char to_hex(char code) {
  static char hex[] = "0123456789abcdef";
  return hex[code & 15];
}

/* Returns a url-encoded version of str */
/* IMPORTANT: be sure to free() the returned string after use */
inline char *url_encode(const char * str) {
  const char *pstr = str;
  char *buf = static_cast<char*>(std::malloc(std::strlen(str) * 3 + 1)), *pbuf = buf;
  while (*pstr) {
    if (isalnum(*pstr) || *pstr == '-' || *pstr == '_' || *pstr == '.' || *pstr == '~') 
      *pbuf++ = *pstr;
    else if (*pstr == ' ') 
      *pbuf++ = '+';
    else 
      *pbuf++ = '%', *pbuf++ = to_hex(*pstr >> 4), *pbuf++ = to_hex(*pstr & 15);
    pstr++;
  }
  *pbuf = '\0';
  return buf;
}

/* Returns a url-decoded version of str */
/* IMPORTANT: be sure to free() the returned string after use */
inline char *url_decode(const char *str) {
  const char *pstr = str;
  char *buf = static_cast<char*>(std::malloc(std::strlen(str) + 1)), *pbuf = buf;
  while (*pstr) {
    if (*pstr == '%') {
      if (pstr[1] && pstr[2]) {
        *pbuf++ = from_hex(pstr[1]) << 4 | from_hex(pstr[2]);
        pstr += 2;
      }
    } else if (*pstr == '+') { 
      *pbuf++ = ' ';
    } else {
      *pbuf++ = *pstr;
    }
    pstr++;
  }
  *pbuf = '\0';
  return buf;
}
}
//TODO can be more effective
inline std::string UrlEncode(const std::string &str) {
    char *c = c_version::url_encode(str.c_str());
    std::string s(c, std::strlen(c));
    free(c);
    return s;
}
inline std::string UrlDecode(const std::string &str) {
    char *c = c_version::url_decode(str.c_str());
    std::string s(c, std::strlen(c));
    free(c);
    return s;
}
}
