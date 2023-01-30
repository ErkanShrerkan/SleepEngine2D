#pragma once
#include "eMessageTypes.h"
#include <unordered_map>
#include <vector>
class Observer;
class Postmaster
{
	static Postmaster* ourInstance;
	Postmaster();
	~Postmaster();
public:
	static Postmaster& GetInstance();
	void SendMail(eMessage aMsg);
	void Subscribe(Observer* anObserver, const eMessage aMsg);
	void UnSubscribe(Observer* anObserver, const eMessage aMsg);
private:
	std::unordered_map<eMessage, std::vector<Observer*>> mySubscribers;
	
};

