#include "stdafx.h"
#include "AVAssignment.h"
#include "AVMatrix.h"

namespace rulekit {

	uopref(AVMatrix) AVAssignment::operator<<(uhandle(Value) value) {
		uoprefcall(_matrix)set(_key, value);
		return _matrix;
	}
	uopref(AVMatrix) AVAssignment::operator<<(ucref(string) value) {
		uoprefcall(_matrix)set(_key, unew(String, value));
		return _matrix;
	}

}
