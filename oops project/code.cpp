#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <ctime>
#include <iomanip>
#include <limits>

using namespace std;
// Maximum sizes (adjust if needed)
const int MAX_STUDENTS = 200;
const int MAX_SUBJECTS = 50;
const int MAX_ATT_FILES = 500;

// =========================== UTILS ===========================
string currentDate() {
    time_t now = time(0);
    tm *ltm = localtime(&now);
    stringstream ss;
    ss << (1900 + ltm->tm_year) << "-"
       << setw(2) << setfill('0') << (1 + ltm->tm_mon) << "-"
       << setw(2) << setfill('0') << ltm->tm_mday;
    return ss.str();
}

void pause() {
    cout << "\nPress Enter to continue...";
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    cin.get();
}

// =========================== STUDENT ===========================
class Student {
public:
    string roll, name;

    Student() {}
    Student(string r, string n) : roll(r), name(n) {}
};

// =========================== SUBJECT ===========================
class Subject {
public:
    string name;

    Subject() {}
    Subject(string n) : name(n) {}
};

// =========================== ADMIN ===========================
class Admin {
private:
    string username, password;

public:
    Admin() {
        load();
    }

    void load() {
        ifstream fin("admin.txt");
        if (!fin) {
            username = "admin";
            password = "admin123";
            save();
            return;
        }
        getline(fin, username);
        getline(fin, password);
    }

    void save() {
        ofstream fout("admin.txt");
        fout << username << "\n" << password;
    }

    bool login() {
        system("cls");
            cout << "======================= ADMIN LOGIN ========================\n";
        string u, p;
        cout << "Username: ";
        cin >> u;
        cout << "Password: ";
        cin >> p;
        return (u == username && p == password);
    }

    void changeCredentials() {
        cout << "Enter new username: ";
        cin >> username;
        cout << "Enter new password: ";
        cin >> password;
        save();
        cout << "Credentials updated.\n";
        pause();
    }
};

// =========================== DATABASE ===========================
class Database {
public:
    int studentCount;
    Student students[MAX_STUDENTS];

    int subjectCount;
    Subject subjects[MAX_SUBJECTS];

    int attendanceFileCount;
    string attendanceFiles[MAX_ATT_FILES];

    Database() : studentCount(0), subjectCount(0), attendanceFileCount(0) {}

    void loadStudents() {
        studentCount = 0;
        ifstream fin("students.csv");
        string line;
        while (getline(fin, line) && studentCount < MAX_STUDENTS) {
            stringstream ss(line);
            string roll, name;
            getline(ss, roll, ',');
            getline(ss, name);
            if (!roll.empty()) {
                students[studentCount++] = Student(roll, name);
            }
        }
    }

    void loadSubjects() {
        subjectCount = 0;
        ifstream fin("subjects.txt");
        string sub;
        while (getline(fin, sub) && subjectCount < MAX_SUBJECTS) {
            if (!sub.empty()) {
                subjects[subjectCount++] = Subject(sub);
            }
        }
    }

    void loadAttendanceFiles() {
        attendanceFileCount = 0;
        ifstream fin("attendance_files.txt");
        string f;
        while (getline(fin, f) && attendanceFileCount < MAX_ATT_FILES) {
            attendanceFiles[attendanceFileCount++] = f;
        }
    }

    void addStudents() {
        system("cls");
            cout << "========================== ADD STUDENTS =========================\n";
        int n;
        cout << "Number of students: ";
        cin >> n;
        cin.ignore();

        ofstream fout("students.csv", ios::app);

        for (int i = 0; i < n; i++) {
            string roll, name;
            cout << "Roll: ";
            getline(cin, roll);
            cout << "Name: ";
            getline(cin, name);
            fout << roll << "," << name << "\n";
        }
        cout << "Students added.\n";
        pause();
    }

    void addSubjects() {
        system("cls");
            cout << "======================== ADD SUBJECTS =========================\n";
        int n;
        cout << "Number of subjects: ";
        cin >> n;
        cin.ignore();

        ofstream fout("subjects.txt", ios::app);

        for (int i = 0; i < n; i++) {
            string s;
            cout << "Subject " << i + 1 << ": ";
            getline(cin, s);
            fout << s << "\n";
        }
        cout << "Subjects added.\n";
        pause();
    }

    void addAttendanceFile(string filename) {
        ofstream fout("attendance_files.txt", ios::app);
        fout << filename << "\n";
    }
};

// =================== ATTENDANCE RECORD (1 FILE) ===================
class AttendanceRecord {
public:
    string filename;
    string subject;

    struct Entry {
        string roll;
        char status;
    };

    int entryCount;
    Entry *entries;

    AttendanceRecord(string file, string sub) : filename(file), subject(sub), entryCount(0) {
        entries = new Entry[MAX_STUDENTS];
    }

    ~AttendanceRecord() {
        delete[] entries;
    }

    void addEntry(const string &roll, char status) {
        if (entryCount < MAX_STUDENTS) {
            entries[entryCount].roll = roll;
            entries[entryCount].status = status;
            entryCount++;
        }
    }

    void save() {
        ofstream fout(filename.c_str());
        for (int i = 0; i < entryCount; i++) {
            fout << entries[i].roll << "," << entries[i].status << "\n";
        }
    }
};

// =========================== ATTENDANCE MANAGER ===========================
class AttendanceManager {
private:
    Database db;

public:
    void takeAttendance() {
        system("cls");
            cout << "======================= TAKE ATTENDANCE ===========================\n";
        db.loadSubjects();
        db.loadStudents();

        if (db.subjectCount == 0 || db.studentCount == 0) {
            cout << "Add subjects/students first.\n";
            pause();
            return;
        }

        cout << "\nSubjects:\n";
        for (int i = 0; i < db.subjectCount; i++)
            cout << i + 1 << ". " << db.subjects[i].name << "\n";

        int choice;
        cout << "Choose subject: ";
        cin >> choice;
        cin.ignore();

        string subject = db.subjects[choice - 1].name;
        string file = "att_" + subject + "_" + currentDate() + ".csv";

        AttendanceRecord record(file, subject);

        cout << "\nEnter P/A:\n";

        for (int i = 0; i < db.studentCount; i++) {
            char st;
            cout << db.students[i].roll << " " << db.students[i].name << ": ";
            cin >> st;
            st = toupper(st);
            record.addEntry(db.students[i].roll, st);
        }

        record.save();
        db.addAttendanceFile(file);

        cout << "Attendance saved in " << file << "\n";
        pause();
    }

    void showPercentages() {
            system("cls");
            cout << "========================== SHOW PERCENTAGE ==========================\n";
        db.loadStudents();
        db.loadSubjects();
        db.loadAttendanceFiles();

        if (db.attendanceFileCount == 0) {
            cout << "No attendance files.\n";
            pause();
            return;
        }

        int S = db.studentCount;
        int C = db.subjectCount;

        int present[MAX_STUDENTS][MAX_SUBJECTS] = {0};
        int total[MAX_STUDENTS][MAX_SUBJECTS] = {0};

        for (int f = 0; f < db.attendanceFileCount; f++) {
            ifstream fin(db.attendanceFiles[f].c_str());
            string line;

            int subjIndex = -1;
            for (int j = 0; j < C; j++) {
                if (db.attendanceFiles[f].find(db.subjects[j].name) != string::npos)
                    subjIndex = j;
            }
            if (subjIndex == -1) continue;

            while (getline(fin, line)) {
                string roll, status;
                stringstream ss(line);
                getline(ss, roll, ',');
                getline(ss, status);

                for (int i = 0; i < S; i++) {
                    if (db.students[i].roll == roll) {
                        total[i][subjIndex]++;
                        if (status == "P") present[i][subjIndex]++;
                    }
                }
            }
        }

        cout << "\nRoll\tName\t";
        for (int i = 0; i < C; i++) cout << db.subjects[i].name << "\t";
        cout << "Overall\n";

        for (int i = 0; i < S; i++) {
            cout << db.students[i].roll << "\t" << db.students[i].name << "\t";

            double overallP = 0, overallT = 0;

            for (int j = 0; j < C; j++) {
                double perc = (total[i][j] == 0) ? 0 : present[i][j] * 100.0 / total[i][j];
                overallP += present[i][j];
                overallT += total[i][j];
                cout << fixed << setprecision(1) << perc << "%\t";
            }

            double all = (overallT == 0) ? 0 : overallP * 100.0 / overallT;
            cout << fixed << setprecision(1) << all << "%\n";
        }
        pause();
    }
};

// =========================== MAIN APPLICATION ===========================
class App {
private:
    Admin admin;
    Database db;
    AttendanceManager attendance;

public:
    void mainMenu() {
        while (true) {
            system("cls");
            cout << "=================== SRM ATTENDANCE SYSTEM ===========================\n";
            cout << "1. Login as Admin\n";
            cout << "0. Exit\n";
            cout << "Choice: ";

            int ch;
            cin >> ch;

            if (ch == 1) {
                if (admin.login())
                    adminMenu();
                else {
                    cout << "Invalid credentials!\n";
                    pause();
                }
            } else if (ch == 0) {
                cout << "Bye!\n";
                break;
            } else {
                cout << "Invalid choice.\n";
            }
        }
    }

    void adminMenu() {
        while (true) {
            system("cls");
            cout << "======================= ADMIN MENU ===========================\n";
            cout << "1. Add Students\n";
            cout << "2. Add Subjects\n";
            cout << "3. Take Student Attendance\n";
            cout << "4. Show  Student Attendance %\n";
            cout << "5. Change  Admin Credentials\n";
            cout << "0. Logout\n";
            cout << "Choice: ";

            int ch;
            cin >> ch;

            switch (ch) {
                case 1: db.addStudents(); break;
                case 2: db.addSubjects(); break;
                case 3: attendance.takeAttendance(); break;
                case 4: attendance.showPercentages(); break;
                case 5: admin.changeCredentials(); break;
                case 0: return;
                default: cout << "Invalid!\n";
            }
        }
    }
};

// =========================== MAIN ===========================
int main() {
    App app;
    app.mainMenu();
    return 0;
}
