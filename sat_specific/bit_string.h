#pragma once
#include <vector>
#include <ostream>

class BitString
{
private:
    std::vector<std::uint64_t> string;
public:
    BitString() = default;
    BitString(unsigned int max_size);
    BitString(const BitString&) = default;

    unsigned int size() const;
    void set_bit(unsigned int index, bool state);
    bool get_bit(unsigned int index) const;
    void switch_bit(unsigned int index);
    void randomize();

    const std::vector<std::uint64_t>& get_uint64_string() const;
    std::uint64_t get_uint64(unsigned int index) const;

    BitString operator&(const BitString&) const;
    BitString operator|(const BitString&) const;
    BitString operator~() const;
    bool is_null() const;

    static std::ostream& print_uint64(std::ostream&, std::uint64_t);
    friend std::ostream& operator<<(std::ostream&, const BitString&);
};

std::ostream& operator<<(std::ostream&, const BitString&);