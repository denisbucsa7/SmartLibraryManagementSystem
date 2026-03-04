#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include <iomanip>
#include <chrono>
#include <ctime>

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
	string getAuthor() const {return author; }
	BookStatus getStatus() const { return status; }

	void setStatus(BookStatus s) { status = s; }

	string getStatusString() const {
		if (status == Available) return "Available";
		if (status == Borrowed) return "Borrowed";
		return "Reserved";
	}
};

class Notification {
private:
	string message;
public:
	Notification(string msg) : message(msg) {}
	void send() {
		cout << "Notification: " << message << endl;
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

	bool authenticate(string inputEmail, string inputPassword) {
		return (email == inputEmail && password == inputPassword);
	}

	int getId() const { return id; }
	string getName() const { return name; }

	virtual void login() {
		cout << "\n" + name << " logged in\n";
	}

	virtual void logout() {
		cout << "\n" + name << " logged out\n";
	}

	virtual void showMenu() = 0; //Polymorphism

	virtual ~User() {}
};

//Member Class below:
class Member : public User {
private:
	friend class LibrarySystem;
	
	vector<int> borrowedBooks; // Tracks books borrowed
	vector<int> borrowedDays; //Track days borrowed
	vector<int> reservedBooks; 
	vector<int> reservationDays;
	vector<time_t> borrowedDates; 

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
		borrowedDates.push_back(time(0));
	}
	void returnBook(int bookId) {
		for (int i = 0; i < borrowedBooks.size(); i++) {
			if (borrowedBooks[i] == bookId) {
				borrowedBooks.erase(borrowedBooks.begin() + i);
				borrowedDays.erase(borrowedDays.begin() + i);
				borrowedDates.erase(borrowedDates.begin() + i);
				break;
			}
		}
	}
	void reserveBook(int bookId) {
		reservedBooks.push_back(bookId);
		reservationDays.push_back(0);
	}
	void incrementDays() {
		for (int i = 0; i < borrowedDays.size(); i++) {
			borrowedDays[i]++;
		}
	}
	void incrementReservationDays() {
		for (int i = 0; i < reservationDays.size(); i++) {
			reservationDays[i]++;
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
				Notification n ("Book ID " + to_string(borrowedBooks[i]) + " is overdue.");
				n.send();
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
	vector<User*> users;
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

	void setLatePenalty(double penalty) {
		latePenalty = penalty;
	}

	void addUser(User* user) {
		users.push_back(user);
	}

	void remove_user(int userId) {
		for (int i = 0; i < users.size(); i++) {
			if (users[i]->getId() == userId) {
				users.erase(users.begin() + i);
			}
		}
	}

	void listUsers() {
		for (auto user : users) {
			cout << user->getName() << endl;
		}
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

	void searchBook(const string& query) {
		for (auto& book : books) {
			if (book.getTitle() == query || book.getAuthor() == query) {
				cout << "Found: " << book.getTitle()
					 << " by " << book.getAuthor()
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

				time_t borrowTime = member.borrowedDates.back();
				time_t dueTime = borrowTime + LOAN_DAYS * 24*60*60;

				char dueDateStr[26];
				ctime_s(dueDateStr, sizeof(dueDateStr), & dueTime);

				cout << "Book borrowed successfully. Due Date: " << dueDateStr;
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

	void checkReservationExpiry(Member& member) {
		for (int i = 0; i < member.reservedBooks.size(); i++) {
			if (member.reservationDays[i] >= RESERVATION_DAYS) {
				int expiredBookId = member.reservedBooks[i];

				// set book to available
				for (auto& book : books) {
					if (book.getID() == expiredBookId) {
						book.setStatus(Available);
						break;
					}
				}
				Notification n ("Reservation expired for Book ID " + to_string(expiredBookId));
				n.send();

				// remove reservation
				member.reservedBooks.erase(member.reservedBooks.begin() + i);
				member.reservationDays.erase(member.reservationDays.begin() + i);

				i --;
			}
		}
	}

	void generateOverdueReport(Member& member) {
		for (int i = 0; i < member.borrowedDays.size(); i++) {

			if (member.borrowedDays[i] > LOAN_DAYS) {

				int overdueDays = member.borrowedDays[i] - LOAN_DAYS;
				double fine = overdueDays * latePenalty;

				std::ostringstream oss;
				oss << "Book ID: " << member.borrowedBooks[i]
					<< " is overdue. Fine: GBP " << std:: fixed << std::setprecision(2) << fine;

				Notification n(oss.str());
				n.send();
			}
		}
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

	void addBook(LibrarySystem& system, int id, string title, string author) {
		system.addBook(id, title, author);
	}

	void removeBook(LibrarySystem& system, int id) {
		system.removeBook(id);
	}

	void viewOverdueReport(LibrarySystem& system, Member& member) {
		system.generateOverdueReport(member);
	}
};

//Administrator class below:
class Administrator : public User {
public:
	Administrator(int i, string n, string e, string p)
		: User(i, n, e, p) {

	}

	void setBorrowLimit(LibrarySystem& system, int limit) {
		system.setBorrowLimit(limit);
	}

	void setLatePenalty(LibrarySystem& system, double penalty) {
		system.setLatePenalty(penalty);
	}

	void showRules(LibrarySystem& system) {
		cout << "Borrow Limit:" << system.getBorrowLimit() << endl;
	}

	void showMenu() override {
		cout << "Administrator Menu: Manage Users | Set Rules\n";
	}

	void addUser(LibrarySystem& system, User* user) {
		system.addUser(user);
	}

	void removeUser(LibrarySystem& system, int userId) {
		system.remove_user(userId);
	}

	void viewUsers(LibrarySystem& system) {
		system.listUsers();
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

	//add users to system
	admin.addUser(library, &m1);
	admin.addUser(library, &m2);
	admin.addUser(library, &l1);
	admin.addUser(library, &admin);

	cout << "\n=== Admin Viewing Users ===\n";
	admin.viewUsers(library);
	
	//Polymorphism, displaying the menus
	User* users[4] = { &m1, &m2,  &l1, &admin };
	cout << "\n=== Display User Menus ===\n";
	for (int i = 0; i < 4; i++)
		users[i]->showMenu();

	//authenticate denisb user logging in
	if (m1.authenticate("denisb@gmail.com", "4321")) {
	m1.login(); 
	}

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
	library.checkReservationExpiry(m1); // should expire reservation

	cout << "\n=== Test Overdue Books ===\n";
	//incrememnt days to cause overdue
	for (int i = 0; i < 15; i++)
		m1.incrementDays();
	library.generateOverdueReport(m1);

	cout << "\n=== Test Librarian Overdue Report ===\n";
	l1.viewOverdueReport(library,m1); //librarian generates report

	cout << "\n=== Test Admin Rules Change ===\n";
	admin.setBorrowLimit(library, 7);
	admin.setLatePenalty(library, 4.0);
	admin.showRules(library);

	return 0;
}