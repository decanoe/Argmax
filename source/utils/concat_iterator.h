#pragma once

#include <iterator>

template <typename It>
class ConcatIterator {
public:
    using value_type = typename std::iterator_traits<It>::value_type;
    using reference = typename std::iterator_traits<It>::reference;
    using pointer = typename std::iterator_traits<It>::pointer;
    using difference_type = typename std::iterator_traits<It>::difference_type;
    using iterator_category = std::input_iterator_tag;

    ConcatIterator(It begin1, It end1, It begin2, bool is_end = false)
        : it1_(begin1),
          end1_(end1),
          it2_(begin2),
          in_first_(!is_end)
    {
        if (it1_ == end1_) {
            in_first_ = false;
        }
    }

    reference operator*() const
    {
        return in_first_ ? *it1_ : *it2_;
    }

    ConcatIterator& operator++()
    {
        if (in_first_) {
            ++it1_;
            if (it1_ == end1_) {
                in_first_ = false;
            }
        } else {
            ++it2_;
        }

        return *this;
    }

    bool operator==(const ConcatIterator& other) const
    {
        if (in_first_ != other.in_first_) {
            return false;
        }

        return in_first_
            ? it1_ == other.it1_
            : it2_ == other.it2_;
    }

    bool operator!=(const ConcatIterator& other) const
    {
        return !(*this == other);
    }

private:
    It it1_;
    It end1_;
    It it2_;

    bool in_first_;
};