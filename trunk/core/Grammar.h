//
//  Copyright (c) 2011 Codesign LLC. All rights reserved.
//  License: GPL v3
//

#pragma once

#include "ListParser.h"
#include "AVMatrix.h"

namespace rulekit {

	udeldef(void, EvalCallback, bool, uref(bool));

	uclass Rule {
	private:
		uhandle(AVMatrix) _head;
		stl::vector<uhandle(AVMatrix)> _tail;
	public:
		Rule() : _head(unew(AVMatrix)) {}
		Rule(uchandleref(AVMatrix) h) : _head(h) {}
		uhandle(Rule) clone() {
			VarMap vars;
			auto rule = unew(Rule, _head->clone(vars));
			for (auto it = _tail.begin(); it != _tail.end(); ++it) {
				rule->_tail.push_back((*it)->clone(vars));
			}
			return rule;
		}
		uhandle(AVMatrix) head() { return _head; }
		uref(stl::vector<uhandle(AVMatrix)>) tail() { return _tail; }
		uhandle(AVMatrix) addToTail() {
			auto avm = unew(AVMatrix);
			_tail.push_back(avm);
			return avm;
		}
	};

	uclass Grammar {
	private:
		stl::vector<uhandle(AVMatrix)> facts;
		stl::vector<uhandle(Rule)> rules;
        void initAvm(uchandleref(AVMatrix) avm, ucref(stl::vector<ListElement>) pairs) {
            for (auto it2 = pairs.begin(); it2 != pairs.end(); ++it2) {
                auto pair = (*it2).getList();
                string key = upurederef(pair[0].getText());
                string value = upurederef(pair[1].getText());
                if (value[0] == '$') *avm << key << unew(Variable, value);
                else *avm << key << value;
            }
        }
	public:
        Grammar() {}
        Grammar(ucref(string) source) {
            addSource(source);
        }
        bool addSource(ucref(string) source) {
            ListParser parser(source);
            bool wellformed;
            auto list = parser.parse(wellformed);
            if (wellformed) {
                for (auto it = list.begin(); it != list.end(); ++it) {
                    auto item = *it;
                    string itemType = upurederef(item.getList()[0].getText());
                    if (itemType == "fact") {
                        auto fact = addFact();
                        auto pairs = item.getList()[1].getList();
                        initAvm(fact, pairs);
                    } else if (itemType == "rule") {
                        auto rule = addRule();
                        auto avms = item.getList()[1].getList();
                        initAvm(rule->head(), avms[0].getList());
                        for (int i = 1; i < avms.size(); i++) {
                            auto tail = rule->addToTail();
                            initAvm(tail, avms[i].getList());
                        }
                    }
                }
                return true;
            }
            return false;
        }
		uhandle(Variable) createVar(ucref(string) s) { return unew(Variable, s); }
		uhandle(AVMatrix) addFact() {
			auto avm = unew(AVMatrix);
			facts.push_back(avm);
			return avm;
		}
		uhandle(Rule) addRule() {
			auto rule = unew(Rule);
			rules.push_back(rule);
			return rule;
		}
		uhandle(AVMatrix) goal() { return unew(AVMatrix); }
		void evaluate(uchandleref(AVMatrix) goal, udel(EvalCallback) callback) {
			evaluate2(goal, callback);
			bool cont = true;
			callback(false, cont);
		}
	private:
		bool evaluate2(uchandleref(AVMatrix) goal, udelref(EvalCallback) callback) {
			bool cont = true;
			stl::vector<uptr(Variable)> vars;
			for (auto it = facts.begin(); it != facts.end(); ++it) {
				auto result = goal->unifiable(ugetptr(*it), vars);
				if (result) callback(true, cont);
				Value::unbind(vars);
				vars.clear();
				if(!cont) break;
			}
			if (cont) for (auto it = rules.begin(); it != rules.end(); ++it) {
				auto rule = (*it)->clone();
				auto result = goal->unifiable(ugetptr(rule->head()), vars);
				if (result) {
					if (!evaluateTail(rule->tail(), 0, callback)) break;
				}
				Value::unbind(vars);
				vars.clear();
			}
			return cont;
		}

		uclass TailCallback {
		private:
			upureref(stl::vector<uhandle(AVMatrix)>) _tail;
			int _index;
			udelref(EvalCallback) _callback;
			uptr(Grammar) _grammar;
		public:
			TailCallback(uref(stl::vector<uhandle(AVMatrix)>) t, int i, udelref(EvalCallback) c, uptr(Grammar) g)
				: _tail(t), _index(i), _callback(c), _grammar(g) {}
			void udelfunc(bool unified, uref(bool) cont) {
				if (unified) {
                    auto localCallback = _index + 1 == _tail.size() ? _callback : unewdel(EvalCallback, TailCallback, _tail, _index + 1, _callback, _grammar);
					cont = _grammar->evaluate2(_tail[_index], localCallback);
				} else cont = true;
			}
		};

		bool evaluateTail(uref(stl::vector<uhandle(AVMatrix)>) tail, int index, udelref(EvalCallback) callback) {
			stl::vector<uptr(Variable)> vars;
            auto localCallback = index + 1 == tail.size() ? callback : unewdel(EvalCallback, TailCallback, tail, index + 1, callback, this);
			bool cont = evaluate2(tail[index], localCallback);
			Value::unbind(vars);
			return cont;
		}
	};

}
