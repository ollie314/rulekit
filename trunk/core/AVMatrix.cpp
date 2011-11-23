#include "stdafx.h"
#include "AVMatrix.h"

namespace rulekit {

	void Value::unbind(uref(stl::vector<uptr(Variable)>) vars) {
		for (auto it = vars.begin(); it != vars.end(); ++it) (*it)->unbind();
	}

	uopret(AVAssignment) AVMatrix::operator<<(ucref(string) key) {
#ifdef _M_CEE_SAFE
		return unew(AVAssignment, key, this);
#else
		return AVAssignment(key, *this);
#endif
	}

	/*uhandle(AVMatrix) AVMatrix::clone(uref2(stl::map<string,uptr(rulekit::Variable)>) vars) {
		auto result = unew(AVMatrix);
		
		return result;
	}*/

}
