//
// Created by Andrey Solovyev on 06/11/2021.
//

#pragma once

#include <random>
#include <chrono>

#ifndef STRATEGY_TRADING_RANDOMER_H
#define STRATEGY_TRADING_RANDOMER_H


using namespace std::chrono;


class Randomer {
public:
	using UniformedInt = std::uniform_int_distribution<>;
	using UniformedReal = std::uniform_real_distribution<>;

	//	Randomer() : mtre{std::random_device{}()} {}//!ctor
	Randomer() { mtre.seed(SetSeed()); } //!ctor

	double operator()(double lowBound, double upperBound, [[maybe_unused]] int seed = std::random_device{}()) {
		//std::mt19937_64 mtre(seed);
		return UniformedReal{lowBound, upperBound}(mtre);
	}//!func

	int operator() (int lowerBound, int upperBound, [[maybe_unused]] int seed = std::random_device{}()) {
		//std::mt19937_64 mtre(seed);
		return UniformedInt{lowerBound, upperBound}(mtre);
	}//!func

	int SetSeed(){
		//		microseconds ms = duration_cast<microseconds>(system_clock::now().time_since_epoch());
		seconds s = duration_cast<seconds>(system_clock::now().time_since_epoch());
		double rndBase = s.count() / 13. * 3.141592653589793;
		int seed = (rndBase - int(rndBase)) * 1e7;
		if (seed < 0) seed = -seed;
		return seed;
	}//!func

private:
	std::mt19937 mtre;

};//!class

static Randomer prm;

#endif //STRATEGY_TRADING_RANDOMER_H
