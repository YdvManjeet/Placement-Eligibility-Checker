#include <iostream>
#include <vector>
#include <algorithm>
#include <fstream>
#include <iomanip>
#include <limits>
#ifdef _WIN32
#include <windows.h>
#else
#include <unistd.h>
#endif

using namespace std;

// ANSI Color Codes
const string RED = "\033[31m";
const string GREEN = "\033[32m";
const string YELLOW = "\033[33m";
const string BLUE = "\033[34m";
const string MAGENTA = "\033[35m";
const string CYAN = "\033[36m";
const string RESET = "\033[0m";
const string BOLD = "\033[1m";

// ASCII-safe symbols
const string OK = "[OK]";
const string X = "[X]";
const string BULLET = "-";

// Input validation constants
const float MIN_CGPA = 0.0;
const float MAX_CGPA = 10.0;
const int MIN_BACKLOGS = 0;
const int MAX_BACKLOGS = 10;
const int MIN_PROJECT_SCORE = 0;
const int MAX_PROJECT_SCORE = 10;

// Helper function for input validation
int getValidIntInput(const string &prompt, int min, int max)
{
    int value;
    while (true)
    {
        cout << prompt;
        if (cin >> value && value >= min && value <= max)
        {
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            return value;
        }
        cout << RED << "Invalid input! Please enter a number between " << min << " and " << max << ".\n"
             << RESET;
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
    }
}

float getValidFloatInput(const string &prompt, float min, float max)
{
    float value;
    while (true)
    {
        cout << prompt;
        if (cin >> value && value >= min && value <= max)
        {
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            return value;
        }
        cout << RED << "Invalid input! Please enter a number between " << min << " and " << max << ".\n"
             << RESET;
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
    }
}

// Forward Declarations
class Project;
class Student;
class Teacher;
class PlacementSystem;

// Project Class
class Project
{
private:
    string title;
    string description;
    int qualityScore;

public:
    Project(string t = "", string d = "", int qs = 0)
        : title(t), description(d), qualityScore(qs) {}

    // Getters
    string getTitle() const { return title; }
    string getDescription() const { return description; }
    int getQualityScore() const { return qualityScore; }

    // Setters
    void setTitle(const string &t) { title = t; }
    void setDescription(const string &d) { description = d; }
    void setQualityScore(int qs) { qualityScore = qs; }

    void display() const
    {
        cout << CYAN << "Title: " << RESET << title << "\n";
        cout << CYAN << "Description: " << RESET << description << "\n";
        cout << CYAN << "Quality Score: " << RESET << qualityScore << "/10\n";
    }
};

// Student Class
class Student
{
private:
    string name;
    string rollNo;
    string password;
    float cgpa;
    int backlogs;
    vector<string> skills;
    vector<Project> projects;
    vector<string> offers;

public:
    Student(string n = "", string r = "", string p = "", float c = 0.0, int b = 0)
        : name(n), rollNo(r), password(p), cgpa(c), backlogs(b) {}

    // Getters
    string getName() const { return name; }
    string getRollNo() const { return rollNo; }
    string getPassword() const { return password; }
    float getCGPA() const { return cgpa; }
    int getBacklogs() const { return backlogs; }
    const vector<string> &getSkills() const { return skills; }
    const vector<Project> &getProjects() const { return projects; }
    const vector<string> &getOffers() const { return offers; }

    // Setters
    void setName(const string &n) { name = n; }
    void setRollNo(const string &r) { rollNo = r; }
    void setPassword(const string &p) { password = p; }
    void setCGPA(float c) { cgpa = c; }
    void setBacklogs(int b) { backlogs = b; }

    // Methods
    void addSkill(const string &skill) { skills.push_back(skill); }
    void addProject(const Project &project) { projects.push_back(project); }
    void addOffer(const string &offer) { offers.push_back(offer); }

    // Remove methods
    bool removeSkill(const string &skill)
    {
        auto it = find(skills.begin(), skills.end(), skill);
        if (it != skills.end())
        {
            skills.erase(it);
            return true;
        }
        return false;
    }

    bool removeProject(const string &title)
    {
        auto it = find_if(projects.begin(), projects.end(),
                          [&title](const Project &p)
                          { return p.getTitle() == title; });
        if (it != projects.end())
        {
            projects.erase(it);
            return true;
        }
        return false;
    }

    bool removeOffer(const string &offer)
    {
        auto it = find(offers.begin(), offers.end(), offer);
        if (it != offers.end())
        {
            offers.erase(it);
            return true;
        }
        return false;
    }

    struct EligibilityStatus
    {
        bool isEligible;
        string reason;
        string strongProjectTitle;
        int strongProjectScore;
    };

    EligibilityStatus checkEligibility() const
    {
        EligibilityStatus status;
        status.isEligible = false;
        status.reason = "";
        status.strongProjectTitle = "";
        status.strongProjectScore = 0;

        // Check for at least one skill (mandatory)
        if (skills.empty())
        {
            status.reason = "Not Eligible: No skills listed";
            return status;
        }

        // Find strongest project
        for (const Project &project : projects)
        {
            if (project.getQualityScore() >= 8 && project.getQualityScore() > status.strongProjectScore)
            {
                status.strongProjectTitle = project.getTitle();
                status.strongProjectScore = project.getQualityScore();
            }
        }

        // Check eligibility criteria
        if (cgpa >= 7.0 && backlogs == 0)
        {
            status.isEligible = true;
            status.reason = "Eligible: CGPA >= 7.0 and No Backlogs";
        }
        else if (status.strongProjectScore >= 8 && backlogs == 0)
        {
            status.isEligible = true;
            status.reason = "Eligible via Strong Project: " + status.strongProjectTitle +
                            " (Score: " + to_string(status.strongProjectScore) + ")";
        }
        else if (cgpa < 7.0)
        {
            status.reason = "Not Eligible: Low CGPA (" + to_string(cgpa) + ")";
        }
        else if (backlogs > 0)
        {
            status.reason = "Not Eligible: Has Backlogs (" + to_string(backlogs) + ")";
        }

        return status;
    }

    bool isEligible() const
    {
        return checkEligibility().isEligible;
    }

    void display() const
    {
        cout << "\n"
             << BOLD << BLUE << "Student Information" << RESET << "\n";
        cout << "Name: " << name << "\n";
        cout << "Roll No: " << rollNo << "\n";
        cout << "CGPA: " << fixed << setprecision(2) << cgpa << "\n";
        cout << "Backlogs: " << backlogs << "\n";

        cout << "\nSkills:\n";
        if (skills.empty())
        {
            cout << "  No skills listed\n";
        }
        else
        {
            for (const string &skill : skills)
            {
                cout << "  " << BULLET << " " << skill << "\n";
            }
        }

        cout << "\nProjects:\n";
        if (projects.empty())
        {
            cout << "  No projects listed\n";
        }
        else
        {
            for (size_t i = 0; i < projects.size(); i++)
            {
                cout << "\n  Project " << (i + 1) << ":\n";
                projects[i].display();
            }
        }

        // Only show offers if eligible
        if (isEligible())
        {
            cout << "\nOffers:\n";
            if (offers.empty())
            {
                cout << "  No offers received yet\n";
            }
            else
            {
                for (const string &offer : offers)
                {
                    cout << "  " << BULLET << " " << offer << "\n";
                }
            }
        }
        else
        {
            cout << "\n"
                 << RED << "Note: You are currently ineligible for placement.\n"
                 << RESET;
            if (!offers.empty())
            {
                cout << YELLOW << "Previous offers (received before ineligibility):\n"
                     << RESET;
                for (const string &offer : offers)
                {
                    cout << "  " << BULLET << " " << offer << "\n";
                }
            }
        }
    }
};

// Teacher Class
class Teacher
{
private:
    string username;
    string password;

public:
    Teacher(string u = "admin", string p = "admin123")
        : username(u), password(p) {}

    bool authenticate(const string &u, const string &p) const
    {
        return username == u && password == p;
    }
};

// Comparison functions for sorting
bool compareByCGPA(const Student &a, const Student &b)
{
    return a.getCGPA() > b.getCGPA();
}

bool compareByProjectQuality(const Student &a, const Student &b)
{
    float aScore = 0, bScore = 0;
    for (size_t i = 0; i < a.getProjects().size(); i++)
    {
        aScore += a.getProjects()[i].getQualityScore();
    }
    for (size_t i = 0; i < b.getProjects().size(); i++)
    {
        bScore += b.getProjects()[i].getQualityScore();
    }
    return aScore > bScore;
}

// Placement System Class
class PlacementSystem
{
private:
    vector<Student> students;
    Teacher teacher;
    Student *currentStudent;

    void clearScreen()
    {
#ifdef _WIN32
        system("cls");
#else
        system("clear");
#endif
    }

    void pauseScreen()
    {
        cout << "\n"
             << BOLD << YELLOW << "Press Enter to continue..." << RESET;
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cin.get();
    }

    void printHeader(const string &title)
    {
        clearScreen();
        cout << BOLD << BLUE << "\n==========================================\n";
        cout << "           " << title << "\n";
        cout << "==========================================\n"
             << RESET;
    }

    void printDivider()
    {
        cout << BOLD << BLUE << "\n------------------------------------------\n"
             << RESET;
    }

    void loadingAnimation()
    {
        cout << BOLD << YELLOW << "\nLoading";
        for (int i = 0; i < 3; i++)
        {
            cout << ".";
            cout.flush();
#ifdef _WIN32
            Sleep(500);
#else
            usleep(500000);
#endif
        }
        cout << RESET << "\n";
    }

public:
    PlacementSystem() : currentStudent(NULL)
    {
        addSampleData();
    }

    void addSampleData()
    {
        // Sample Student 1
        Student s1("Manjeet Yadav", "24CSU121", "121", 2, 0);
        s1.addSkill("C++");
        s1.addSkill("Python");
        s1.addSkill("Data Structures");
        s1.addProject(Project("Web App", "Full-stack application", 8));
        s1.addOffer("Google");
        s1.addOffer("Microsoft");
        students.push_back(s1);

        // Sample Student 2
        Student s2("Nikhil Yadav", "24CSU144", "144", 7.8, 1);
        s2.addSkill("Java");
        s2.addSkill("Spring Boot");
        s2.addSkill("SQL");
        s2.addProject(Project("Mobile App", "Android application", 8));
        s2.addOffer("Amazon");
        students.push_back(s2);

        // Sample Student 3
        Student s3("Krish Sharma", "24CSU104", "104", 6.5, 2);
        s3.addSkill("C++");
        s3.addSkill("React");
        s3.addSkill("Fullstack");
        s3.addProject(Project("AI Project", "Machine Learning application", 9));
        students.push_back(s3);

        // Sample Student 4
        Student s4("Lakshay Kataria", "24CSU109", "109", 7, 1);
        s4.addSkill("JavaScript");
        s4.addSkill("React");
        s4.addSkill("Node.js");
        s4.addProject(Project("AI Project", "Machine Learning application", 9));
        students.push_back(s4);

        // Sample Student 5
        Student s5("Lakshay Yadav", "24CSU107", "107", 5.5, 4);
        s5.addSkill("C--");
        s5.addSkill("Html");
        s5.addSkill("Wordpress");
        s5.addProject(Project("AI Project", "Machine Learning application", 9));
        students.push_back(s5);
    }

    void run()
    {
        while (true)
        {
            printHeader("Placement Eligibility Checker");
            cout << BOLD << GREEN << "1. " << RESET << "Teacher Login\n";
            cout << BOLD << GREEN << "2. " << RESET << "Student Login\n";
            cout << BOLD << RED << "3. " << RESET << "Exit\n";
            cout << "\nEnter your choice: ";

            int choice;
            if (!(cin >> choice))
            {
                cin.clear();
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                cout << RED << "Invalid input! Please enter a number.\n"
                     << RESET;
                loadingAnimation();
                continue;
            }
            cin.ignore(numeric_limits<streamsize>::max(), '\n');

            switch (choice)
            {
            case 1:
                teacherLogin();
                break;
            case 2:
                studentLogin();
                break;
            case 3:
                cout << BOLD << YELLOW << "\nThank you for using the system!\n"
                     << RESET;
                return;
            default:
                cout << RED << "Invalid choice! Please enter a number between 1 and 3.\n"
                     << RESET;
                loadingAnimation();
            }
        }
    }

    void teacherLogin()
    {
        printHeader("Teacher Login");
        string username, password;
        cout << "Username: ";
        getline(cin, username);
        cout << "Password: ";
        getline(cin, password);

        if (teacher.authenticate(username, password))
        {
            cout << GREEN << "\nLogin successful!\n"
                 << RESET;
            loadingAnimation();
            teacherMenu();
        }
        else
        {
            cout << RED << "\nInvalid credentials!\n"
                 << RESET;
            loadingAnimation();
        }
    }

    void studentLogin()
    {
        printHeader("Student Login");
        string rollNo, password;
        cout << "Roll Number: ";
        getline(cin, rollNo);
        cout << "Password: ";
        getline(cin, password);

        for (size_t i = 0; i < students.size(); i++)
        {
            if (students[i].getRollNo() == rollNo && students[i].getPassword() == password)
            {
                currentStudent = &students[i];
                cout << GREEN << "\nLogin successful!\n"
                     << RESET;
                loadingAnimation();
                studentMenu();
                return;
            }
        }
        cout << RED << "\nInvalid credentials!\n"
             << RESET;
        loadingAnimation();
    }

    void teacherMenu()
    {
        while (true)
        {
            printHeader("Teacher Menu");
            cout << BOLD << GREEN << "1. " << RESET << "View All Students\n";
            cout << BOLD << GREEN << "2. " << RESET << "View Eligible Students\n";
            cout << BOLD << GREEN << "3. " << RESET << "Sort and Display Students\n";
            cout << BOLD << GREEN << "4. " << RESET << "View Company Offers\n";
            cout << BOLD << GREEN << "5. " << RESET << "Export Eligible Students\n";
            cout << BOLD << GREEN << "6. " << RESET << "Edit Student Details\n";
            cout << BOLD << GREEN << "7. " << RESET << "Add Student\n";
            cout << BOLD << GREEN << "8. " << RESET << "Delete Student\n";
            cout << BOLD << RED << "9. " << RESET << "Logout\n";
            cout << "\nEnter your choice: ";

            int choice;
            cin >> choice;
            cin.ignore(numeric_limits<streamsize>::max(), '\n');

            switch (choice)
            {
            case 1:
                viewAllStudents();
                break;
            case 2:
                viewEligibleStudents();
                break;
            case 3:
                sortAndDisplayStudents();
                break;
            case 4:
                viewCompanyOffers();
                break;
            case 5:
                exportEligibleStudents();
                break;
            case 6:
                editStudentDetails();
                break;
            case 7:
                addStudent();
                break;
            case 8:
                deleteStudent();
                break;
            case 9:
                return;
            default:
                cout << RED << "Invalid choice! Please try again.\n"
                     << RESET;
            }
            loadingAnimation();
        }
    }

    void studentMenu()
    {
        while (true)
        {
            printHeader("Student Menu");
            cout << BOLD << GREEN << "1. " << RESET << "View Personal Data\n";
            cout << BOLD << GREEN << "2. " << RESET << "View Eligibility Status\n";
            cout << BOLD << GREEN << "3. " << RESET << "View Offers\n";
            cout << BOLD << GREEN << "4. " << RESET << "View Top Rankings\n";
            cout << BOLD << RED << "5. " << RESET << "Logout\n";
            cout << "\nEnter your choice: ";

            int choice;
            cin >> choice;
            cin.ignore(numeric_limits<streamsize>::max(), '\n');

            switch (choice)
            {
            case 1:
                currentStudent->display();
                break;
            case 2:
                viewEligibilityStatus();
                break;
            case 3:
                viewOffers();
                break;
            case 4:
                viewTopRankings();
                break;
            case 5:
                currentStudent = NULL;
                return;
            default:
                cout << RED << "Invalid choice! Please try again.\n"
                     << RESET;
            }
            cout << "\nPress Enter to continue...";
            cin.get();
        }
    }

    void viewAllStudents()
    {
        printHeader("All Students");
        if (students.empty())
        {
            cout << "\nNo students registered in the system.\n";
        }
        else
        {
            for (size_t i = 0; i < students.size(); i++)
            {
                cout << "\nStudent " << (i + 1) << ":\n";
                students[i].display();
                printDivider();
            }
        }
        pauseScreen();
    }

    void viewEligibleStudents()
    {
        printHeader("Eligible Students");
        bool found = false;
        for (size_t i = 0; i < students.size(); i++)
        {
            if (students[i].isEligible())
            {
                cout << "\nStudent " << (i + 1) << ":\n";
                students[i].display();
                printDivider();
                found = true;
            }
        }
        if (!found)
        {
            cout << "\nNo eligible students found.\n";
        }
        pauseScreen();
    }

    void sortAndDisplayStudents()
    {
        printHeader("Sort Students");
        cout << BOLD << GREEN << "1. " << RESET << "Sort by CGPA\n";
        cout << BOLD << GREEN << "2. " << RESET << "Sort by Project Quality\n";
        cout << "\nEnter your choice: ";

        int choice;
        cin >> choice;
        cin.ignore(numeric_limits<streamsize>::max(), '\n');

        vector<Student> sortedStudents = students;

        if (choice == 1)
        {
            sort(sortedStudents.begin(), sortedStudents.end(), compareByCGPA);
            printHeader("Students Sorted by CGPA");
        }
        else if (choice == 2)
        {
            sort(sortedStudents.begin(), sortedStudents.end(), compareByProjectQuality);
            printHeader("Students Sorted by Project Quality");
        }

        for (size_t i = 0; i < sortedStudents.size(); i++)
        {
            sortedStudents[i].display();
            printDivider();
        }
        pauseScreen();
    }

    void viewCompanyOffers()
    {
        printHeader("Company Offers");
        bool found = false;
        for (size_t i = 0; i < students.size(); i++)
        {
            if (!students[i].getOffers().empty())
            {
                cout << BOLD << BLUE << "Student: " << RESET << students[i].getName()
                     << " (" << students[i].getRollNo() << ")\n";
                cout << CYAN << "Offers:\n"
                     << RESET;
                const vector<string> &offers = students[i].getOffers();
                for (size_t j = 0; j < offers.size(); j++)
                {
                    cout << "  " << BULLET << " " << offers[j] << "\n";
                }
                printDivider();
                found = true;
            }
        }
        if (!found)
        {
            cout << YELLOW << "\nNo offers found.\n"
                 << RESET;
        }
        pauseScreen();
    }

    void exportEligibleStudents()
    {
        ofstream file("eligible_students.txt");
        if (file.is_open())
        {
            file << "=== Eligible Students ===\n\n";
            for (size_t i = 0; i < students.size(); i++)
            {
                if (students[i].isEligible())
                {
                    file << "Name: " << students[i].getName() << "\n";
                    file << "Roll No: " << students[i].getRollNo() << "\n";
                    file << "CGPA: " << students[i].getCGPA() << "\n";
                    file << "Backlogs: " << students[i].getBacklogs() << "\n";
                    file << "------------------------\n";
                }
            }
            file.close();
            cout << GREEN << "Data exported to eligible_students.txt\n"
                 << RESET;
        }
        else
        {
            cout << RED << "Error opening file!\n"
                 << RESET;
        }
    }

    void viewEligibilityStatus()
    {
        printHeader("Eligibility Status");
        Student::EligibilityStatus status = currentStudent->checkEligibility();

        cout << "Status: " << (status.isEligible ? GREEN : RED)
             << (status.isEligible ? "Eligible" : "Not Eligible") << RESET << "\n\n";

        cout << "Reason: " << status.reason << "\n\n";

        cout << "CGPA: " << fixed << setprecision(2) << currentStudent->getCGPA()
             << (currentStudent->getCGPA() >= 7.0 ? " " + OK : " " + X) << "\n";

        cout << "Backlogs: " << currentStudent->getBacklogs()
             << (currentStudent->getBacklogs() == 0 ? " " + OK : " " + X) << "\n";

        cout << "Skills: " << (currentStudent->getSkills().empty() ? X : OK) << "\n";

        if (status.strongProjectScore > 0)
        {
            cout << "\nStrongest Project:\n";
            cout << "Title: " << status.strongProjectTitle << "\n";
            cout << "Score: " << status.strongProjectScore << "/10\n";
        }

        pauseScreen();
    }

    void viewOffers()
    {
        printHeader("Your Offers");
        if (!currentStudent->isEligible())
        {
            cout << RED << "\nYou are currently ineligible for placement.\n"
                 << RESET;
            if (!currentStudent->getOffers().empty())
            {
                cout << YELLOW << "\nPrevious offers (received before ineligibility):\n"
                     << RESET;
                for (const string &offer : currentStudent->getOffers())
                {
                    cout << "  " << BULLET << " " << offer << "\n";
                }
            }
        }
        else
        {
            const vector<string> &offers = currentStudent->getOffers();
            if (offers.empty())
            {
                cout << "\nNo offers received yet.\n";
            }
            else
            {
                cout << "\n";
                for (size_t i = 0; i < offers.size(); i++)
                {
                    cout << (i + 1) << ". " << offers[i] << "\n";
                }
            }
        }
        pauseScreen();
    }

    void viewTopRankings()
    {
        printHeader("Top Rankings");
        vector<Student> sortedStudents = students;
        sort(sortedStudents.begin(), sortedStudents.end(), compareByCGPA);

        cout << "Top 5 Students by CGPA:\n\n";
        if (sortedStudents.empty())
        {
            cout << "No students in the system.\n";
        }
        else
        {
            for (size_t i = 0; i < std::min(static_cast<size_t>(5), sortedStudents.size()); i++)
            {
                cout << (i + 1) << ". " << sortedStudents[i].getName()
                     << " - CGPA: " << fixed << setprecision(2) << sortedStudents[i].getCGPA() << "\n";
            }
        }
        pauseScreen();
    }

    void editStudentDetails()
    {
        printHeader("Edit Student Details");
        if (students.empty())
        {
            cout << "\nNo students registered in the system.\n";
            pauseScreen();
            return;
        }

        cout << "Select a student to edit:\n";
        for (size_t i = 0; i < students.size(); i++)
        {
            cout << (i + 1) << ". " << students[i].getName() << " (" << students[i].getRollNo() << ")\n";
        }

        int choice = getValidIntInput("\nEnter student number: ", 1, students.size());
        Student &student = students[choice - 1];

        while (true)
        {
            printHeader("Edit Student: " + student.getName());
            cout << BOLD << GREEN << "1. " << RESET << "Edit Name\n";
            cout << BOLD << GREEN << "2. " << RESET << "Edit Password\n";
            cout << BOLD << GREEN << "3. " << RESET << "Edit CGPA\n";
            cout << BOLD << GREEN << "4. " << RESET << "Edit Backlogs\n";
            cout << BOLD << GREEN << "5. " << RESET << "Manage Skills\n";
            cout << BOLD << GREEN << "6. " << RESET << "Manage Projects\n";
            cout << BOLD << GREEN << "7. " << RESET << "Manage Offers\n";
            cout << BOLD << RED << "8. " << RESET << "Exit\n";
            cout << "\nEnter your choice: ";

            int editChoice = getValidIntInput("", 1, 8);

            switch (editChoice)
            {
            case 1:
            {
                string name;
                cout << "Enter new name: ";
                getline(cin, name);
                if (!name.empty())
                    student.setName(name);
                break;
            }
            case 2:
            {
                string password;
                cout << "Enter new password: ";
                getline(cin, password);
                if (!password.empty())
                    student.setPassword(password);
                break;
            }
            case 3:
            {
                float cgpa = getValidFloatInput("Enter new CGPA: ", MIN_CGPA, MAX_CGPA);
                student.setCGPA(cgpa);
                break;
            }
            case 4:
            {
                int backlogs = getValidIntInput("Enter new number of backlogs: ",
                                                MIN_BACKLOGS, MAX_BACKLOGS);
                student.setBacklogs(backlogs);
                break;
            }
            case 5:
            {
                manageSkills(student);
                break;
            }
            case 6:
            {
                manageProjects(student);
                break;
            }
            case 7:
            {
                manageOffers(student);
                break;
            }
            case 8:
                return;
            }
            cout << GREEN << "Changes saved successfully!\n"
                 << RESET;
            pauseScreen();
        }
    }

    void manageSkills(Student &student)
    {
        while (true)
        {
            printHeader("Manage Skills");
            cout << BOLD << GREEN << "1. " << RESET << "Add Skill\n";
            cout << BOLD << GREEN << "2. " << RESET << "Remove Skill\n";
            cout << BOLD << GREEN << "3. " << RESET << "View Skills\n";
            cout << BOLD << RED << "4. " << RESET << "Exit\n";
            cout << "\nEnter your choice: ";

            int choice = getValidIntInput("", 1, 4);

            switch (choice)
            {
            case 1:
            {
                string skill;
                cout << "Enter skill to add: ";
                getline(cin, skill);
                if (!skill.empty())
                    student.addSkill(skill);
                break;
            }
            case 2:
            {
                string skill;
                cout << "Enter skill to remove: ";
                getline(cin, skill);
                if (student.removeSkill(skill))
                {
                    cout << GREEN << "Skill removed successfully!\n"
                         << RESET;
                }
                else
                {
                    cout << RED << "Skill not found!\n"
                         << RESET;
                }
                break;
            }
            case 3:
            {
                cout << "\nCurrent Skills:\n";
                for (const string &skill : student.getSkills())
                {
                    cout << BULLET << " " << skill << "\n";
                }
                break;
            }
            case 4:
                return;
            }
            pauseScreen();
        }
    }

    void manageProjects(Student &student)
    {
        while (true)
        {
            printHeader("Manage Projects");
            cout << BOLD << GREEN << "1. " << RESET << "Add Project\n";
            cout << BOLD << GREEN << "2. " << RESET << "Remove Project\n";
            cout << BOLD << GREEN << "3. " << RESET << "View Projects\n";
            cout << BOLD << RED << "4. " << RESET << "Exit\n";
            cout << "\nEnter your choice: ";

            int choice = getValidIntInput("", 1, 4);

            switch (choice)
            {
            case 1:
            {
                string title, description;
                cout << "Enter project title: ";
                getline(cin, title);
                cout << "Enter project description: ";
                getline(cin, description);
                int qualityScore = getValidIntInput("Enter project quality score (0-10): ",
                                                    MIN_PROJECT_SCORE, MAX_PROJECT_SCORE);
                student.addProject(Project(title, description, qualityScore));
                break;
            }
            case 2:
            {
                string title;
                cout << "Enter project title to remove: ";
                getline(cin, title);
                if (student.removeProject(title))
                {
                    cout << GREEN << "Project removed successfully!\n"
                         << RESET;
                }
                else
                {
                    cout << RED << "Project not found!\n"
                         << RESET;
                }
                break;
            }
            case 3:
            {
                cout << "\nCurrent Projects:\n";
                for (const Project &project : student.getProjects())
                {
                    project.display();
                }
                break;
            }
            case 4:
                return;
            }
            pauseScreen();
        }
    }

    void manageOffers(Student &student)
    {
        while (true)
        {
            printHeader("Manage Offers");
            cout << BOLD << GREEN << "1. " << RESET << "Add Offer\n";
            cout << BOLD << GREEN << "2. " << RESET << "Remove Offer\n";
            cout << BOLD << GREEN << "3. " << RESET << "View Offers\n";
            cout << BOLD << RED << "4. " << RESET << "Exit\n";
            cout << "\nEnter your choice: ";

            int choice = getValidIntInput("", 1, 4);

            switch (choice)
            {
            case 1:
            {
                string offer;
                cout << "Enter offer to add: ";
                getline(cin, offer);
                if (!offer.empty())
                    student.addOffer(offer);
                break;
            }
            case 2:
            {
                string offer;
                cout << "Enter offer to remove: ";
                getline(cin, offer);
                if (student.removeOffer(offer))
                {
                    cout << GREEN << "Offer removed successfully!\n"
                         << RESET;
                }
                else
                {
                    cout << RED << "Offer not found!\n"
                         << RESET;
                }
                break;
            }
            case 3:
            {
                cout << "\nCurrent Offers:\n";
                for (const string &offer : student.getOffers())
                {
                    cout << BULLET << " " << offer << "\n";
                }
                break;
            }
            case 4:
                return;
            }
            pauseScreen();
        }
    }

    void addStudent()
    {
        printHeader("Add New Student");
        string name, rollNo, password;
        float cgpa;
        int backlogs;

        cout << "Enter student name: ";
        getline(cin, name);
        cout << "Enter roll number: ";
        getline(cin, rollNo);
        cout << "Enter password: ";
        getline(cin, password);

        cgpa = getValidFloatInput("Enter CGPA: ", MIN_CGPA, MAX_CGPA);
        backlogs = getValidIntInput("Enter number of backlogs: ", MIN_BACKLOGS, MAX_BACKLOGS);

        Student newStudent(name, rollNo, password, cgpa, backlogs);

        // Add skills
        int numSkills = getValidIntInput("Enter number of skills: ", 0, 10);
        for (int i = 0; i < numSkills; i++)
        {
            string skill;
            cout << "Enter skill " << (i + 1) << ": ";
            getline(cin, skill);
            newStudent.addSkill(skill);
        }

        // Add projects
        int numProjects = getValidIntInput("Enter number of projects: ", 0, 10);
        for (int i = 0; i < numProjects; i++)
        {
            string title, description;
            int qualityScore;

            cout << "\nProject " << (i + 1) << ":\n";
            cout << "Enter project title: ";
            getline(cin, title);
            cout << "Enter project description: ";
            getline(cin, description);
            qualityScore = getValidIntInput("Enter project quality score (0-10): ",
                                            MIN_PROJECT_SCORE, MAX_PROJECT_SCORE);

            newStudent.addProject(Project(title, description, qualityScore));
        }

        students.push_back(newStudent);
        cout << GREEN << "Student added successfully!\n"
             << RESET;
        pauseScreen();
    }

    void deleteStudent()
    {
        printHeader("Delete Student");
        if (students.empty())
        {
            cout << "\nNo students registered in the system.\n";
            pauseScreen();
            return;
        }

        cout << "Select a student to delete:\n";
        for (size_t i = 0; i < students.size(); i++)
        {
            cout << (i + 1) << ". " << students[i].getName() << " (" << students[i].getRollNo() << ")\n";
        }

        int choice;
        cout << "\nEnter student number: ";
        cin >> choice;
        cin.ignore(numeric_limits<streamsize>::max(), '\n');

        if (choice < 1 || choice > static_cast<int>(students.size()))
        {
            cout << RED << "Invalid student number!\n"
                 << RESET;
            pauseScreen();
            return;
        }

        students.erase(students.begin() + choice - 1);
        cout << GREEN << "Student deleted successfully!\n"
             << RESET;
        pauseScreen();
    }
};

int main()
{
    PlacementSystem system;
    system.run();
    return 0;
}