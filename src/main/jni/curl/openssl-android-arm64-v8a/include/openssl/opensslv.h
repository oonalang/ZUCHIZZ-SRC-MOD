
#ifndef HEADER_OPENSSLV_H
# define HEADER_OPENSSLV_H

#ifdef  __cplusplus
extern "C" {
#endif

# define OPENSSL_VERSION_NUMBER  0x1010003fL
# ifdef OPENSSL_FIPS
#  define OPENSSL_VERSION_TEXT    "OpenSSL 1.1.0c-fips  10 Nov 2016"
# else
#  define OPENSSL_VERSION_TEXT    "OpenSSL 1.1.0c  10 Nov 2016"
# endif

# define SHLIB_VERSION_HISTORY ""
# define SHLIB_VERSION_NUMBER "1.1"

#ifdef  __cplusplus
}
#endif
#endif
