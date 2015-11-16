// Console color for pretty printf
// src: http://cc.byexamples.com/2007/01/20/print-color-string-without-ncurses/

#ifndef CCOLOR_HPP_
#define CCOLOR_HPP_

#include <iostream>
#include <sstream>

class cColor {
public:
	class attr {
	public:
		static const int AllOff = 0;
		static const int Bold = 1;
		static const int UnderScore = 4;
		static const int Blink = 5;
		static const int ReverseVideo = 7;
		static const int Concealed = 8;
	};

	class cText {
	public:
		static const int NoChange 	= 0;
		static const int Black 	= 30;
		static const int Red 		= 31;
		static const int Green 	= 32;
		static const int Yellow 	= 33;
		static const int Blue 		= 34;
		static const int Magenta 	= 35;
		static const int Cyan 		= 36;
		static const int White 	= 37;
	};

	class cBg { // Background
	public:
		static const int NoChange 	= 0;
		static const int Black 	= 40;
		static const int Red 		= 41;
		static const int Green 	= 42;
		static const int Yellow 	= 43;
		static const int Blue 		= 44;
		static const int Magenta 	= 45;
		static const int Cyan 		= 46;
		static const int White 	= 47;
	};


	// This function show the syntax of color string and will
	// be used through this class.
	static std::string _colorstring(int attr, int ctext, int bg) {
	  std::ostringstream ret;
	  ret << "\e[" << attr;
	  if (ctext != 0) {
		  ret << ";" << ctext ;
		  if (bg !=0 ) {
			  ret << ";" << bg;
		  }
	  }
	  ret << "m";
	  return ret.str();
	}


	static std::string colorText(std::string txt, int attr, int ctext, int bg) {
		std::ostringstream ret;
		ret << _colorstring(attr,ctext,bg) << txt;
		return ret.str();
	}

	static std::string colorText(char* txt, int attr, int ctext, int bg) {
			std::string _t = txt;
			return cColor::colorText(_t,attr, ctext,  bg);
	}

	static std::string colorText(std::string txt, cColor predefined) {
		return cColor::colorText(txt,
				predefined.m_attr,  predefined.m_ctxt, predefined.m_cbg);
	}

	static std::string colorText(char* txt, cColor predefined) {
			std::string _t = txt;
			return cColor::colorText(_t, predefined);
		}


	static const std::string Default;

	cColor(int attr, int text, int bg)
	{
		m_attr = attr;
		m_ctxt = text;
		m_cbg = bg;
	}

	friend std::ostream & operator<<(std::ostream &os, const cColor& c){
		os << _colorstring(c.m_attr,c.m_ctxt,c.m_cbg);
		return os;
	}


private:
	int m_attr;
	int m_ctxt;
	int m_cbg;

};

const std::string cColor::Default = "\e[0m"; // You can achieve same result with cColor(0,0,0);

#endif
