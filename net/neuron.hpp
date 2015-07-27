#ifndef _NEURON_H_INCLUDED_
#define _NEURON_H_INCLUDED_

#include <vector>
#include <utility>
#include <array>
#include <limits>
#include <functional>
#include <cmath>
#include <cassert>
#include <ios>

#include <stdint.h>

namespace neuron
{
    struct Neuron {
        double m_FireValue, m_NextFireValue;
    };

    double sigmoid(double u) {
        return 1.0 / (1.0 + exp(-u));
    }

    const double INVALID_SYNAPSE = std::numeric_limits<double>::max();

    template <uint32_t SIZE>
    class Net {
    public:
        enum uint32_t { NeuronCount = SIZE };


        Net() {
            std::fill(m_Neurons.begin(), m_Neurons.end(), Neuron{0, 0});
            for (auto & row : m_SynapseMap) {
                for (auto & cell : row) {
                    cell = INVALID_SYNAPSE;
                }
            }
        }

        class NetProxy {
        public:
            double & operator[](int idx) {
                assert(idx >= 0 && idx < SIZE && "Index out of bounds!");
                return m_Row[idx];
            }
            NetProxy & operator=(const NetProxy &) = delete;
        private:
            friend class Net<SIZE>;

            NetProxy(std::array<double, SIZE> & row): m_Row(row) {}
            std::array<double, SIZE> & m_Row;
        };

        class NetProxyConst {
        public:
            const double & operator[](int idx) const {
                assert(idx >= 0 && idx < SIZE && "Index out of bounds!");
                return m_Row[idx];
            }
            NetProxyConst & operator=(const NetProxyConst &) = delete;
        private:
            friend class Net<SIZE>;

            NetProxyConst(const std::array<double, SIZE> & row): m_Row(row) {}
            const std::array<double, SIZE> & m_Row;
        };

        NetProxy operator[](int idx) {
            assert(idx >= 0 && idx < SIZE && "Index out of bounds!");
            return NetProxy(m_SynapseMap[idx]);
        }

        const NetProxyConst operator[](int idx) const {
            assert(idx >= 0 && idx < SIZE && "Index out of bounds!");
            return NetProxyConst(m_SynapseMap[idx]);
        }

        const std::array<Neuron, SIZE> neurons() const {
            return m_Neurons;
        }

        void stimulate(int idx, double value) {
            m_Neurons[idx].m_FireValue = value;
        }

        double value(int idx) const {
            return m_Neurons[idx].m_FireValue;
        }

        bool hasSynapse(int from, int to) const {
            return m_SynapseMap[from][to] != INVALID_SYNAPSE;
        }

        void step() {
            for(int c = 0; c < SIZE; ++c) {
                for(int r = 0; r < SIZE; ++r) {
                    if (m_SynapseMap[c][r] == INVALID_SYNAPSE) {
                        continue;
                    }

                    m_Neurons[r].m_NextFireValue += m_SynapseMap[c][r] * m_Neurons[c].m_FireValue;
                }
            }

            for (auto & nrn : m_Neurons) {
                nrn.m_FireValue = 2.0 * (sigmoid(nrn.m_NextFireValue) - 0.5);
                nrn.m_NextFireValue = 0.0;
            }
        }

    private:
        std::array<Neuron, SIZE> m_Neurons;

        // m_SynapseMap[x][y] is the edge value from x to y or INVALID_SYNAPSE
        std::array<std::array<double, SIZE>, SIZE> m_SynapseMap;
    };

    template <uint32_t SIZE>
    std::ostream & operator<<(std::ostream & os, Net<SIZE> & net) {
        for (int c = 0; c < SIZE; ++c) {
            for (int r = 0; r < SIZE; ++r) {
                if (net.hasSynapse(c, r)) {
                    os << c << ' ' << r << ' ' << net[c][r] << std::endl;
                }
            }
        }
        return os;
    }

}


#endif // _NEURON_H_INCLUDED_
