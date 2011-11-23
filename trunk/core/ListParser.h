//
//  ListParser.h
//  NLParser Test
//
//  Created by Petr Homola on 10/17/11.
//  Copyright (c) 2011 __MyCompanyName__. All rights reserved.
//

#ifndef NLParser_Test_ListParser_h
#define NLParser_Test_ListParser_h

#ifdef _M_CEE_SAFE

#include "Tokenizer.h"

namespace rulekit {
    
    value class ListElement {
    public:
        bool islist;
        System::String^ text;
        stl::vector<ListElement>^ list;
        //ListElement(String^ s) : islist(false), text(s) {}
        //ListElement(stl::vector<ListElement^> l) : islist(true), list(l) {}
        System::String^ getText() { return islist ? nullptr : text; }
        stl::vector<ListElement>% getList() { return *list; }
		bool operator==(ListElement% el) {
			if (islist) return text == el.text;
			else return list == el.list;
		}
        virtual System::String^ ToString() override {
            if (islist) {
                stx::stringstream ss;
                ss << "[ ";
				for each (auto% el in list) {
                    ss << el.ToString() << " ";
                }
                ss << "]";
                return ss.str();
            } else return text;
        }
    };
    
    ref class ListParser {
    private:
        stl::vector<System::String^> tokens;
        stl::vector<ListElement>^ parse(int% pos, bool% wellformed) {
            auto v = gcnew stl::vector<ListElement>();
            wellformed = true;
            if (tokens[pos] != "(") wellformed = false;
            else {
                pos++;
                for (;;) {
                    if (pos >= tokens.size()) { wellformed = false; break; }
                    if (tokens[pos] == ")") { pos++; break; }
                    if (tokens[pos] == "(") {
                        auto list = parse(pos, wellformed);
                        if (!wellformed) break;
						ListElement listElement;
						listElement.islist = true;
						listElement.list = list;
                        v->push_back(listElement);
                    } else {
						ListElement listElement;
						listElement.islist = false;
						listElement.text = tokens[pos];
                        v->push_back(listElement);
                        pos++;
                    }
                }
            }
            return v;
        }
    public:
        ListParser(System::String^ s) {
            Tokenizer t(s);
            tokens = t.tokens();
        }
        ListParser(stl::vector<System::String^>% v) : tokens(v) {}
        stl::vector<ListElement> parse(bool% wellformed) {
            int pos = 0;
            return *parse(pos, wellformed);
        }
    };
    
}

#else

#include <sstream>
#include <string.h>
#include "Tokenizer.h"

namespace rulekit {
    
    class ListElement {
    private:
        bool islist;
        std::string text;
        std::vector<ListElement> list;
    public:
        ListElement(const std::string& s) : islist(false), text(s) {}
        ListElement(const std::vector<ListElement>& l) : islist(true), list(l) {}
        const std::string* getText() const { return islist ? nullptr : &text; }
        const std::vector<ListElement>& getList() const { return list; }
        std::string description() const {
            if (islist) {
                std::stringstream ss;
                ss << "[ ";
#ifdef __cplusplus_cli
				for each (const auto& el in list) {
#else
                for (const auto& el : list) {
#endif
                    ss << el.description() << " ";
                }
                ss << "]";
                return ss.str();
            } else return text;
        }
    };
    
    class ListParser {
    private:
        std::vector<std::string> tokens;
        std::vector<ListElement> parse(int& pos, bool& wellformed) const {
            std::vector<ListElement> v;
            wellformed = true;
            if (strcmp(tokens[pos].c_str(), "(")) wellformed = false;
            else {
                pos++;
                for (;;) {
                    if (pos >= tokens.size()) { wellformed = false; break; }
                    if (!strcmp(tokens[pos].c_str(), ")")) { pos++; break; }
                    if (!strcmp(tokens[pos].c_str(), "(")) {
                        auto list = parse(pos, wellformed);
                        if (!wellformed) break;
                        v.push_back(ListElement(list));
                    } else {
                        v.push_back(ListElement(tokens[pos]));
                        pos++;
                    }
                }
            }
            return v;
        }
    public:
        ListParser(const std::string& s) {
            Tokenizer t(s);
            tokens = t.tokens();
        }
        ListParser(const std::vector<std::string>& v) : tokens(v) {}
        std::vector<ListElement> parse(bool& wellformed) const {
            int pos = 0;
            return parse(pos, wellformed);
        }
    };
    
}

#endif
#endif
