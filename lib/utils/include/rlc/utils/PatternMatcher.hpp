
/*
Copyright 2025 Massimo Fioravanti

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

	 http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
*/
#pragma once

#include "llvm/Support/raw_ostream.h"
#include "mlir/IR/BuiltinOps.h"

namespace mlir::rlc
{

	namespace impl
	{
		template<typename T>
		struct function_traits;

		template<typename Ret, typename ClassType, typename Arg, typename... Args>
		struct function_traits<Ret (ClassType::*)(Arg, Args...) const>
		{
			using return_type = Ret;
			using arg1_type = Arg;
			using function_type = std::function<Ret(Arg, Args...)>;
		};
	}	 // namespace impl

	/* *
	 * A type serializer that can be configured with lambda functions.
	 * The usefullnes of suing this over regular switch is that it caches
	 * results so it does not recompute everything every time.
	 * */
	class TypeSerializer
	{
		public:
		template<typename Callable>
		void add(Callable&& callable)
		{
			using traits =
					impl::function_traits<decltype(&std::decay_t<Callable>::operator())>;

			using MLIRType = typename traits::arg1_type;
			const auto dispatcher = [callable](mlir::Type type) {
				std::string toWrite;
				llvm::raw_string_ostream OS(toWrite);
				callable(mlir::cast<MLIRType>(type), OS);
				return toWrite;
			};
			if (patterns.contains(
							mlir::FunctionType::getTypeID().getAsOpaquePointer()))
			{
				assert(false && "Pattern for type already registered");
				return;
			}
			patterns[MLIRType::getTypeID().getAsOpaquePointer()] = dispatcher;
		}

		llvm::StringRef convert(mlir::Type type)
		{
			if (auto iter = cache.find(type.getAsOpaquePointer());
					iter != cache.end())
				return (*iter).second;

			auto candidate = patterns.find(type.getTypeID().getAsOpaquePointer());
			if (candidate != patterns.end())
			{
				cache[type.getAsOpaquePointer()] = candidate->second(type);
				return cache[type.getAsOpaquePointer()];
			}
			type.dump();
			llvm_unreachable("NO CONVERSION PATTERN FOUND ");
			return "NO CONVERSION PATTERNT FOUND";
		}

		private:
		std::map<const void*, std::function<std::string(mlir::Type)>> patterns;
		std::map<const void*, std::string> cache;
	};

	class StreamWriter
	{
		public:
		class IndenterRAII
		{
			public:
			IndenterRAII(StreamWriter& writer): writer(&writer)
			{
				writer.indentation_level++;
			}
			~IndenterRAII() { writer->indentation_level--; }

			private:
			StreamWriter* writer;
		};
		StreamWriter(llvm::raw_ostream& out): OS(&out) { addTypeSerializer(); }

		template<typename... ObjectType>
		StreamWriter& write(ObjectType... toPrint)
		{
			if (startOfLine)
				OS->indent(indentation_level * 4);
			((*OS << toPrint), ...);
			startOfLine = false;
			return *this;
		}

		template<typename... ObjectType>
		StreamWriter& writenl(const ObjectType&... toPrint)
		{
			if (startOfLine)
				OS->indent(indentation_level * 4);
			size_t counter = 0;
			((*OS << toPrint), ...);
			endLine();
			return *this;
		}

		StreamWriter& indentOnce(int indentationLevel)
		{
			if (startOfLine)
				OS->indent(indentation_level * 4);
			OS->indent(indentationLevel * 4);
			startOfLine = false;
			return *this;
		}

		StreamWriter& endLine()
		{
			*OS << '\n';
			startOfLine = false;
			indentOnce(0);
			startOfLine = true;
			return *this;
		}

		IndenterRAII indent() { return IndenterRAII(*this); }

		template<typename Callable>
		void addTypePattern(Callable&& callable, size_t serializerId = 0)
		{
			typeSerializer[serializerId].add(std::forward<Callable>(callable));
		}

		StreamWriter& writeType(mlir::Type type, size_t serializerId = 0)
		{
			write(typeSerializer[serializerId].convert(type));
			return *this;
		}
		StreamWriter& writeTypenl(mlir::Type type, size_t serializerId = 0)
		{
			writenl(typeSerializer[serializerId].convert(type));
			return *this;
		}

		void addTypeSerializer() { typeSerializer.emplace_back(); }

		TypeSerializer& getTypeSerializer(size_t i = 0)
		{
			return typeSerializer[i];
		}

		private:
		size_t indentation_level = 0;
		bool startOfLine = true;
		llvm::raw_ostream* OS;
		std::vector<TypeSerializer> typeSerializer;
	};

	class PatternImpl
	{
		public:
		virtual ~PatternImpl() = default;
		virtual bool matches(mlir::Operation* op) = 0;
		virtual void apply(mlir::Operation* op, StreamWriter& OS) = 0;
	};

	template<typename ObjectType, typename OperationType>
	OperationType getOpTypeImpl(
			void (ObjectType::*)(OperationType, StreamWriter&))
	{
		return OperationType(nullptr);
	}

	template<typename Matcher>
	class Pattern: public PatternImpl
	{
		public:
		using OperationType = decltype(getOpTypeImpl(&Matcher::apply));

		Pattern(Matcher&& matcher): matcher(std::forward<Matcher>(matcher)) {}

		virtual bool matches(mlir::Operation* op) final
		{
			return mlir::isa<OperationType>(op);
		}

		virtual void apply(mlir::Operation* op, StreamWriter& OS) final
		{
			matcher.apply(mlir::cast<OperationType>(op), OS);
		}

		private:
		Matcher matcher;
	};

	class PatternMatcher
	{
		public:
		PatternMatcher(llvm::raw_ostream& out): OS(out) {}

		void apply(mlir::ModuleOp op)
		{
			op.walk([this](mlir::Operation* current) {
				for (auto& pattern : patterns)
				{
					if (pattern->matches(current))
					{
						pattern->apply(current, OS);
						break;
					}
				}
			});
		}

		template<typename Matcher>
		void add(Matcher&& to_add)
		{
			patterns.push_back(
					std::make_unique<Pattern<Matcher>>(std::forward<Matcher>(to_add)));
		}

		template<typename Matcher, typename... Args>
		void add(Args&&... args)
		{
			patterns.push_back(std::make_unique<Pattern<Matcher>>(
					Matcher(std::forward<Args>(args)...)));
		}

		StreamWriter& getWriter() { return OS; }

		void addTypeSerializer() { OS.addTypeSerializer(); }

		private:
		std::vector<std::unique_ptr<PatternImpl>> patterns;
		StreamWriter OS;
	};
}	 // namespace mlir::rlc
