#pragma once
#include <iostream>

class Calculator
{
	int a;
	int b;
public:
	void setAnB(int a_, int b_)
	{
		this->a = a_;
		this->b = b_;
	}
	int sum()
	{
		return a + b;
	}
};