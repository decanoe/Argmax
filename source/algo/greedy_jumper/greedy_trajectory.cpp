#include "greedy_trajectory.h"
#include <iostream>
#include <algorithm>
#include <random>

using namespace LocalSearch;

BitFlip::BitFlip(unsigned int index, float score, std::shared_ptr<std::mt19937> random_generator): index(index), score(score), tie_breaker(0) {
    std::uniform_real_distribution<float> dist(0, 1);
    this->tie_breaker = dist(*random_generator);
}
BitFlip::BitFlip(): index(0), score(0), tie_breaker(0) {}
std::ostream& BitFlip::cout(std::ostream& c) const {
    return c << "(" << index << ", " << score << ")";
}
std::ostream& LocalSearch::operator<<(std::ostream& c, const BitFlip& b) { return b.cout(c); }

class AscendantHalfTrajectory: public GreedyHalfTrajectory
{
    std::set<BitFlip, std::function<bool(BitFlip, BitFlip)>> bit_flips_set;
public:
    static bool cmp(BitFlip a, BitFlip b) {
        if (a.score != b.score) return a.score < b.score;
        if (a.tie_breaker != b.tie_breaker) return a.tie_breaker < b.tie_breaker;
        else return a.index < b.index;
    };

    AscendantHalfTrajectory(): GreedyHalfTrajectory(), bit_flips_set(cmp) {}

    GreedyHalfTrajectory* insert_flip(unsigned int index, float score) override {
        this->ensure_insert_flip();
        this->bit_flips_set.insert(BitFlip(index, score, random_generator));
        return this;
    }
    unsigned int finalize(unsigned int max_count = -1U) override {
        GreedyHalfTrajectory::finalize(max_count);

        if (this->bit_flips_set.size() < max_count)
            this->bit_flips.assign(this->bit_flips_set.begin(), this->bit_flips_set.end());
        else
            this->bit_flips.assign(this->bit_flips_set.begin(), std::next(this->bit_flips_set.begin(), max_count));

        return this->bit_flips.size();
    }
    void clear() override {
        this->bit_flips_set.clear();
        GreedyHalfTrajectory::clear();
    }
    unsigned int size() const override {
        if (finalized) return GreedyHalfTrajectory::size();
        return bit_flips_set.size();
    }

    std::ostream& cout(std::ostream& c) const override {
        if (finalized) return GreedyHalfTrajectory::cout(c);

        if (bit_flips_set.size() != 0) {
            for (const BitFlip& bitflip : bit_flips_set) c << bitflip << ", ";
            return c << "\b\b";
        }
        return c;
    }
};
class DescendantHalfTrajectory: public GreedyHalfTrajectory
{
    std::set<BitFlip, std::function<bool(BitFlip, BitFlip)>> bit_flips_set;
public:
    static bool cmp(BitFlip a, BitFlip b) {
        if (a.score != b.score) return a.score > b.score;
        if (a.tie_breaker != b.tie_breaker) return a.tie_breaker < b.tie_breaker;
        else return a.index < b.index;
    };

    DescendantHalfTrajectory(): GreedyHalfTrajectory(), bit_flips_set(cmp) {}

    GreedyHalfTrajectory* insert_flip(unsigned int index, float score) override {
        this->ensure_insert_flip();
        this->bit_flips_set.insert(BitFlip(index, score, random_generator));
        return this;
    }
    unsigned int finalize(unsigned int max_count) override {
        GreedyHalfTrajectory::finalize(max_count);
        
        if (this->bit_flips_set.size() < max_count)
            this->bit_flips.assign(this->bit_flips_set.begin(), this->bit_flips_set.end());
        else
            this->bit_flips.assign(this->bit_flips_set.begin(), std::next(this->bit_flips_set.begin(), max_count));

        return this->bit_flips.size();
    }
    void clear() override {
        this->bit_flips_set.clear();
        GreedyHalfTrajectory::clear();
    }
    unsigned int size() const override {
        if (finalized) return GreedyHalfTrajectory::size();
        return bit_flips_set.size();
    }

    std::ostream& cout(std::ostream& c) const override {
        if (finalized) return GreedyHalfTrajectory::cout(c);

        if (bit_flips_set.size() != 0) {
            for (const BitFlip& bitflip : bit_flips_set) c << bitflip << ", ";
            return c << "\b\b";
        }
        return c;
    }
};
class RandomHalfTrajectory: public GreedyHalfTrajectory
{
public:
    RandomHalfTrajectory(): GreedyHalfTrajectory() {}
    
    GreedyHalfTrajectory* insert_flip(unsigned int index, float score) override {
        this->ensure_insert_flip();
        this->bit_flips.push_back(BitFlip(index, score, random_generator));
        return this;
    }
    unsigned int finalize(unsigned int max_count) override {
        GreedyHalfTrajectory::finalize();

        if (this->bit_flips.size() > max_count) this->bit_flips.resize(max_count);
        std::shuffle(this->bit_flips.begin(), this->bit_flips.end(), *this->random_generator);

        return this->bit_flips.size();
    }
};

GreedyHalfTrajectory::Iterator GreedyHalfTrajectory::begin() const {
    return this->bit_flips.begin();
}
GreedyHalfTrajectory::Iterator GreedyHalfTrajectory::end() const {
    return this->bit_flips.end();
}
GreedyHalfTrajectory::ReverseIterator GreedyHalfTrajectory::rbegin() const {
    return this->bit_flips.rbegin();
}
GreedyHalfTrajectory::ReverseIterator GreedyHalfTrajectory::rend() const {
    return this->bit_flips.rend();
}
GreedyHalfTrajectory* GreedyHalfTrajectory::set_seed(std::shared_ptr<std::mt19937> random_generator) { this->random_generator = random_generator; return this; }
void GreedyHalfTrajectory::ensure_insert_flip() {
    if (this->finalized) {
        std::cerr << "\033[1;31mCan't insert flip in a finalized GreedyHalfTrajectory\n\033[0m";
        exit(1);
    }
}
unsigned int GreedyHalfTrajectory::finalize(unsigned int max_count) {
    if (this->finalized) {
        std::cerr << "\033[1;31mCan't finalize a finalized GreedyHalfTrajectory\n\033[0m";
        exit(1);
    }
    this->finalized = true;
    return this->bit_flips.size();
}
void GreedyHalfTrajectory::clear() {
    this->finalized = false;
    this->bit_flips.clear();
}
unsigned int GreedyHalfTrajectory::size() const { return bit_flips.size(); }
GreedyHalfTrajectory::GreedyHalfTrajectory(): bit_flips(), finalized(false) {}
std::ostream& GreedyHalfTrajectory::cout(std::ostream& c) const {
    if (bit_flips.size() != 0) {
        for (const BitFlip& bitflip : bit_flips) c << bitflip << ", ";
        return c << "\b\b";
    }
    return c;
}
std::ostream& LocalSearch::operator<<(std::ostream& c, const GreedyHalfTrajectory& t) { return t.cout(c); }


GreedyTrajectory::GreedyTrajectory(NeighborhoodOrdering positive_ordering, NeighborhoodOrdering negative_ordering) {
    this->positive_bits = constructGreedyHalfTrajectory(positive_ordering);
    this->negative_bits = constructGreedyHalfTrajectory(negative_ordering);
}
std::shared_ptr<GreedyHalfTrajectory> GreedyTrajectory::constructGreedyHalfTrajectory(NeighborhoodOrdering ordering) {
    switch (ordering)
    {
    case NeighborhoodOrdering::Asc: return std::make_shared<AscendantHalfTrajectory>();
    case NeighborhoodOrdering::Desc: return std::make_shared<DescendantHalfTrajectory>();
    case NeighborhoodOrdering::Rand: return std::make_shared<RandomHalfTrajectory>();
    }
    
    std::cerr << "\033[1;31mUnknown NeighborhoodOrdering enum\n\033[0m";
    exit(1);
}
GreedyTrajectory::NeighborhoodOrdering GreedyTrajectory::ordering_from_string(const std::string& parameter) {
    if (parameter == "Asc") return NeighborhoodOrdering::Asc;
    if (parameter == "Desc") return NeighborhoodOrdering::Desc;
    if (parameter == "Rand") return NeighborhoodOrdering::Rand;

    std::cerr << "\033[1;31mUnknown NeighborhoodOrdering key \"" << parameter << "\"\n\033[0m";
    exit(1);
}

GreedyTrajectory* GreedyTrajectory::set_seed(std::shared_ptr<std::mt19937> random_generator) {
    this->positive_bits->set_seed(random_generator);
    this->negative_bits->set_seed(random_generator);
    return this;
}

GreedyTrajectory* GreedyTrajectory::insert_positive_flip(unsigned int index, float score) {
    this->positive_bits->insert_flip(index, score);
    return this;
}
GreedyTrajectory* GreedyTrajectory::insert_negative_flip(unsigned int index, float score) {
    this->negative_bits->insert_flip(index, score);
    return this;
}
unsigned int GreedyTrajectory::finalize(unsigned int max_count) {
    unsigned int size = this->positive_bits->finalize(max_count);
    if (max_count < size) max_count = size;
    size += this->negative_bits->finalize(max_count - size);
    return size;
}
void GreedyTrajectory::clear() {
    this->positive_bits->clear();
    this->negative_bits->clear();
}
unsigned int GreedyTrajectory::positive_size() const { return positive_bits->size(); }
unsigned int GreedyTrajectory::size() const { return positive_bits->size() + negative_bits->size(); }

ConcatIterator<GreedyHalfTrajectory::Iterator> GreedyTrajectory::begin() const {
    return ConcatIterator<GreedyHalfTrajectory::Iterator>(
        positive_bits->begin(), positive_bits->end(),
        negative_bits->begin()
    );
}
ConcatIterator<GreedyHalfTrajectory::Iterator> GreedyTrajectory::end() const {
    return ConcatIterator<GreedyHalfTrajectory::Iterator>(
        positive_bits->end(), positive_bits->end(),
        negative_bits->end(),
        true
    );
}
ConcatIterator<GreedyHalfTrajectory::ReverseIterator> GreedyTrajectory::rbegin() const {
    return ConcatIterator<GreedyHalfTrajectory::ReverseIterator>(
        negative_bits->rbegin(), negative_bits->rend(),
        positive_bits->rbegin()
    );
}
ConcatIterator<GreedyHalfTrajectory::ReverseIterator> GreedyTrajectory::rend() const {
    return ConcatIterator<GreedyHalfTrajectory::ReverseIterator>(
        negative_bits->rend(), negative_bits->rend(),
        positive_bits->rend(),
        true
    );
}

std::ostream& GreedyTrajectory::cout(std::ostream& c) const {
    return c << "{\033[32m" << *positive_bits <<  "\033[0m|\033[31m" << *negative_bits << "\033[0m}";
}
std::ostream& LocalSearch::operator<<(std::ostream& c, const GreedyTrajectory& t) { return t.cout(c); }
