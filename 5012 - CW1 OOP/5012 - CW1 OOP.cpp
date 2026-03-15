#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include <iomanip>
#include <chrono>
#include <ctime>

using namespace std;

//system rules and limits
const int MAX_BORROW = 5;  // max books a member can borrow
const int LOAN_DAYS = 14;   // number of days before a book becomes overdue
const int RESERVATION_DAYS = 3;  // reservation expiry time

//book status options
enum BookStatus {
	Available,
	Borrowed,
	Reserved
};

//Book Class below which represents a book in the library:
class Book {
private:
	int id;
	string title;
	string author;
	BookStatus status; // Available or Borrowed or Reserved

public:
	// constructor to create a new book
	Book(int i, string t, string a)
		: id(i), title(t), author(a), status(Available) {
	}
	// getter methods
	int getID() const { return id; }
	string getTitle() const { return title; }
	string getAuthor() const {return author; }
	BookStatus getStatus() const { return status; }

	//change the book status
	void setStatus(BookStatus s) { status = s; }

	// convert book status to readable text
	string getStatusString() const {
		if (status == Available) return "Available";
		if (status == Borrowed) return "Borrowed";
		return "Reserved";
	}
};

// notification class below which handles system messages to users:
class Notification {
private:
	string message;
public:
// constructor stores notification message
	Notification(string msg) : message(msg) {}
	// displays notification to user
	void send() {
		cout << "Notification: " << message << endl;
	}
};

//User Class below which is the base class for all system users:
class User {
protected:
	int id;
	string name;
	string email;
	string password;

public:
// constructor to create user
	User(int i, string n, string e, string p)
		: id(i), name(n), email(e), password(p) {
	}

	// authenticate user credentials
	bool authenticate(string inputEmail, string inputPassword) {
		return (email == inputEmail && password == inputPassword);
	}

	// getter functions
	int getId() const { return id; }
	string getName() const { return name; }

	// log in message
	virtual void login() {
		cout << "\n" + name << " logged in\n\n";
	}

	// log out message
	virtual void logout() {
		cout << "\n" + name << " logged out\n\n";
	}

	//virtual function for displaying role menu
	virtual void showMenu() = 0; //Polymorphism

	virtual ~User() {}
};

//Member Class below which inherits from user and allows borrowing and reserving books:
class Member : public User {
private:
// allows LibrarySystem to access private member data
	friend class LibrarySystem;
	
	vector<int> borrowedBooks; // Tracks books borrowed
	vector<int> borrowedDays; //Track days borrowed
	vector<int> reservedBooks; // stores reserved books
	vector<int> reservationDays; // tracks reservation duration
	vector<time_t> borrowedDates; // stores date when book is borrowed

public:
	Member(int i, string n, string e, string p)
		: User(i, n, e, p) {
	}
	// checks if member is allowed to borrow another book
	bool canBorrow(int limit) {
		return borrowedBooks.size() < limit;
	}
	// add book to borrowed list
	void borrowBook(int bookId) {
		borrowedBooks.push_back(bookId);
		borrowedDays.push_back(0);
		borrowedDates.push_back(time(0));
	}
	// remove returned book
	void returnBook(int bookId) {
		bool found = false;
		for (int i = 0; i < borrowedBooks.size(); i++) {
			if (borrowedBooks[i] == bookId) {

				found = true;
				borrowedBooks.erase(borrowedBooks.begin() + i);
				borrowedDays.erase(borrowedDays.begin() + i);
				borrowedDates.erase(borrowedDates.begin() + i);
				break;
			}
		}
		if (!found)
			cout << "You did not borrow this book.\n";
	}
	// add reservation
	void reserveBook(int bookId) {
		reservedBooks.push_back(bookId);
		reservationDays.push_back(0);
	}
	// simulate a day passing for borrowed books
	void incrementDays() {
		for (int i = 0; i < borrowedDays.size(); i++) {
			borrowedDays[i]++;
		}
	}
	// simulate a day passing for reservations
	void incrementReservationDays() {
		for (int i = 0; i < reservationDays.size(); i++) {
			reservationDays[i]++;
		}
	}
	// check is books are overdue and send notifications
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
	// display menu for member
	void showMenu() override {
		cout << "Member Menu: Search | Borrow | Return | Reserve | Logout\n";
	}
};

//Library system class below which manages books, users and borrowing logic:
class LibrarySystem {
private:
	vector<Book> books; // stores all books
	vector<User*> users; // stores all users

	int borrowLimit; // max number of books a member can borrow
	double latePenalty; // fine per overdue day

public:
// constructor sets deafult rules
	LibrarySystem() {
		borrowLimit = 5;
		latePenalty = 2.0;
	}
	// admin functions to change system rules
	void setBorrowLimit(int limit) {
		borrowLimit = limit;
	}

	int getBorrowLimit() const {
		return borrowLimit;
	}

	void setLatePenalty(double penalty) {
		latePenalty = penalty;
	}
	// add user to system
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
	// add new book
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
		cout << "Librarian Menu: Add | Remove | View Reports | Logout\n";
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
		cout << "Administrator Menu: Set Borrow Limit | Set Late Penalty | View Users | Logout\n";
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

	// Add some books
	library.addBook(1, "A Tour of C++", "Bjarne Stroustrup");
	library.addBook(2, "Object-Oriented Thought Process", "Matt Weisfeld");
	library.addBook(3, "Data Structures", "Mark Allen Weiss");

	// Create the users
	Member m1(101, "Denis B", "denisb", "4321");
	Librarian l1(201, "Ian V", "ianv", "1234");
	Administrator admin(301, "Admin", "admin", "9999");

	//Add users to the system
	library.addUser(&m1);
	library.addUser(&l1);
	library.addUser(&admin);

	int roleChoice;

	do {

		cout << "\n===== Library System Login =====\n";
		cout << "\n1. Member\n";
		cout << "2. Librarian\n";
		cout << "3. Administrator\n";
		cout << "0. Exit System\n";
		cout << "\nSelect role: ";	
		cin >> roleChoice;
		// member
		if (roleChoice == 1) {

			string email, password;
			cout << "Enter Email: ";
			cin >> email;

			cout << "Enter Password: ";
			cin >> password;

			if (m1.authenticate(email, password)) {

				m1.login();
				int choice;

				do {
					m1.showMenu();

					cout << "\n1 Search Book\n";
					cout << "2 Borrow Book\n";
					cout << "3 Return Book\n";
					cout << "4 Reserve Book\n";
					cout << "0 Logout\n";
					cout << "\nChoice: ";
					cin >> choice;

					if (choice == 1) {
						string query;
						cout << "Enter title or author: ";
						cin >> query;
						library.searchBook(query);
					}

					else if (choice == 2) {
						int bookId;
						cout << "Enter Book ID: ";
						cin >> bookId;
						library.borrowBook(m1, bookId);
						for (int i = 0; i < 15; i++)
							m1.incrementDays();
						m1.checkOverdue();
					}

					else if (choice == 3) {
						int bookId;
						cout << "Enter Book ID: ";
						cin >> bookId;
						library.returnBook(m1, bookId);
					}

					else if (choice == 4) {
						int bookId;
						cout << "Enter Book ID: ";
						cin >> bookId;
						library.reserveBook(m1, bookId);
						for (int i = 0; i < 15; i++)
							m1.incrementReservationDays();
							library.checkReservationExpiry(m1);
					}

				} while (choice != 0);
				m1.logout();
			}
		}
		// librarian
		else if (roleChoice == 2) {

			string email, password;
			cout << "Enter Email: ";
			cin >> email;

			cout << "Enter Password: ";
			cin >> password;
			if (l1.authenticate(email, password)) {

				l1.login();
				int choice;

				do {
					l1.showMenu();
					cout << "\n1 Add Book\n";
					cout << "2 Remove Book\n";
					cout << "3 View Overdue Report\n";
					cout << "0 Logout\n";
					cout << "\nChoice: ";
					cin >> choice;

					if (choice == 1) {
						int id;
						string title, author;
						cout << "Enter ID: ";
						cin >> id;
						cout << "Enter Title: ";
						cin.ignore();
						getline(cin, title);
						cout << "Enter Author: ";
						getline(cin, author);
						l1.addBook(library, id, title, author);
					}

					else if (choice == 2) {
						int id;
						cout << "Enter Book ID: ";
						cin >> id;
						l1.removeBook(library, id);
					}

					else if (choice == 3) {
						l1.viewOverdueReport(library, m1);
					}

				} while (choice != 0);
				l1.logout();
			}
		}
		//admin
		else if (roleChoice == 3) {

			string email, password;
			cout << "Enter Email: ";
			cin >> email;

			cout << "Enter Password: ";
			cin >> password;
			if (admin.authenticate(email, password)) {

				admin.login();
				int choice;

				do {
					admin.showMenu();
					cout << "\n1 Set Borrow Limit\n";
					cout << "2 Set Late Penalty\n";
					cout << "3 View Users\n";
					cout << "4 Add User\n";
					cout << "5 Remove User\n";
					cout << "0 Logout\n";
					cout << "\nChoice: ";
					cin >> choice;

					if (choice == 1) {
						int limit;
						cout << "Enter new limit: ";
						cin >> limit;
						admin.setBorrowLimit(library, limit);
					}

					else if (choice == 2) {
						double penalty;
						cout << "Enter penalty: ";
						cin >> penalty;
						admin.setLatePenalty(library, penalty);
					}

					else if (choice == 3) {
						admin.viewUsers(library);
					}

					else if (choice == 4) {

						int id;
						string name, email, password;
						int role;

						cout << "Enter User ID: ";
						cin >> id;

						cout << "Enter Name: ";
						cin.ignore();
						getline(cin, name);

						cout << "Enter Email: ";
						getline(cin, email);

						cout << "Enter Password: ";
						getline(cin, password);

						cout << "Select Role (1 = Member, 2 = Librarian): ";
						cin >> role;

						if (role == 1) {
							Member* newMember = new Member(id, name, email, password);
							admin.addUser(library, newMember);
							cout << "Member added successfully.\n";
						}

						else if (role == 2) {
							Librarian* newLibrarian = new Librarian(id, name, email, password);
							admin.addUser(library, newLibrarian);
							cout << "Librarian added successfully.\n";
						}

						else {
							cout << "Invalid role.\n";
						}
					}

					else if (choice == 5) {

						int userId;
						cout << "Enter User ID to remove: ";
						cin >> userId;

						admin.removeUser(library, userId);
						cout << "User removed if ID existed.\n";
					}

				} while (choice != 0);
				admin.logout();
			}
		}
	} while (roleChoice != 0);

	cout << "\nSystem closed.\n";
	return 0;
	}