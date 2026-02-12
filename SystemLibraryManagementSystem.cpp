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

//Member Class below:
class Member : public User {
private: 
	vector<int> borrowedBooks;

public:
	Member( int i, string n) : User(i, n) {}

	bool canBorrow() {
		return borrowedBooks.size() < MAX_BORROW;

	}
	void borrowBook(int bookId) {
		borrowedBooks.push_back(bookId);
	}
	void returnBook(int bookId) {
		for (int i = 0; i < borrowedBooks.size(); i++) {
			if (borrowedBooks[i] == bookId) {
				borrowedBooks.erase(borrowedBooks.begin() +i);
				break;
			}
		}
	}
	void showMenu() override {
		cout << "Member Menu: Search | Borrow | Return\n";
	}
};

//Librarian class below:
class Librarian : public User {
public:
	Librarian(int i, string n) : User(i,n) {}

	void showMenu() override {
		cout << "Librarian Menu: Add | Remove | View Reports\n";
	}
};

//LIbrary system class below:
class LibrarySystem {
private:
	vector<Book> books;
public:
	

};