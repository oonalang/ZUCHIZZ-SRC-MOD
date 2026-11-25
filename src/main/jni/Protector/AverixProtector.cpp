#include "AverixProtector.h"
#include "ProtectorConfig.h"

#include <openssl/sha.h>
#include <openssl/hmac.h>
#include <openssl/crypto.h>

#include <cstdio>
#include <cstring>
#include <string>

namespace AverixProtector {
namespace {
static std::string ToHex(const unsigned char* data, unsigned int size) {
    static const char digits[] = "0123456789abcdef";
    std::string out;
    out.resize(size * 2);
    for (unsigned int i = 0; i < size; ++i) {
        out[i * 2] = digits[(data[i] >> 4) & 0x0f];
        out[i * 2 + 1] = digits[data[i] & 0x0f];
    }
    return out;
}

static bool IsHexDigest(const std::string& value) {
    if (value.size() != 64) return false;
    for (char c : value) {
        if (!((c >= '0' && c <= '9') || (c >= 'a' && c <= 'f') ||
              (c >= 'A' && c <= 'F'))) return false;
    }
    return true;
}
}

bool Sha256File(const std::string& path, std::string& hexDigest) {
    hexDigest.clear();
    if (path.empty()) return false;
    FILE* file = fopen(path.c_str(), "rb");
    if (!file) return false;

    SHA256_CTX ctx;
    if (SHA256_Init(&ctx) != 1) { fclose(file); return false; }
    unsigned char buffer[8192];
    size_t count = 0;
    bool ok = true;
    while ((count = fread(buffer, 1, sizeof(buffer), file)) > 0) {
        if (SHA256_Update(&ctx, buffer, count) != 1) { ok = false; break; }
    }
    if (ferror(file)) ok = false;
    fclose(file);
    if (!ok) return false;

    unsigned char digest[SHA256_DIGEST_LENGTH];
    if (SHA256_Final(digest, &ctx) != 1) return false;
    hexDigest = ToHex(digest, SHA256_DIGEST_LENGTH);
    OPENSSL_cleanse(digest, sizeof(digest));
    return true;
}

bool ConstantTimeEqual(const std::string& a, const std::string& b) {
    if (a.size() != b.size()) return false;
    if (a.empty()) return true;
    return CRYPTO_memcmp(a.data(), b.data(), a.size()) == 0;
}

bool VerifyFileSha256(const std::string& path, const std::string& expectedHex) {
    if (!IsHexDigest(expectedHex)) return false;
    std::string actual;
    if (!Sha256File(path, actual)) return false;
    return ConstantTimeEqual(actual, expectedHex);
}

bool VerifyLicenseHmac(const std::string& token,
                       const std::string& signatureHex,
                       const std::string& key) {
    if (token.empty() || key.empty() || !IsHexDigest(signatureHex)) return false;
    unsigned char digest[EVP_MAX_MD_SIZE];
    unsigned int digestLength = 0;
    if (!HMAC(EVP_sha256(), key.data(), static_cast<int>(key.size()),
              reinterpret_cast<const unsigned char*>(token.data()), token.size(),
              digest, &digestLength)) return false;
    const std::string actual = ToHex(digest, digestLength);
    OPENSSL_cleanse(digest, sizeof(digest));
    return ConstantTimeEqual(actual, signatureHex);
}

bool RunConfiguredChecks() {
    const std::string path = AverixProtectorConfig::kProtectedFilePath;
    const std::string expected = AverixProtectorConfig::kExpectedFileSha256;
    if (path.empty() && expected.empty()) return true;
    if (path.empty() || expected.empty()) return false;
    return VerifyFileSha256(path, expected);
}

}
