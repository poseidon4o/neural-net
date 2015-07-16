#include "net/neuron.hpp"
#include "net/genetic.hpp"

#include <algorithm>
#include <iostream>

using namespace neuron;
using namespace std;

typedef Net<7> XorNet;

const int GRADE_REPETITIONS = 10;
const double MAX_ERROR = 0.01;

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


vector<pair<XorNet*, double>> grade(vector<XorNet> & nets) {
    vector<pair<XorNet*, double>> results;

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

        results.push_back(make_pair(&net, deviation / maxDeviation));
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

        auto best = max_element(grades.begin(), grades.end(),
            [](const pair<XorNet*, double> & l, const pair<XorNet*, double> & r) {
                return l.second > r.second;
            });

        if (best->second < MAX_ERROR) {
            cout << "Best error: " << best->second << endl;

            test(*best->first);
            return 0;
        }

        mutationStep(grades);
    }

    return 0;
}
