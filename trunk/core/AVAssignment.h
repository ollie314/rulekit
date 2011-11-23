//
//  Copyright (c) 2011 Codesign LLC. All rights reserved.
//  License: GPL v3
//

#pragma once

#include "CliMacros.h"

namespace rulekit {

	uclass AVMatrix;
	uclass Value;

	uclass AVAssignment {
	private:
		string _key;
		uopref(AVMatrix) _matrix;
	public:
		AVAssignment(ucref(string) k, uopref(AVMatrix) m) : _key(k), _matrix(m) {}
		uopref(AVMatrix) operator<<(uhandle(Value) value);
		uopref(AVMatrix) operator<<(ucref(string) value);
	};

}
