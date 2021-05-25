
#define __STDC_FORMAT_MACROS
#include <jp/def.h>
#include <jp/visx.hpp>
#include <chrono>
#include <stdio.h>
#include <iostream>

using std::chrono::high_resolution_clock;
using std::chrono::duration;
using std::chrono::duration_cast;
using namespace jp::visx::uasf;

uint test_UncertaintyTableElement(void);
uint test_UncertaintyTable(void);

static FPTR_VARR(uint, tests, void) = {test_UncertaintyTableElement, test_UncertaintyTable};

int main(void) {
	for (uint64 test_num = 0; test_num < NELS(tests); ++test_num) {
		high_resolution_clock::time_point time_start = high_resolution_clock::now();
		uint result = 0, iterations = 1;
		result = tests[test_num]();
		high_resolution_clock::time_point time_end = high_resolution_clock::now();
		duration<double> time_span = duration_cast<duration<double>>(time_end - time_start);
		printf("Did %u iterations of test %" PRIu64 " in %g seconds. Test %s with value %u.\n", iterations, test_num, time_span.count(), result ? "failed" : "succeeded", result);
	}
	printf("Did all tests.\n");
	return 0;
}

uint test_UncertaintyTableElement(void) {
	UncertaintyTableElement element(UOPERATION_NUL, 3.2, 0.3, 1.5, 0.1);
	UncertaintyPair pair;
	static UncertaintyPair *pair_ptr = &pair;
	for (UncertaintyTableElementType op = UOPERATION_NUL; op <= UOPERATION_INVALID; op = (UncertaintyTableElementType)(((int)op) + 1)) {
		element.setType(op);
		element.compute(pair_ptr);
		printf("OP %d: %g pm %g\n", op, pair.value, pair.uncertainty);
	}
	return 0;
}

uint test_UncertaintyTable(void) {
#define PV() printf("Result at " JP_FILEPOS ": %g pm %g\n", table.getResult(), table.getResultingUncertainty())
	UncertaintyTable table{};
	PV();
	int x;
	double v, u;
	while (std::cin >> x >> v >> u && x < (int)UOPERATION_INVALID && x > -3) {
		if (x == -1) table.remove(table.count() - 1);
		else if (x == -2) table.clear();
		else table.add((UncertaintyTableElementType)x, v, u);
		PV();
	}
	return 0;
}
