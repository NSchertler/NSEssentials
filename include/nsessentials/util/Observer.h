#pragma once

#pragma once

#include <functional>
#include <vector>

namespace nse
{
	namespace util
	{
		template <class... Args>
		class Observer
		{
		public:

			void operator()(Args... arguments) const
			{
				for (auto& f : subscribers)
					f(arguments...);
			}

			void Subscribe(std::function<void(Args...)> subscriber)
			{
				subscribers.push_back(subscriber);
			}

		private:
			std::vector<std::function<void(Args...)>> subscribers;
		};
	}
}