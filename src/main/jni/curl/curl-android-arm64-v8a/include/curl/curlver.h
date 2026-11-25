#ifndef __CURL_CURLVER_H
#define __CURL_CURLVER_H

#define LIBCURL_COPYRIGHT "1996 - 2016 Daniel Stenberg, <daniel@haxx.se>."

#define LIBCURL_VERSION "7.51.0"

#define LIBCURL_VERSION_MAJOR 7
#define LIBCURL_VERSION_MINOR 51
#define LIBCURL_VERSION_PATCH 0

#define LIBCURL_VERSION_NUM 0x073300

#define LIBCURL_TIMESTAMP "Wed Nov  2 06:54:46 UTC 2016"

#define CURL_VERSION_BITS(x,y,z) ((x)<<16|(y)<<8|z)
#define CURL_AT_LEAST_VERSION(x,y,z) \
  (LIBCURL_VERSION_NUM >= CURL_VERSION_BITS(x, y, z))

#endif
