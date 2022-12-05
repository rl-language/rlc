#pragma once

#include <cstdint>
#include <type_traits>
#include <variant>

#include "llvm/Support/YAMLTraits.h"
#include "llvm/Support/raw_ostream.h"
#include "rlc/ast/Type.hpp"
#include "rlc/utils/SourcePosition.hpp"
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

	template<typename T>
	llvm::StringRef primitiveTypeToString();

	template<>
	inline llvm::StringRef primitiveTypeToString<std::int64_t>()
	{
		return "int64";
	}

	template<>
	inline llvm::StringRef primitiveTypeToString<double>()
	{
		return "double";
	}

	template<>
	inline llvm::StringRef primitiveTypeToString<bool>()
	{
		return "bool";
	}

	class ScalarConstant
	{
		public:
		static constexpr const char* name = "scalar";
		friend llvm::yaml::MappingTraits<rlc::ScalarConstant>;
		ScalarConstant(const ScalarConstant& other) = default;
		ScalarConstant(ScalarConstant&& other) = default;
		ScalarConstant& operator=(ScalarConstant&& other) = default;
		ScalarConstant& operator=(const ScalarConstant& other) = default;
		~ScalarConstant() = default;

		static ScalarConstant longC(
				std::int64_t l, SourcePosition pos = SourcePosition())
		{
			return ScalarConstant(l, std::move(pos));
		}

		static ScalarConstant boolC(bool b, SourcePosition pos = SourcePosition())
		{
			return ScalarConstant(b, pos);
		}

		static ScalarConstant doubleC(
				double d, SourcePosition pos = SourcePosition())
		{
			return ScalarConstant(d, pos);
		}

		template<typename T>
		static ScalarConstant make(T val)
		{
			auto constant = ScalarConstant::boolC(false);
			constant.content = val;
			return val;
		}

		template<typename T>
		[[nodiscard]] bool isA() const
		{
			return std::holds_alternative<T>(content);
		}

		template<typename T>
		[[nodiscard]] T& get()
		{
			return std::get<T>(content);
		}

		template<typename T>
		[[nodiscard]] const T& get() const
		{
			return std::get<T>(content);
		}

		[[nodiscard]] const int64_t& getInt() const
		{
			return std::get<std::int64_t>(content);
		}

		[[nodiscard]] int64_t& getInt() { return std::get<std::int64_t>(content); }

		template<typename Visitor>
		auto visit(Visitor&& visitor) const -> decltype(visitor(get<bool>()))
		{
			return std::visit(std::forward<Visitor>(visitor), content);
		}

		template<typename Visitor>
		auto visit(Visitor&& visitor) -> decltype(visitor(get<bool>()))
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
		ScalarConstant(T constant, SourcePosition pos)
				: content(constant), position(std::move(pos))
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
		[[nodiscard]] const SourcePosition& getPosition() const { return position; }
		void setPosition(const SourcePosition& newPoisition)
		{
			position = newPoisition;
		}

		[[nodiscard]] llvm::StringRef typeToString() const
		{
			return visit([]<typename T>(const T& value) -> llvm::StringRef {
				return primitiveTypeToString<T>();
			});
		}

		private:
		std::variant<std::int64_t, double, bool> content;
		SourcePosition position;
	};

}	 // namespace rlc

template<>
struct llvm::yaml::MappingTraits<rlc::ScalarConstant>
{
	static void mapping(IO& io, rlc::ScalarConstant& value)
	{
		assert(io.outputting());
		value.visit([&]<typename T>(T& Value) -> void {
			auto tID = rlc::primitiveTypeToString<T>().str();
			io.mapRequired(tID.c_str(), Value);
			io.mapTag(tID);
		});
		if (not value.getPosition().isMissing())
			io.mapRequired("position", value.position);
	}
};
