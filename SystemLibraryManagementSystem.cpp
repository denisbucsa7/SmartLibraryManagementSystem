#include <iostream>
#include <vector>
#include <string>

using namespace std;

const int MAX_BORROW = 5;
const int LOAN_DAYS = 14;
const int RESERVATION_DAYS = 3;

//Book Class below:
class Book {
private:
	int id;
	string title;
	string author;
	string status; // Available or Borrowed or Reserved

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
	vector<int> borrowedDays; //Track days borrowed
	vector<int> reservedBooks;

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
	void reserveBook(int bookId) {
		reservedBooks.push_back(bookId);
	}
	void incrementDays() {
		for (int i = 0; i < borrowedDays.size(); i++) {
			borrowedDays[i]++;
		}
	}
	void checkOverdue() {
		for (int i = 0; i < borrowedDays.size(); i++) {
			if (borrowedDays[i] > LOAN_DAYS) {
				cout << "Book ID" << borrowedBooks[i]
					 << "is overdue\n";
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

//Administrator class below:
class Administrator : public User {
private: 
	int borrowLimit;
	double latePenalty;

public:
	Administrator(int i, string n)
		: User(i,n), borrowLimit(MAX_BORROW), latePenalty(2.0) { }

	void setBorrowLimit(int limit) {
		borrowLimit = limit;
	}

	void setLatePenalty(double penalty) {
		latePenalty = penalty;
	}

	void showRules() {
		cout << "Borrow Limit:" << borrowLimit << endl;
		cout << "Late Penalty per day: £" << latePenalty << endl;
	}

	void showMenu() override {
		cout << "Administrator Menu: Manage Users | Set Rules\n";
	}
};

//LIbrary system class below:
class LibrarySystem {
private:
	vector<Book> books;
public:
	void addBook(int id, string title, string author) {
		books.push_back(Book(id, title, author));
	}

	void searchBook(string title) {
		for (auto& book : books) {
			if (book.getTitle() == title) {
				cout << "Found: " << book.getTitle()
					 << " | Status: " << book.getStatus() << endl;
				return;
			}
		}
		cout << "Book not found\n";
	}

	void borrowBook(Member& member, int bookId) {
		for (auto& book : books) {
			if (book.getID() == bookId &&
				book.getStatus() == "Available") {

				if (!member.canBorrow()) {
					cout << "Borrow limit reached\n";
					return;
				}

				book.setStatus("Borrowed");
				member.borrowBook(bookId);
				cout << "Book borrowed successfully.\n";
				return;
			}
		}
		cout << "Borrow failed.\n";
	}
	void returnBook(Member& member, int bookId) {
		for (auto& book : books) {
			if (book.getID() == bookId) {
				book.setStatus("Available");
				member.returnBook(bookId);
				cout << "Book returned succesfully.\n";
			}
		}
	}
};
// Main program:
int main() {
	LibrarySystem library;

	library.addBook(1, "A tour of C++", "Bjarne Stroustrup");
	library.addBook(2, "Object-Oriented Thought Process", "Matt A. Weisfeld");

	Member m1(101, "Denis B");
	Librarian l1(201, "Catherine Williams");

	//Polymorphism
	User* user1 = &m1;
	User* user2 = &l1;

	user1->showMenu();
	user2->showMenu();

	//Borrowing
	library.searchBook("A tour of C++");
	library.borrowBook(m1,1);
	library.searchBook("A tour of C++");

	library.returnBook(m1,1);
	library.searchBook("A tour of C++");

	return 0;
}