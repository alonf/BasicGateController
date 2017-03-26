// Util.h

#ifndef _UTIL_h
#define _UTIL_h

#include "arduino.h"
#include <algorithm>
#include <vector>

namespace Util
{
	void software_Reboot(); //rebot the board

	template <typename T, std::size_t n>
	void String2Array(const String &str, T(&arr)[n])
	{
		memcpy(arr, str.c_str(), std::min(n, str.length() + 1));
	}

	class StringMap
	{
	private:
		std::vector<std::pair<String, String>> _vector;

	public:
		void clear() { _vector.clear(); }
		String &at(const String &key);
		const String &at(const String &key) const;
		

		String &operator[](const String &key) { return at(key); }
	};
}


#endif

