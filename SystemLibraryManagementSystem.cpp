#include <iostream>
#include <vector>
#include <string>

using namespace std;

const int MAX_BORROW = 5;

//Book Class below:
class Book {
private:
	int id;
	string title;
	string author;
	string status; // Available or Borrowed

public:
	Book(int i, string t, string a)
		: id(i), title(t), author(a), status("Available") { }

	int getID() const { return id;}
	string getTitle() const { return title;}
	string getStatus() const {return status;}

	void setStatus(string s) {status = s;}
};

//User Class below:
class User {
protected:
	int id;
	string name;

public:
	User(int i, string n) : id(i), name(n) {}
	virtual void showMenu() = 0; //Polymorphism

	int getId() const {return id;}
	string getName() const { return name;}

	virtual ~User() {}
};
