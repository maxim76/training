#include <iostream>
#include <vector>
#include <queue>


class Company
{
	struct Person
	{
		int id;
		int time;
		std::vector<Person *> subordinates;
	};
public:
	Company() : root{ nullptr }
	{
	}

	Person * add(int id, int time, Person *boss);
	void display();

	int getLongestTime();
	Person *getLast();

private:
	Person *root;
	int _getLongestTime(Person *person);
	Person *_getLast(Person *person);
};

Company::Person * Company::add(int id, int time, Person * boss)
{
	Company::Person *newPerson = new(Person);
	newPerson->id = id;
	newPerson->time = time;
	if (boss == nullptr) root = newPerson;
	else boss->subordinates.push_back(newPerson);
	return newPerson;
}

void Company::display()
{
	std::queue<Company::Person *> q;
	Company::Person *person;
	q.push(root);
	while (!q.empty())
	{
		person = q.front();
		q.pop();
		std::cout << person->id << " " << person->time << "\n";
		for (auto item : person->subordinates)
		{
			q.push(item);
		}
	}
}

int Company::_getLongestTime(Person *person)
{
	//if (person == nullptr) return 0;
	int subordinateLongestTime = 0;
	for (auto subordinate : person->subordinates)
	{
		subordinateLongestTime = std::max(subordinateLongestTime, this->_getLongestTime(subordinate));
	}
	return person->time + subordinateLongestTime;
}

int Company::getLongestTime()
{
	if (root == nullptr) 
	{
		std::cout << "Company is empty\n";
		return -1;
	}
	return _getLongestTime(root);
}

Company::Person * Company::_getLast(Person *person)
{
	Person *currentSolution = person;
	Person *localCandidate;
	for (auto subordinate : person->subordinates)
	{
		localCandidate = _getLast(subordinate);
		if (localCandidate->time > currentSolution->time)
		{
			currentSolution = localCandidate;
		}
	}
	return currentSolution;
}

Company::Person * Company::getLast()
{
	if (root == nullptr)
	{
		std::cout << "Company is empty\n";
		return nullptr;
	}
	return _getLast(root);
}

bool unittest()
{
	// Case 1. Empty
	Company company;
	std::cout << "Longest time is " << company.getLongestTime() << "\n";
	std::cout << "Last person ID is " << (company.getLast() ? company.getLast()->id : -1) << "\n";
	
	auto boss = company.add(0,0, nullptr);
	auto p1 = company.add(1, 1, boss);
	auto p2 = company.add(2, 2, boss);
	auto p3 = company.add(3, 3, boss);

	auto p11 = company.add(11, 11, p1);
	auto p12 = company.add(12, 12, p1);
	auto p13 = company.add(13, 13, p1);

	auto p21 = company.add(21, 21, p2);
	auto p22 = company.add(22, 22, p2);

	company.display();

	std::cout << "Longest time is " << company.getLongestTime() << "\n";

	auto lastPerson = company.getLast();
	std::cout << "Last person ID is " << (lastPerson ? lastPerson->id : -1) << "\n";


	return true;
}

int main()
{
	unittest();
	return 0;
}