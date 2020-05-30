#pragma once

#include <cstdint>
#include <type_traits>
#include <variant>

#include "llvm/Support/raw_ostream.h"
#include "rlc/ast/Type.hpp"
namespace rlc
{
	template<typename From, typename To>
	constexpr bool convertible()
	{
		return std::is_convertible<From, To>::value;
	}

	template<typename From, typename To>
	To runTimeFailConvert(From f)
	{
		if constexpr (convertible<From, To>())
			return static_cast<To>(f);
		else
		{
			assert(false && "could not cast different types");
			return To();
		}
	}

	class ScalarConstant
	{
		public:
		ScalarConstant(const ScalarConstant& other) = default;
		ScalarConstant(ScalarConstant&& other) = default;
		ScalarConstant& operator=(ScalarConstant&& other) = default;
		ScalarConstant& operator=(const ScalarConstant& other) = default;
		~ScalarConstant() = default;

		static ScalarConstant longC(std::int64_t l) { return ScalarConstant(l); }

		static ScalarConstant boolC(bool b) { return ScalarConstant(b); }

		static ScalarConstant doubleC(double d) { return ScalarConstant(d); }

		template<typename T>
		[[nodiscard]] bool isA() const
		{
			return std::holds_alternative<T>(content);
		}

		template<typename T>
		[[nodiscard]] T get() const
		{
			return std::get<T>(content);
		}

		template<typename Visitor>
		auto visit(Visitor&& visitor) const
		{
			return std::visit(std::forward<Visitor>(visitor), content);
		}

		template<typename Visitor>
		auto visit(Visitor&& visitor)
		{
			return std::visit(std::forward<Visitor>(visitor), content);
		}

		template<typename T>
		[[nodiscard]] T as() const
		{
			return visit(
					[](auto c) { return runTimeFailConvert<decltype(c), T>(c); });
		}

		template<typename T>
		[[nodiscard]] bool canBeCastedInto() const
		{
			return visit([](auto c) { return convertible<decltype(c), T>(); });
		}

		void print(llvm::raw_ostream& OS) const;
		void dump() const;

		[[nodiscard]] Type* type(TypeDB& db) const;
		template<typename T>
		ScalarConstant(T constant): content(constant)
		{
		}

		[[nodiscard]] bool operator==(const ScalarConstant& other) const
		{
			return content == other.content;
		}

		[[nodiscard]] bool operator!=(const ScalarConstant& other) const
		{
			return !(*this == other);
		}

		private:
		std::variant<std::int64_t, double, bool> content;
	};

}	 // namespace rlc
