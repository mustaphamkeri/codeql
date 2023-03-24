#include "swift/extractor/infra/SwiftMangledName.h"

#include <picosha2.h>

namespace codeql {

namespace {
void appendPart(std::string& out, const std::string& prefix) {
  out += prefix;
}

void appendPart(std::string& out, UntypedTrapLabel label) {
  out += '{';
  out += label.str();
  out += '}';
}

void appendPart(std::string& out, unsigned index) {
  out += std::to_string(index);
}

void hashPart(picosha2::hash256_one_by_one& out, const std::string& prefix) {
  out.process(prefix.begin(), prefix.end());
}

void hashPart(picosha2::hash256_one_by_one& out, UntypedTrapLabel label) {
  auto bytes = reinterpret_cast<char*>(&label);
  out.process(bytes, bytes + sizeof(label));
}

void hashPart(picosha2::hash256_one_by_one& out, unsigned index) {
  auto bytes = reinterpret_cast<char*>(&index);
  out.process(bytes, bytes + sizeof(index));
}

}  // namespace

std::string SwiftMangledName::str() const {
  std::string out;
  for (const auto& part : parts) {
    std::visit([&](const auto& contents) { appendPart(out, contents); }, part);
  }
  return out;
}

std::string SwiftMangledName::hash() const {
  auto hasher = picosha2::hash256_one_by_one();
  for (const auto& part : parts) {
    std::visit([&](const auto& contents) { hashPart(hasher, contents); }, part);
  }
  hasher.finish();
  auto ret = get_hash_hex_string(hasher);
  ret.resize(ret.size() / 2);
  return ret;
}

SwiftMangledName& SwiftMangledName::operator<<(SwiftMangledName::Part&& part) & {
  parts.emplace_back(std::move(part));
  return *this;
}

SwiftMangledName& SwiftMangledName::operator<<(UntypedTrapLabel label) & {
  assert(label && "using undefined label in mangled name");
  parts.emplace_back(label);
  return *this;
}

SwiftMangledName& SwiftMangledName::operator<<(unsigned int i) & {
  parts.emplace_back(i);
  return *this;
}

SwiftMangledName& SwiftMangledName::operator<<(SwiftMangledName&& other) & {
  parts.reserve(parts.size() + other.parts.size());
  for (auto& p : other.parts) {
    parts.emplace_back(std::move(p));
  }
  other.parts.clear();
  return *this;
}

SwiftMangledName& SwiftMangledName::operator<<(const SwiftMangledName& other) & {
  parts.reserve(parts.size() + other.parts.size());
  parts.insert(parts.end(), other.parts.begin(), other.parts.end());
  return *this;
}

}  // namespace codeql
