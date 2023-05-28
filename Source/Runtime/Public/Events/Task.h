#pragma once

#include "CoreTypes.h"
#include "Events/Observer.h"

#include <functional>

namespace ESource {

	template<typename ...Args>
	struct Task {
	private:
		TArray<const Observer*> Observers;

		void NotifyFinished() {
			for (auto& ItObserver : Observers) {
				ItObserver->Call();
			}
		}

	public:
		typedef std::function<void(Args...)> TaskFunction;

		TaskFunction Function;

		Task(const Task& Other) = delete;
		Task() : Function() { }
		Task(TaskFunction Function) :
			Function(Function) {}

		void Run(Args... Arguments) {
			Function(Arguments);
			NotifyFinished();
		}

		void AttachObserver(const Observer* value) {
			Observers.push_back(value);
		}

		void DetachObserver(const Observer* value) {
			Observers.erase(std::remove(Observers.begin(), Observers.end(), value), Observers.end());
		}
	};

}