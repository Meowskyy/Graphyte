#pragma once

#include <string>

namespace Graphyte {
	class Console {
	private:
		static std::string inputText;

		static void GetCommand();
	public:
		static void Update();
	};
}