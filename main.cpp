#include "net/neuron.hpp"
#include "net/genetic.hpp"

#include <algorithm>
#include <iostream>

using namespace neuron;
using namespace std;

typedef Net<7> XorNet;

const int GRADE_REPETITIONS = 4;
const double MAX_ERROR = 0.00001;

static const vector<vector<int>> input = {
    { -1, -1, -1 },
    { -1,  1,  1 },
    {  1, -1,  1 },
    {  1,  1, -1 }
};

void test(XorNet & net) {
    for (int c = 0; c < input.size(); ++c) {
        net.stimulate(0, input[c][0]);
        net.stimulate(1, input[c][1]);

        net.step();

        cout << "INPUT 1: "<< input[c][0] << "\tINPUT 2: " << input[c][1] << "\tEXPECTED: " << input[c][2]
             << "\tOUTPUT: " << net.value(6) << endl;
    }
}


vector<double> grade(vector<XorNet> & nets) {
    vector<double> results;

    auto nextIt = [](decltype(input.begin()) & iter) {
        if (++iter == input.end()) {
            iter = input.begin();
        }
    };

    for (auto & net : nets) {
        double deviation = 0;
        double maxDeviation = input.size() * GRADE_REPETITIONS * 2.;

        auto inputIter = input.begin();

        for (int c = 0; c < input.size() * GRADE_REPETITIONS; nextIt(inputIter), ++c) {
            auto & data = *inputIter;

            net.stimulate(0, data[0]);
            net.stimulate(1, data[1]);

            net.step();

            deviation += fabs((data[2] + 1.) - (net.value(6) + 1.));
        }

        results.push_back(deviation / maxDeviation);
    }

    return results;
}



int main() {


    XorNet base;
    // input 0 - to hidden
    base[0][2] = 0;
    base[0][3] = 0;
    base[0][4] = 0;
    base[0][5] = 0;

    // input 1 - to hidden
    base[1][2] = 0;
    base[1][3] = 0;
    base[1][4] = 0;
    base[1][5] = 0;

    // hidden to output
    base[2][6] = 0;
    base[3][6] = 0;
    base[4][6] = 0;
    base[5][6] = 0;


    vector<XorNet> nets(10, base);
    for_each(nets.begin(), nets.end(), neuron::random<7>);

    while (1) {
        auto grades = grade(nets);
        vector<int> sorted(grades.size());
        int c = 0;
        for (auto & i : sorted) { i = c++; }

        sort(sorted.begin(), sorted.end(), [&grades] (int left, int right) {
            return grades[left] > grades[right];
        });

        for (int c = 0; c < grades.size(); ++c) {
            cout << grades[sorted[c]] <<  endl;
        }

        XorNet & best = nets[sorted.back()];

        if (sorted.back() < MAX_ERROR) {
            test(best);
            return 0;
        }


        auto randNet = [&nets]() -> XorNet & {
            return nets[randMax(nets.size() - 1)];
        };

        for (int c = 0; c < grades.size() / 2; ++c) {
            auto idx = sorted[c];

            nets[idx] = cross(best, randNet());
            if (chance(0.05)) {
                mutate(nets[idx], 0.5);
            }
        }
    }

}
