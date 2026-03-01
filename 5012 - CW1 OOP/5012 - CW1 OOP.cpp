#include <iostream>
#include <vector>
#include <string>

using namespace std;

const int MAX_BORROW = 5;
const int LOAN_DAYS = 14;
const int RESERVATION_DAYS = 3;

enum BookStatus {
	Available,
	Borrowed,
	Reserved
};

//Book Class below:
class Book {
private:
	int id;
	string title;
	string author;
	BookStatus status; // Available or Borrowed or Reserved

public:
	Book(int i, string t, string a)
		: id(i), title(t), author(a), status(Available) {
	}

	int getID() const { return id; }
	string getTitle() const { return title; }
	BookStatus getStatus() const { return status; }

	void setStatus(BookStatus s) { status = s; }

	string getStatusString() const {
		if (status == Available) return "Available";
		if (status == Borrowed) return "Borrowed";
		return "Reserved";
	}
};

//User Class below:
class User {
protected:
	int id;
	string name;
	string email;
	string password;

public:
	User(int i, string n, string e, string p)
		: id(i), name(n), email(e), password(p) {
	}

	int getId() const { return id; }
	string getName() const { return name; }

	virtual void login() {
		cout << name << " logged in\n";
	}

	virtual void logout() {
		cout << name << " logged out\n";
	}

	virtual void showMenu() = 0; //Polymorphism

	virtual ~User() {}
};

//Member Class below:
class Member : public User {
private:
	vector<int> borrowedBooks; // Tracks books borrowed
	vector<int> borrowedDays; //Track days borrowed
	vector<int> reservedBooks; 
	vector<int> reservationDays;

public:
	Member(int i, string n, string e, string p)
		: User(i, n, e, p) {
	}

	bool canBorrow(int limit) {
		return borrowedBooks.size() < limit;
	}

	void borrowBook(int bookId) {
		borrowedBooks.push_back(bookId);
		borrowedDays.push_back(0);
	}
	void returnBook(int bookId) {
		for (int i = 0; i < borrowedBooks.size(); i++) {
			if (borrowedBooks[i] == bookId) {
				borrowedBooks.erase(borrowedBooks.begin() + i);
				borrowedDays.erase(borrowedDays.begin() + i);
				break;
			}
		}
	}
	void reserveBook(int bookId) {
		reservedBooks.push_back(bookId);
		reservationDays.push_back(0);
	}
	void incrementReservationDays() {
		for (int i = 0; i < reservationDays.size(); i++) {
			reservationDays[i]++;
		}
	}
	void checkReservationDaysExpiry() {
		for (int i = 0; i < reservationDays.size(); i++) {
			if (reservationDays[i] > RESERVATION_DAYS) {
				cout << "Reservation expired for Book ID "
					 << reservedBooks[i] << endl;
			}
		}
	}
	void incrementDays() {
		for (int i = 0; i < borrowedDays.size(); i++) {
			borrowedDays[i]++;
		}
	}
	void checkOverdue() {
		for (int i = 0; i < borrowedDays.size(); i++) {
			if (borrowedDays[i] > LOAN_DAYS - 1) {
				cout << "Reminder: Book ID " 
					 << borrowedBooks[i]
					 << " is overdue tomorrow.\n";
			}
			if (borrowedDays[i] > LOAN_DAYS) {
				cout << "Book ID "
				     << borrowedBooks[i]
					 << " is overdue.\n";
			}
		}
	}

	void showMenu() override {
		cout << "Member Menu: Search | Borrow | Return\n";
	}
};

//LIbrary system class below:
class LibrarySystem {
private:
	vector<Book> books;
	int borrowLimit;
	double latePenalty;

public:

	LibrarySystem() {
		borrowLimit = 5;
		latePenalty = 2.0;
	}

	void setBorrowLimit(int limit) {
		borrowLimit = limit;
	}

	int getBorrowLimit() const {
		return borrowLimit;
	}

	void addBook(int id, string title, string author) {
		books.push_back(Book(id, title, author));
	}

	void removeBook(int id) {
		for (int i = 0; i < books.size(); i++) {
			if (books[i].getID() == id) {
				books.erase(books.begin() + i);
				cout << "Book removed\n";
				return;
			}
		}
	}

	void searchBook(string title) {
		for (auto& book : books) {
			if (book.getTitle() == title) {
				cout << "Found: " << book.getTitle()
					<< " | Status: " << book.getStatusString() << endl;
				return;
			}
		}
		cout << "Book not found\n";
	}

	void borrowBook(Member& member, int bookId) {
		for (int i = 0; i < books.size(); i++) {
			if (books[i].getID() == bookId &&
				books[i].getStatus() == Available) {

				if (!member.canBorrow(borrowLimit)) {
					cout << "Borrow limit reached\n";
					return;
				}

				books[i].setStatus(Borrowed);
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
				book.setStatus(Available);
				member.returnBook(bookId);
				cout << "Book returned successfully.\n";
				return;
			}
		}
	}

	void reserveBook(Member& member, int bookId) {
		for (int i = 0; i < books.size(); i++) {
			if (books[i].getID() == bookId &&
				books[i].getStatus() == Borrowed) {

				books[i].setStatus(Reserved);
				member.reserveBook(bookId);
				cout << "Book reserved (expires in 3 days) \n";
				return;
			}
		}
		cout << "Reservation failed\n";
	}

	void generateOverdueReport(Member& member) {
		member.checkOverdue();
	}
};

//Librarian class below:
class Librarian : public User {
public:
	Librarian(int i, string n, string e, string p)
		: User(i, n, e, p) {
	}

	void showMenu() override {
		cout << "Librarian Menu: Add | Remove | View Reports\n";
	}

	void addBook() {
		cout << "Book added\n";
	}
	void removeBook() {
		cout << "Removed Book\n";
	}
	void viewOverdueReport(LibrarySystem& system, Member& member) {
		system.generateOverdueReport(member);
	}
};


//Administrator class below:
class Administrator : public User {
private:
	int borrowLimit;
	double latePenalty;

public:
	Administrator(int i, string n, string e, string p)
		: User(i, n, e, p),
		borrowLimit(MAX_BORROW),
		latePenalty(2.0) {
	}

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

// Main program :
int main() {
	LibrarySystem library;

	//Add books
	library.addBook(1, "A tour of C++", "Bjarne Stroustrup");
	library.addBook(2, "Object-Oriented Thought Process", "Matt A. Weisfeld");
	library.addBook(3, "Data Structures", "Mark Allen Weiss");

	//Create users
	Member m1(101, "Denis B", "denisb@gmail.com", "4321");
	Member m2(102, "Ian V", "ianv@gmail.com", "1234");

	Librarian l1(201, "Catherine Williams", "catherinew@gmail.com", "7654");
	Administrator admin(301, "Admin", "administrator@icloud.com", "9999");

	//Polymorphism, displaying the menus
	User* users[4] = { &m1, &m2,  &l1, &admin };
	cout << "=== Display User Menus ===\n";
	for (int i = 0; i < 4; i++)
		users[i]->showMenu();

	cout << "\n=== Test Borrowing Books ===\n";
	library.borrowBook(m1, 1); // should succeed
	library.borrowBook(m1, 2); // should succeed
	library.borrowBook(m1,3); //should succeed
	library.borrowBook(m1, 2); // already borrowed, should fail

	cout << "\n=== Test Borrow Limit ===\n";
	library.addBook(4,"Algorithms", "Robert Sedgewick");
	library.addBook(5,"Python Basics", "Guido van Rossum");
	library.addBook(6,"Java Fundamentals", "Herbert Schildt");
	library.borrowBook(m1, 4);
	library.borrowBook(m1, 5);
	library.borrowBook(m1, 6); // should fail due to MAX_BORROW

	cout << "\n=== Test Returning Books ===\n";
	library.returnBook(m1, 2); //return book 2
	library.returnBook(m1, 6); // now should succeed

	cout << "\n=== Test Reservations ===\n";
	library.borrowBook(m2,2); //m2 borrows book 2
	library.reserveBook(m1, 2); //m1 reserves it
	m1.incrementReservationDays(); // 1 day passing
	m1.incrementReservationDays();
	m1.incrementReservationDays(); // 3 days now passed
	m1.checkReservationDaysExpiry(); // should expire reservation

	cout << "\n=== Test Overdue Books ===\n";
	//incrememnt days to cause overdue
	for (int i = 0; i < 15; i++)
		m1.incrementDays();
	m1.checkOverdue();

	cout << "\n=== Test Librarian Overdue Report ===\n";
	l1.viewOverdueReport(library,m1); //librarian generates report

	cout << "\n=== Test Admin Rules Change ===\n";
	admin.setBorrowLimit(7);
	admin.setLatePenalty(5.0);
	admin.showRules();

	return 0;
}