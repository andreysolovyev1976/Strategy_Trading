//
// Created by Andrey Solovyev on 2019-09-01.
//


#pragma once

#include<unordered_map>
#include<string_view>
#include<string>
#include<iostream>

#ifndef TASK_104_INI_H
#define TASK_104_INI_H

namespace Ini {

	using namespace std;
	using Section = unordered_map<string, string>;

	pair<string_view, string_view> Split(string_view line, char by) {
		size_t pos = line.find(by);
		string_view left = line.substr(0, pos);

		if (pos < line.size() and pos + 1 < line.size()) {
			return {left, line.substr(pos + 1)};
		} else {
			return {left, string_view()};
		}
	}//!end of func

	bool SectionHeadlineFound (string_view value) {
		if (!value.empty() and value.front() == '[' and value.back() == ']') return true;
		else return false;
	}//!end of func

	string_view Unquote(string_view value) {
		if (!value.empty() && value.front() == '[') {
			value.remove_prefix(1);
		}
		if (!value.empty() && value.back() == ']') {
			value.remove_suffix(1);
		}
		return value;
	}//!end of func

	class Document {
	public:
		Section& AddSection (string name) {

			//Section& output = sections[name];

			return sections[name];
		}//!end of func

		const Section& GetSection (const string &name) const {

			auto it = sections.find(name);
			if (it == sections.end()) throw std::out_of_range ("unknown fucking section\n");
			else return it->second;
		}//!end of func

		size_t SectionCount () const {
			return sections.size();
		}//!end of func

	private:
		unordered_map<string, Section> sections;

	};//!end of class

	Document Load (istream &input) {
		Document document;
		Section* section;
		string str;

		while ( input.peek() != EOF )
		{
			getline (input, str);

			if ( SectionHeadlineFound (str))
			{
				str = Unquote (str);
				section = &document.AddSection(str);
			}
			else if (section and !str.empty())
			{
				const auto& [key, value] = Split (str, '=');
				section->insert (make_pair (key, value));
			}
		}

		return document;
	}//!end of func

}//!end of namespace

#endif //TASK_104_INI_H

/*
Compile error: b'In file included from /tmp/submission8t4dphvr/check.cpp:1:
 /tmp/submission8t4dphvr/ini.h: In function \xe2\x80\x98Ini::Document Ini::Load(std::istream&)\xe2\x80\x99:
 /tmp/submission8t4dphvr/ini.h:75:28: error: could not convert
 \xe2\x80\x98std::getline<char, std::char_traits<char>, std::allocator<char> >((* & input), str)\xe2\x80\x99
 from \xe2\x80\x98std::basic_istream<char>\xe2\x80\x99 to \xe2\x80\x98bool\xe2\x80\x99
 for (string str; getline (input, str);)
 ~~~~~~~~^~~~~~~~~~~~
 compilation terminated due to -Wfatal-errors.\n'


 Compile error: b'In file included from /tmp/submissiondb31b270/check.cpp:1:
 /tmp/submissiondb31b270/ini.h: In function \xe2\x80\x98Ini::Document Ini::Load(std::istream&)\xe2\x80\x99:
 /tmp/submissiondb31b270/ini.h:76:19: error: could not convert
 \xe2\x80\x98std::getline<char, std::char_traits<char>, std::allocator<char> >((* & input), str)\xe2\x80\x99
 from \xe2\x80\x98std::basic_istream<char>\xe2\x80\x99 to \xe2\x80\x98bool\xe2\x80\x99
 while ( getline (input, str);)
 ~~~~~~~~^~~~~~~~~~~~
 compilation terminated due to -Wfatal-errors.
 '

 Compile error: b'In file included from /tmp/submission7m8hkefl/check.cpp:1:
 /tmp/submission7m8hkefl/ini.h: In function \xe2\x80\x98Ini::Document Ini::Load(std::istream&)\xe2\x80\x99:
 /tmp/submission7m8hkefl/ini.h:76:19: error: could not convert
 \xe2\x80\x98std::getline<char, std::char_traits<char>, std::allocator<char> >((* & input), str)\xe2\x80\x99
 from \xe2\x80\x98std::basic_istream<char>\xe2\x80\x99 to \xe2\x80\x98bool\xe2\x80\x99
 while ( getline (input, str))
            ~~~~~~~~^~~~~~~~~~~~
            compilation terminated due to -Wfatal-errors.'



 Compile error: b'In file included from /tmp/submission9adet5xb/check.cpp:1:
 /tmp/submission9adet5xb/ini.h: In function \xe2\x80\x98Ini::Document Ini::Load(std::istream&)\xe2\x80\x99:
 /tmp/submission9adet5xb/ini.h:76:11: error: invalid use of incomplete type \xe2\x80\x98std::istream\xe2\x80\x99
 {aka \xe2\x80\x98class std::basic_istream<char>\xe2\x80\x99}
 while ( input.peek() != EOF )
 ^~~~~
 compilation terminated due to -Wfatal-errors.\n'



 */