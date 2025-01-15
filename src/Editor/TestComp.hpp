#pragma once

#include "Nexus-Core/nxpch.hpp"

struct TestComp
{
	int x1 = 5;
	int x2 = 32;

	friend std::ostream &operator<<(std::ostream &os, const TestComp &c)
	{
		os << c.x1 << " " << c.x2;
		return os;
	}

	friend std::istream &operator>>(std::istream &is, TestComp &c)
	{
		is >> c.x1 >> c.x2;
		return is;
	}
};