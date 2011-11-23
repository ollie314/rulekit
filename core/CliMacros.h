//
//  Copyright (c) 2011 Codesign LLC. All rights reserved.
//  License: GPL v3
//

#pragma once

#ifdef _M_CEE_SAFE

#include <cliext/map>
#include <cliext/vector>
namespace stl = cliext;
#define uclass ref class
#define uoverride override
#define unew(t, ...) gcnew t(__VA_ARGS__)
#define string System::String^
#define uref(t) t%
//#define uref2(t1,t2) t1,t2%
#define urefop %
#define ucref(t) t%
#define upureref(t) t
#define upurederef(x) x
#define uhandle(t) t^
#define uhandleref(t) t^
#define uchandleref(t) t^
#define uderef(x) %x
#define uopref(t) t^
#define uopret(t) t^
#define uoprefcall(x) x->
#define uptr(t) t^
#define ugetptr(x) x
#define S(x) gcnew System::String(x)
namespace stx {
	ref class stringstream {
	private:
		string _str;
	public:
		stringstream() : _str("") {}
		stringstream% operator<<(string s) { _str += s; return *this; }
		string str() { return _str; }
	};
}
#define ulog(x) Console::WriteLine(x);
#define udeldef(r, n, ...) delegate r n(__VA_ARGS__);
#define udel(x) x^
#define udelref(x) x^
#define unewdel(d, x, ...) gcnew d(gcnew x(__VA_ARGS__), &x::Run)
#define udelfunc Run

#else

#include <string>
#include <map>
#include <vector>
#include <functional>
#include <memory>
#include <sstream>
#include <iostream>
namespace stl = std;
namespace stx = std;
#define uclass class
#define uoverride
#define unew(t, ...) std::make_shared<t>(__VA_ARGS__)
#define string std::string
#define uref(t) t&
//#define uref2(t1,t2) t1,t2&
#define urefop &
#define ucref(t) const t&
#define upureref(t) t&
#define upurederef(x) *x
#define uhandle(t) std::shared_ptr<t>
#define uhandleref(t) std::shared_ptr<t>&
#define uchandleref(t) const std::shared_ptr<t>&
#define uderef(x) *x
#define uopref(t) t&
#define uopret(t) t
#define ulog(x) std::cout << x << std::endl;
#define uoprefcall(x) x.
#define S(x) x
#define uptr(t) t*
#define ugetptr(x) &*x
#define udeldef(r, n, ...) typedef std::function<r(__VA_ARGS__)> n;
#define udel(x) x
#define udelref(x) x&
#define unewdel(d, x, ...) x(__VA_ARGS__)
#define udelfunc operator()

#endif
