#include "bit_string.h"

BitString::BitString(unsigned int print_size, unsigned int start_size): string((start_size >> 6) + (start_size % (1<<6) != 0), 0), print_size(print_size) {}
BitString::BitString(const BitString& b): string(b.string), print_size(b.print_size) {}

unsigned int BitString::size() const {
    return string.size() << 6;
}
void BitString::set_bit(unsigned int index, bool state) {
    if (index > print_size) print_size = index;
    while ((index >> 6) >= string.size()) string.push_back(0);

    if (state)  string[index >> 6] |=   1ULL << (index % (1 << 6));
    else        string[index >> 6] &= ~(1ULL << (index % (1 << 6)));
}
bool BitString::get_bit(unsigned int index) const {
    if ((index >> 6) >= string.size()) return false;
    return (string[index >> 6] & (1ULL << (index % (1 << 6)))) != 0;
}
void BitString::switch_bit(unsigned int index) {
    if (index > print_size) print_size = index;
    while ((index >> 6) >= string.size()) string.push_back(0);
    string[index >> 6] ^= (1ULL << (index % (1 << 6)));
}
std::uint64_t rand64(void)
{
    return rand() ^
        ((std::uint64_t)rand() << 15) ^
        ((std::uint64_t)rand() << 30) ^
        ((std::uint64_t)rand() << 45) ^
        ((std::uint64_t)rand() << 60);
}
BitString& BitString::randomize() {
    for (unsigned int i = 0; i < string.size(); i++)
        string[i] = rand64();
    return *this;
}
BitString BitString::randomize_clone() const {
    BitString result = BitString(*this);
    for (unsigned int i = 0; i < result.string.size(); i++)
        result.string[i] = rand64();
    return result;
}

const std::vector<std::uint64_t>& BitString::get_uint64_string() const { return string; }
std::uint64_t BitString::get_uint64(unsigned int index) const { if (index >= string.size()) return 0; else return string[index]; }

BitString BitString::operator&(const BitString& other) const {
    BitString result(std::min(print_size, other.print_size), std::min(size(), other.size()));
    for (size_t i = 0; i < result.string.size(); i++) result.string[i] = other.string[i] & string[i];
    return result;
}
BitString BitString::operator|(const BitString& other) const {
    BitString result(std::min(print_size, other.print_size), std::max(size(), other.size()));
    for (size_t i = 0; i < result.string.size(); i++) result.string[i] = other.get_uint64(i) | get_uint64(i);
    return result;
}
BitString BitString::operator^(const BitString& other) const {
    BitString result(std::min(print_size, other.print_size), std::max(size(), other.size()));
    for (size_t i = 0; i < result.string.size(); i++) result.string[i] = other.get_uint64(i) ^ get_uint64(i);
    return result;
}
BitString BitString::operator~() const {
    BitString result(print_size, size());
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
    c << "[" << s.print_size << "]";
    for (unsigned int i = 0; i < std::max(1U, std::min(s.print_size, s.size())); i++)
        if (s.get_bit(i)) c << "\033[1;34m1\033[0m";
        else c << "0";
    return c;
}