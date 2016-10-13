#pragma once

namespace oak {

	inline size_t operator"" _kb(long long unsigned int kilos) {
		return kilos * 1000;
	}

	inline size_t operator"" _mb(long long unsigned int megs) {
		return megs * 1000000;
	}

	inline size_t operator"" _gb(long long unsigned int gigs) {
		return gigs * 1000000000;
	}

}