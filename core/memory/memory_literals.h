#pragma once

namespace oak {

	inline size_t operator"" _kb(long long unsigned int kilos) {
		return kilos * 1e3;
	}

	inline size_t operator"" _mb(long long unsigned int megs) {
		return megs * 1e6;
	}

	inline size_t operator"" _gb(long long unsigned int gigs) {
		return gigs * 1e9;
	}

	inline size_t operator"" _tb(long long unsigned int tbs) {
		return tbs * 1e12;
	}

}