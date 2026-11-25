#pragma once

#include <string>

namespace AverixProtector {

bool Sha256File(const std::string& path, std::string& hexDigest);

bool VerifyFileSha256(const std::string& path, const std::string& expectedHex);

bool ConstantTimeEqual(const std::string& a, const std::string& b);

bool VerifyLicenseHmac(const std::string& token,
                       const std::string& signatureHex,
                       const std::string& key);

bool RunConfiguredChecks();

}
