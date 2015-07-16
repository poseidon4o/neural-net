#ifndef _GENETIC_H_INCLUDED_
#define _GENETIC_H_INCLUDED_

#include "neuron.hpp"
#include <random>

namespace neuron
{
    std::random_device gen;
    std::mt19937 mt(gen());
    std::uniform_real_distribution<double> dist(0.0, 1.0);

    double rand() {
        return dist(mt);
    }

    //returns random number between 0 to max
    double randMax(int max) {
        return rand() * max;
    }

    bool chance(double percent) {
        return percent < rand();
    }

    template <uint32_t SIZE>
    void random(Net<SIZE> & subject) {
        for (int c = 0; c < SIZE; ++c) {
            for (int r = 0; r < SIZE; ++r) {
                if (subject.hasSynapse(c, r)) {
                    subject[c][r] = randMax(2) - 1.;
                }
            }
        }
    }

    template <uint32_t SIZE>
    Net<SIZE> cross(const Net<SIZE> & mother, const Net<SIZE> & father) {
        const Net<SIZE> * parents[2] = {&mother, &father};
        Net<SIZE> child;

        int inheritence = 1;
        for (int c = 0; c < SIZE; ++c) {
            for (int r = 0; r < SIZE; ++r) {
                if (mother.hasSynapse(c, r)) {
                    // alternate parents
                    child[c][r] = (*parents[inheritence = !inheritence])[c][r];
                }
            }
        }


        return child;
    }

    template <uint32_t SIZE>
    void mutate(Net<SIZE> & subject, double rate) {
        for (int c = 0; c < SIZE; ++c) {
            for (int r = 0; r < SIZE; ++r) {
                if (subject.hasSynapse(c, r) && chance(rate)) {
                    if (chance(0.5)) {
                        subject[c][r] += rand() - .5;
                    } else {
                        subject[c][r] = randMax(2) - 1.;
                    }
                }
            }
        }
    }


    template <uint32_t SIZE>
    void mutationStep(std::vector<std::pair<Net<SIZE>*, double>> & grades) {
        sort(grades.begin(), grades.end(),
            [](const std::pair<Net<SIZE>*, double> & left, const std::pair<Net<SIZE>*, double> & right) {
                return left.second > right.second;
            });

        auto randNet = [&grades]() -> Net<SIZE> & {
            return *grades[randMax(grades.size() - 1)].first;
        };

        const Net<SIZE> & best = *grades.front().first;


        // cross the lower half with low chance of mutation
        for (int c = 0; c < grades.size() / 2; ++c) {
            *grades[c].first = cross(best, randNet());

            if (chance(0.1)) {
                mutate(*grades[c].first, 0.5);
            }
        }

        // mutate 20% of the middle
        for (int c = 0; c < grades.size() / 5; ++c) {
            mutate(*grades[c + grades.size() / 2].first, 0.5);
        }
    }
}


#endif // _GENETIC_H_INCLUDED_
