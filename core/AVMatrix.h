//
//  Copyright (c) 2011 Codesign LLC. All rights reserved.
//  License: GPL v3
//

#pragma once

#include "CliMacros.h"
#include "AVAssignment.h"
#include "Utils.h"

namespace rulekit {

	uclass String;
	uclass Variable;
	uclass AVMatrix;

	uclass Value {
	protected:
	public:
		virtual uptr(String) asString() { return nullptr; }
		virtual uptr(AVMatrix) asMatrix() { return nullptr; }
		virtual uptr(Variable) asVariable() { return nullptr; }
		virtual bool isNil() { return true; }
		virtual uhandle(Value) unify(uptr(Value) value, uref(stl::vector<uptr(Variable)>)) { return unew(Value); }
		virtual bool unifiable(uptr(Value) value, uref(stl::vector<uptr(Variable)>)) { return false; }
		virtual string description() { return "NIL"; }
		static void unbind(uref(stl::vector<uptr(Variable)>) vars);
	};

	uclass Variable : public Value {
	private:
		string _name;
		uptr(Value) _value;
		uptr(Value) finalValue() {
			if (_value == nullptr) return this;
			uptr(Variable) var = _value->asVariable();
			return var != nullptr ? var->finalValue() : _value;
		}
		void bind(uptr(Value) v) { _value = v; }
	public:
		Variable(ucref(string) n) : _name(n), _value(nullptr) {}
		Variable(ucref(string) n, uptr(Value) v) : _name(n), _value(v) {}
		void unbind() { _value = nullptr; }
		string name() { return _name; }
		uptr(Value) value() { return finalValue(); }
		virtual uptr(String) asString() uoverride { return nullptr; }
		virtual uptr(AVMatrix) asMatrix() uoverride { return nullptr; }
		virtual uptr(Variable) asVariable() uoverride { return this; }
		virtual bool isNil() uoverride { return false; }
		virtual uhandle(Value) unify(uptr(Value) value, uref(stl::vector<uptr(Variable)>) vars) uoverride {
			uptr(Value) _finalValue = finalValue();
			uptr(Variable) var1 = _finalValue->asVariable();
			uptr(Variable) var2 = value->asVariable();
			if (var1 == nullptr && var2 != nullptr) return var2->unify(_finalValue, vars);
			if (var1 == nullptr && var2 == nullptr) return _finalValue->unify(value, vars);
			var1->bind(value);
			vars.push_back(var1);
			return unew(Variable, _name, value);
		}
		virtual bool unifiable(uptr(Value) value, uref(stl::vector<uptr(Variable)>) vars) uoverride {
			uptr(Value) _finalValue = finalValue();
			uptr(Variable) var1 = _finalValue->asVariable();
			uptr(Variable) var2 = value->asVariable();
			if (var1 == nullptr && var2 != nullptr) return var2->unifiable(_finalValue, vars);
			if (var1 == nullptr && var2 == nullptr) return _finalValue->unifiable(value, vars);
			var1->bind(value);
			vars.push_back(var1);
			return true;
		}
		virtual string description() uoverride {
			stx::stringstream ss;
			ss << _name << "=" << (_value == nullptr ? "?" : _value->description());
			return ss.str();
		}
	};
	
	uclass String : public Value {
	private:
		string _str;
	public:
		String(ucref(string) s) : _str(s) {}
		virtual uptr(String) asString() uoverride { return this; }
		virtual uptr(AVMatrix) asMatrix() uoverride { return nullptr; }
		virtual uptr(Variable) asVariable() uoverride { return nullptr; }
		virtual bool isNil() uoverride { return false; }
		virtual uhandle(Value) unify(uptr(Value) value, uref(stl::vector<uptr(Variable)>) vars) uoverride {
			uptr(Variable) var = value->asVariable();
			if (var != nullptr) return var->unify(this, vars);
			uptr(String) str = value->asString();
			return str != nullptr && str->_str == _str ? unew(String, _str) : unew(Value);
		}
		virtual bool unifiable(uptr(Value) value, uref(stl::vector<uptr(Variable)>) vars) uoverride {
			uptr(Variable) var = value->asVariable();
			if (var != nullptr) return var->unifiable(this, vars);
			uptr(String) str = value->asString();
			return str != nullptr && str->_str == _str;
		}
		virtual string description() uoverride { return _str; }
	};

	uclass VarMap {
	private:
		stl::map<string,uhandle(Variable)> _vars;
	public:
		void put(ucref(string) key, uhandle(Variable) val) { _vars[key] = val; }
		bool hasKey(ucref(string) key) { return _vars.find(key) != _vars.end(); }
		uhandle(Variable) get(ucref(string) key) { return _vars[key]; }
	};

	uclass AVAssignment;

	uclass AVMatrix : public Value {
	private:
		stl::map<string,uhandle(Value)> _avm;
	public:
		uopret(AVAssignment) operator<<(ucref(string) key);
		void set(ucref(string) key, uchandleref(Value) value) {
			_avm[key] = value;
		}
		virtual uptr(String) asString() uoverride { return nullptr; }
		virtual uptr(AVMatrix) asMatrix() uoverride { return this; }
		virtual uptr(Variable) asVariable() uoverride { return nullptr; }
		virtual bool isNil() uoverride { return false; }
		virtual uhandle(Value) unify(uptr(Value) value, uref(stl::vector<uptr(Variable)>) vars) uoverride {
			uptr(Variable) var = value->asVariable();
			if (var != nullptr) return var->unify(this, vars);
			uptr(AVMatrix) matrix = value->asMatrix();
			if (matrix != nullptr) {
				stl::vector<uptr(Variable)> _vars;
				auto result = unew(AVMatrix);
				auto it1 = _avm.begin();
				auto it2 = matrix->_avm.begin();
				while (it1 != _avm.end() && it2 != matrix->_avm.end()) {
					if (it1->first == it2->first) {
						auto subresult = it1->second->unify(ugetptr(it2->second), _vars);
						if (subresult->isNil()) { unbind(_vars); return unew(Value); }
						result->set(it1->first, subresult);
						++it1; ++it2;
					} else if (it1->first < it2->first) while (it1 != _avm.end() && it1->first < it2->first) {
						result->set(it1->first, it1->second);
						++it1;
					} else while (it2 != matrix->_avm.end() && it2->first < it1->first) {
						result->set(it2->first, it2->second);
						++it2;
					}
				}
				while (it1 != _avm.end()) {
					result->set(it1->first, it1->second);
					++it1;
				}
				while (it2 != matrix->_avm.end()) {
					result->set(it2->first, it2->second);
					++it2;
				}
				vars.insert(vars.end(), _vars.begin(), _vars.end());
				return result;
			}
			return unew(Value);
		}
		virtual bool unifiable(uptr(Value) value, uref(stl::vector<uptr(Variable)>) vars) uoverride {
			uptr(Variable) var = value->asVariable();
			if (var != nullptr) return var->unifiable(this, vars);
			uptr(AVMatrix) matrix = value->asMatrix();
			if (matrix != nullptr) {
				stl::vector<uptr(Variable)> _vars;
				auto it1 = _avm.begin();
				auto it2 = matrix->_avm.begin();
				while (it1 != _avm.end() && it2 != matrix->_avm.end()) {
					if (it1->first == it2->first) {
						auto subresult = it1->second->unifiable(ugetptr(it2->second), _vars);
						if (!subresult) { unbind(_vars); return false; }
						++it1; ++it2;
					} else if (it1->first < it2->first) while (it1 != _avm.end() && it1->first < it2->first) {
						++it1;
					} else while (it2 != matrix->_avm.end() && it2->first < it1->first) {
						++it2;
					}
				}
				vars.insert(vars.end(), _vars.begin(), _vars.end());
				return true;
			}
			return false;
		}
		uhandle(AVMatrix) clone(uref(VarMap) vars) {
			auto result = unew(AVMatrix);
			for (auto it = _avm.begin(); it != _avm.end(); ++it) {
				auto var = it->second->asVariable();
				if (var == nullptr) {
					auto matrix = it->second->asMatrix();
					if (matrix == nullptr) result->set(it->first, it->second);
					else result->set(it->first, matrix->clone(vars));
				} else {
					if (vars.hasKey(var->name())) {
						auto var2 = vars.get(var->name());
						result->set(it->first, var2);
					} else {
						auto var2 = unew(Variable, var->name());
						vars.put(var->name(), var2);
						result->set(it->first, var2);
					}
				}
			}
			return result;
		}
		virtual string description() uoverride {
			stx::stringstream ss; bool first = true;
			for (auto it = _avm.begin(); it != _avm.end(); ++it) {
				if (first) first = false; else ss << ",";
				ss << it->first << "=" << it->second->description();
			}
			return ss.str();
		}
	};

}
