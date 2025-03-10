#include "bit_string.h"

BitString::BitString(unsigned int max_size): string((max_size >> 6) + (max_size % (1<<6) != 0), 0) {}
BitString::BitString(const BitString& b): string(b.string) {}

unsigned int BitString::size() const {
    return string.size() << 6;
}
void BitString::set_bit(unsigned int index, bool state) {
    while ((index >> 6) > string.size()) string.push_back(0);

    if (state)  string[index >> 6] |=   1ULL << (index % (1 << 6));
    else        string[index >> 6] &= ~(1ULL << (index % (1 << 6)));
}
bool BitString::get_bit(unsigned int index) const {
    if ((index >> 6) > string.size()) return false;
    return (string[index >> 6] & (1ULL << (index % (1 << 6)))) != 0;
}
void BitString::switch_bit(unsigned int index) {
    while ((index >> 6) > string.size()) string.push_back(0);
    string[index >> 6] ^= (1ULL << (index % (1 << 6)));
}
void BitString::randomize() {
    for (unsigned int i = 0; i < string.size(); i++)
        string[i] = std::rand();
}

const std::vector<std::uint64_t>& BitString::get_uint64_string() const { return string; }
std::uint64_t BitString::get_uint64(unsigned int index) const { return string[index]; }

BitString BitString::operator&(const BitString& other) const {
    BitString result(std::min(size(), other.size()));
    for (size_t i = 0; i < result.string.size(); i++) result.string[i] = other.string[i] & string[i];
    return result;
}
BitString BitString::operator|(const BitString& other) const {
    BitString result(std::min(size(), other.size()));
    for (size_t i = 0; i < result.string.size(); i++) result.string[i] = other.string[i] | string[i];
    return result;
}
BitString BitString::operator~() const {
    BitString result(size());
    for (size_t i = 0; i < result.string.size(); i++) result.string[i] = ~(string[i]);
    return result;
}
bool BitString::is_null() const {
    for (size_t i = 0; i < string.size(); i++) if (string[i] != 0) return false;
    return true;
}

std::ostream& BitString::print_uint64(std::ostream& c, std::uint64_t value) {
    for (unsigned int i = 0; i < 1<<6; i++)
        if ((value & (1ULL << i)) == 0) c << "0";
        else c << "\033[1;34m1\033[0m";
    return c;
}
std::ostream& operator<<(std::ostream& c, const BitString& s) {
    for (auto sub_s : s.string)
        BitString::print_uint64(c, sub_s);
    return c;
}