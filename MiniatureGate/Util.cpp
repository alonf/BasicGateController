// 
// 
// 

#include "Util.h"

using namespace std;

namespace Util
{
	/*int findChar(const String& str, char c, int from, int to)
	{
		while (from < to)
		{
			if (str[from] == c)
				break;
			++from;
		}
		return from;
	}*/

	

	void software_Reboot() //rebot the board
	{
		ESP.restart();
	}

	String& StringMap::at(const String& key)
	{
		for (int i = 0; i < _vector.size(); ++i)
		{
			if (_vector[i].first == key) //found
				return _vector[i].second;
		}
		_vector.push_back(pair<String, String>(key, String()));
		return _vector.rbegin()->second;
	}

	const String& StringMap::at(const String& key) const
	{
		for (int i = 0; i < _vector.size(); ++i)
		{
			if (_vector[i].first == key) //found
				return _vector[i].second;
		}
		return String();
	}
}
