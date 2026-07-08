#pragma once
#include <functional>
#include <set>
#include <memory>
#include <random>
#include <variant>
#include <iterator>

#include "../../utils/concat_iterator.h"

namespace LocalSearch {
    class BitFlip {
        public:
        unsigned int index;
        float score;
        float tie_breaker;

        BitFlip(unsigned int index, float score, std::shared_ptr<std::mt19937> random_generator);
        BitFlip();

        std::ostream& cout(std::ostream&) const;
    };
    std::ostream& operator<<(std::ostream&, const BitFlip&);
    
    class GreedyHalfTrajectory {
    protected:
        std::vector<BitFlip> bit_flips;
        bool finalized;
        std::shared_ptr<std::mt19937> random_generator;
    public:
        GreedyHalfTrajectory();

        void ensure_insert_flip();
        virtual GreedyHalfTrajectory* insert_flip(unsigned int index, float score) = 0;

        virtual GreedyHalfTrajectory* set_seed(std::shared_ptr<std::mt19937> random_generator);

        virtual unsigned int finalize(unsigned int max_count = -1U);
        virtual void clear();
        virtual unsigned int size() const;

        typedef std::vector<BitFlip>::const_iterator Iterator;
        typedef std::vector<BitFlip>::const_reverse_iterator ReverseIterator;
        Iterator begin() const;
        Iterator end() const;
        ReverseIterator rbegin() const;
        ReverseIterator rend() const;

        virtual std::ostream& cout(std::ostream&) const;
    };
    std::ostream& operator<<(std::ostream&, const GreedyHalfTrajectory&);

    class GreedyTrajectory {
    public:
        enum class NeighborhoodOrdering { Desc, Asc, Rand };
        static NeighborhoodOrdering ordering_from_string(const std::string& parameter);

        GreedyTrajectory(NeighborhoodOrdering positive_ordering, NeighborhoodOrdering negative_ordering);

        GreedyTrajectory* set_seed(std::shared_ptr<std::mt19937> random_generator);

        GreedyTrajectory* insert_positive_flip(unsigned int index, float score);
        GreedyTrajectory* insert_negative_flip(unsigned int index, float score);

        unsigned int finalize(unsigned int max_count = -1U);
        void clear();
        unsigned int positive_size() const;
        unsigned int size() const;

        ConcatIterator<GreedyHalfTrajectory::Iterator> begin() const;
        ConcatIterator<GreedyHalfTrajectory::Iterator> end() const;
        ConcatIterator<GreedyHalfTrajectory::ReverseIterator> rbegin() const;
        ConcatIterator<GreedyHalfTrajectory::ReverseIterator> rend() const;

        std::ostream& cout(std::ostream&) const;
    protected:
        std::shared_ptr<GreedyHalfTrajectory> positive_bits, negative_bits;

        static std::shared_ptr<GreedyHalfTrajectory> constructGreedyHalfTrajectory(NeighborhoodOrdering ordering);
    };
    std::ostream& operator<<(std::ostream&, const GreedyTrajectory&);
}