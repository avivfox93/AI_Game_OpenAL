#pragma once

#include <vector>
#include <functional>


template<typename T, typename S>
class Transition
{
public:
	Transition(S from, S to, std::function<bool(T*)> predicate) : from(from),to(to),predicate(predicate) {}
	S GetTo() const { return to; }
	S GetFrom() const { return from; }
	bool Check(T* obj) const { return predicate(obj); }
private:
	S from, to;
	std::function<bool(T*)> predicate;
};

template<typename T, typename S>
class StateMachine
{
public:
	StateMachine(T* context, S startState) : context(context), state(startState){}
	void AddTransition(const Transition<T,S>& transition) { transitions.push_back(transition); }
	void AddTransition(S from, S to, std::function<bool(T*)> predicate) { AddTransition(Transition<T,S>(from,to,predicate)); }
	void Tick() {
		for (const auto& transition : transitions) {
			if (transition.GetFrom() == state && transition.Check(context)) {
				state = transition.GetTo();
				return;
			}
		}
	}
	S GetState() { return state; }
private:
	T* context;
	S state;
	std::vector<Transition<T,S>> transitions;
};

