#include "problems/xor.hpp"

#include <iostream>
int main() {
    XorTest x;
	auto & net = x.run();
	x.test(net);

	std::cout << x.runs;
    return 0;
}
