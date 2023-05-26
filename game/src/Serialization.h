#pragma once

#include <lib/cereal/archives/portable_binary.hpp>
#include <lib/cereal/cereal.hpp>
#include <lib/cereal/types/vector.hpp>

namespace spt
{
	template<typename T>
	T deserialize(std::vector<char> data)
	{
		std::stringstream ss(std::string(data.begin(), data.end()));
		cereal::PortableBinaryInputArchive deserializer(ss);

		T obj = T();

		deserializer(obj);

		return obj;
	}

	template<typename T>
	std::vector<char> serialize(T& p)
	{
		std::stringstream ss;
		cereal::PortableBinaryOutputArchive serializer(ss);

		serializer(p);

		std::string str = ss.str();

		return std::vector<char>(str.begin(), str.end());
	}
}
