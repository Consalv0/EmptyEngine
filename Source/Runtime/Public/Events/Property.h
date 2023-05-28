#pragma once

#include "CoreTypes.h"
#include "Events/Observer.h"

namespace ESource {

	template <typename T>
	struct Property {
		T InternalValue;
		TArray<const Observer*> Observers;

		virtual inline void NotifyChange() {
			for (auto& ItObserver : Observers) {
				ItObserver->Call();
			}
		}

	public:
		Property() : InternalValue() {
		}

		Property(const T & value) : InternalValue(value) {
		}

		T virtual inline Get() const { return InternalValue; }
		inline operator T() const { return InternalValue; }

		T virtual inline Set(const T & value) {
			InternalValue = value;
			NotifyChange();
			return InternalValue;
		}
		T virtual inline operator=(const T & value) { return Set(value); }

		bool virtual inline operator==(const T & ValueOne) { return ValueOne == InternalValue; }

		virtual inline void AttachObserver(const Observer* value) {
			Observers.push_back(value);
		}

		virtual inline void DetachObserver(const Observer* value) {
			Observers.erase(std::remove(Observers.begin(), Observers.end(), value), Observers.end());
		}
	};

}