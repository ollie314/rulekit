//
//  Tokenizer.h
//  NLParser Test
//
//  Created by Petr Homola on 10/16/11.
//  Copyright (c) 2011 __MyCompanyName__. All rights reserved.
//

#ifndef NLParser_Test_Tokenizer_h
#define NLParser_Test_Tokenizer_h

#ifdef _M_CEE_SAFE

#include "CliMacros.h"

namespace rulekit {
    
    ref class Tokenizer {
    private:
        System::String^ text;
        bool whitespace(wchar_t c) { return c == ' ' || c == '\t' || c == '\r' || c == '\n'; }
        bool alphanumeric(wchar_t c) { return (c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z') || (c >= '0' && c <= '9') || c == '$' || c >= 128; }
    public:
        Tokenizer(System::String^ s) : text(s) {}
        stl::vector<System::String^>^ tokens() {
            auto v = gcnew stl::vector<System::String^>();
            int pos = 0;
            for (;;) {
                while (pos < text->Length && whitespace(text[pos])) pos++;
                if (pos >= text->Length) break;
                if (alphanumeric(text[pos])) {
                    System::String^ s = "";
                    while (alphanumeric(text[pos])) { s += text[pos]; pos++; }
                    v->push_back(s);
                } else {
					System::String^ s = "" + text[pos];
                    v->push_back(s);
                    pos++;
                }
            }
            return v;
        }
    };
    
}

#else

#include <string>
#include <vector>

namespace rulekit {
    
    class Tokenizer {
    private:
        std::string text;
        bool whitespace(char c) const { return c == ' ' || c == '\t' || c == '\r' || c == '\n'; }
        bool alphanumeric(char c) const { return (c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z') || (c >= '0' && c <= '9') || c == '$' || c >= 128; }
    public:
        Tokenizer(const std::string& s) : text(s) {}
        std::vector<std::string> tokens() const {
            std::vector<std::string> v;
            const char* c = text.c_str();
            for (;;) {
                while (whitespace(*c)) c++;
                if (*c == 0) break;
                if (alphanumeric(*c)) {
                    char s[1000]; int i = 0;
                    while (alphanumeric(*c)) { s[i++] = *c; c++; }
                    s[i] = 0;
                    v.push_back(s);
                } else {
                    char s[2];
                    s[0] = *c;
                    s[1] = 0;
                    v.push_back(s);
                    c++;
                }
            }
            return v;
        }
    };
    
}

#endif
#endif
