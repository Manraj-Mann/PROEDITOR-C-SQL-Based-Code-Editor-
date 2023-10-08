// proeditor.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <Windows.h>
#include<string>
#include <limits>
#include <ios>
#include <ctype.h>
#include <vector>
#include <conio.h>
#include <windows.h>
#include <sqlext.h>
#include <sqltypes.h>
#include <sql.h>
#include <iomanip>

// ****************************************************** SQL OPERATIONS CLASS ****************************** //

class SQLDatabase {
public:
    SQLDatabase() : SQLEnvHandle(NULL), SQLConnectionHandle(NULL), SQLStatementHandle(NULL) {}
    ~SQLDatabase() {
        Disconnect();
    }

    bool Connect(const std::string& connectionString) {
        if (SQL_SUCCESS != SQLAllocHandle(SQL_HANDLE_ENV, SQL_NULL_HANDLE, &SQLEnvHandle)) {
            ShowSQLError(SQL_HANDLE_ENV, SQLEnvHandle);
            return false;
        }

        if (SQL_SUCCESS != SQLSetEnvAttr(SQLEnvHandle, SQL_ATTR_ODBC_VERSION, (SQLPOINTER)SQL_OV_ODBC3, 0)) {
            ShowSQLError(SQL_HANDLE_ENV, SQLEnvHandle);
            return false;
        }

        if (SQL_SUCCESS != SQLAllocHandle(SQL_HANDLE_DBC, SQLEnvHandle, &SQLConnectionHandle)) {
            ShowSQLError(SQL_HANDLE_ENV, SQLEnvHandle);
            return false;
        }

        SQLCHAR retConString[1024];
        switch (SQLDriverConnect(SQLConnectionHandle, NULL, (SQLCHAR*)connectionString.c_str(), SQL_NTS, retConString, 1024, NULL, SQL_DRIVER_NOPROMPT)) {
        case SQL_SUCCESS:
            return true;
        case SQL_SUCCESS_WITH_INFO:
            return true;
        case SQL_NO_DATA_FOUND:
        case SQL_INVALID_HANDLE:
        case SQL_ERROR:
            ShowSQLError(SQL_HANDLE_DBC, SQLConnectionHandle);
            return false;
        default:
            return false;
        }
    }

    void Disconnect() {
        if (SQLStatementHandle) {
            SQLFreeHandle(SQL_HANDLE_STMT, SQLStatementHandle);
            SQLStatementHandle = NULL;
        }
        if (SQLConnectionHandle) {
            SQLDisconnect(SQLConnectionHandle);
            SQLFreeHandle(SQL_HANDLE_DBC, SQLConnectionHandle);
            SQLConnectionHandle = NULL;
        }
        if (SQLEnvHandle) {
            SQLFreeHandle(SQL_HANDLE_ENV, SQLEnvHandle);
            SQLEnvHandle = NULL;
        }
    }

    bool ExecuteQuery(const std::string& query, std::vector<std::vector<std::string>>& result) {

        if (!SQLConnectionHandle) {
            std::cerr << "Not connected to the database." << std::endl;
            return false;
        }

        if (SQL_SUCCESS != SQLAllocHandle(SQL_HANDLE_STMT, SQLConnectionHandle, &SQLStatementHandle)) {
            ShowSQLError(SQL_HANDLE_ENV, SQLEnvHandle);
            return false;
        }

        if (SQL_SUCCESS != SQLExecDirect(SQLStatementHandle, (SQLCHAR*)query.c_str(), SQL_NTS)) {
            ShowSQLError(SQL_HANDLE_STMT, SQLStatementHandle);
            return false;
        }

        result.clear();

        while (SQLFetch(SQLStatementHandle) == SQL_SUCCESS) {
            SQLSMALLINT columns;
            SQLNumResultCols(SQLStatementHandle, &columns);
            std::vector<std::string> row;

            for (SQLLEN i = 1; i <= columns; ++i) {

                SQLCHAR columnData[256];
                SQLLEN dataSize;
                SQLGetData(SQLStatementHandle, i, SQL_C_CHAR, columnData, sizeof(columnData), &dataSize);
                row.push_back(std::string(reinterpret_cast<char*>(columnData), dataSize));
            }

            result.push_back(row);
        }

        return true;
    }

private:
    SQLHANDLE SQLEnvHandle;
    SQLHANDLE SQLConnectionHandle;
    SQLHANDLE SQLStatementHandle;

    void ShowSQLError(unsigned int handleType, const SQLHANDLE& handle) {
        SQLCHAR SQLState[1024];
        SQLCHAR message[1024];
        if (SQL_SUCCESS == SQLGetDiagRec(handleType, handle, 1, SQLState, NULL, message, 1024, NULL)) {
            std::cerr << "SQL driver message: " << message << "\nSQL state: " << SQLState << "." << std::endl;
        }
    }
};


SQLDatabase database;
std::string connectionString = "DRIVER={SQL Server}; SERVER=LAPTOP-9H2KNCLC, 1433; DATABASE=codebase; UID=root; PWD=root;";


// ****************************************************** GENERAL FUNCTIONS ********************************* //

void clearStream() {

    std::cin.ignore(INT_MAX, '\n');

}
void clearScreen() {

    system("cls");
}

// ****************************************************** USER ATTRIBUTES CLASS ********************************* //

class UserDetials {

    std::string username , usermail , password;

public:

    void setCredentials() {

        std::cout << "\n\n\n";
        std::cout << "\t\t\tENTER USERMAIL : ";
        std::cin >> usermail;
        std::cout << std::endl;
        std::cout << "\t\t\tENTER PASSWORD : ";
        std::cin >> password;

    }

    bool login() {

        setCredentials();
        return true;
    }
    std::string getUsermail() {

        return usermail;
    }
    std::string getPassword() {

        return password;
    }
    
};

UserDetials* user = new UserDetials();

// ****************************************************** FILE ATTRIBUTES CLASS ********************************* //

class fileAttributes {

private:
    
    std::string fileName, fileAuthor, fileLanguage , header;
    std::string fileContents ;
    bool existingFile = false;
    int fileSerial = 0;

public:

    void setAttributes() {

        clearScreen();
        clearStream();
        
        std::cout << "\t\t\t SET FILE ATTRIBUTES\n\n";

        // Prompt the user for input
        std::cout << "Enter the name of the File : ";
        std::getline(std::cin, fileName);

        std::cout << "Enter the author's name : ";
        std::getline(std::cin, fileAuthor);

        std::cout << "Enter the programming language : ";
        std::getline(std::cin, fileLanguage);



    }

    void getAttributes() {

        
        // Prompt the user for input
        std::cout << "Filename : "<<fileName<<std::endl;
        std::cout << "Author : " << fileAuthor << std::endl;
        std::cout << "Language : " << fileLanguage << std::endl;
        

    }

    bool confirm() {

        char choice;
        std::cout << "\n\nCreate file with above values ? (y/n) : ";
        std::cin >> choice;
        choice = tolower(choice);
        if (choice == 'y') {

            
            // go ahead with attribuates
        }
        else {

            // dont make the file redo the values
            return false;
        }

        return true;
    }

    friend class Editor;


};


// ****************************************************** EDITOR CLASS ********************************* //

class Editor : private fileAttributes {

private:
    std::vector<std::vector<std::string>> userFiles;
public: 

    int runEditor() {

        system("cls");

        
        MAKEFILE : makeFile();
        if (confirm()) {

            // go ahead file is created
            displayEditor();
        }
        else {

            goto MAKEFILE;
        }

        return 1;

    }

    bool getFiles() {

        std::string query = "SELECT * from record WHERE usermail = '" + user->getUsermail() +"'";

        if (database.Connect(connectionString)) {

            std::vector<std::vector<std::string>> result;

            if (database.ExecuteQuery(query, result)) {

                userFiles = result;
            }
            else {

                std::cerr << "Query execution failed." << std::endl;
                return false;
            }
        }
        else {

            std::cerr << "Database connection failed." << std::endl;
            return false;
        }

        return true;

    }
    void loadFile() {
        

        int fileNumber = 0;
        std::cout << "\n\n\t\t\t Which file you want to open : ";
        std::cin >> fileNumber;
        fileNumber--;
        if (fileNumber >= 0 && fileNumber <= userFiles.size() - 1) {

            fileSerial = stoi(userFiles[fileNumber][0]);
            existingFile = true;
            fileName = userFiles[fileNumber][1];
            fileAuthor = userFiles[fileNumber][2];
            fileLanguage = userFiles[fileNumber][4];
            fileContents = userFiles[fileNumber][5];
        }
        
        
    }

    void openList() {

        clearScreen();

        std::cout << "\t\t\t --------------- " << user->getUsermail() << " Files -----------------\n\n";
        
        std::cout << std::endl;
        std::cout << std::setw(12) << "File Number" << std::setw(20) << "File Author" << std::setw(30) << "File Name" << std::setw(25) << "File Language" << std::endl;
        std::cout << "\n======================================================================================================" << std::endl;
        int index = 1;
        for (auto i : userFiles) {

            std::cout << std::setw(12) << index++ << std::setw(20) << i[2] << std::setw(30) << i[3] << std::setw(20) << i[4] << std::setw(15) << std::endl;

        }
        std::cout << "\n======================================================================================================" << std::endl;
    }
    int fileExplorer() {

        if (getFiles() == false) {

            return 1;

        }
        openList();
        loadFile();
        displayEditor();
        

        return 1;
    }

    void deleteInstace() {

        int fileNumber = 0;
        std::cout << "\n\n\t\t\t Which file you want to delete : ";
        std::cin >> fileNumber;
        
        if (fileNumber >= 0 && fileNumber <= userFiles.size() - 1) {

            std::string query = "DELETE FROM record WHERE RID =  '" + std::to_string(fileNumber) + "'";

            if (database.Connect(connectionString)) {

                std::vector<std::vector<std::string>> result;

                if (database.ExecuteQuery(query, result)) {

                    userFiles = result;
                }
                else {

                    std::cerr << "Query execution failed." << std::endl;
                    
                }
            }
            else {

                std::cerr << "Database connection failed." << std::endl;
                
            }
        }


    }

    int deleteFile() {

        if (getFiles() == false) {

            return 1;

        }
        openList();
        deleteInstace();

        return 1;
    }


private:


    void makeFile() {

        setAttributes();
        
    }
    void readContents() {
           
        char c;
        while (1) {

            if (_kbhit()) {


                c = _getch();
                if (c == 27) { // Check for the Escape key (ASCII value 27)
                    break; // Exit the loop if the Escape key is pressed
                }

                // Handle Enter key (ASCII value 13)
                if (c == 13) {

                    fileContents += '\n';
                    std::cout << std::endl; // Move to a new line
                }
                else if (c == 8) { // Handle Backspace key (ASCII value 8)

                    // if (!inputBuffer.empty() && inputBuffer.back() == '\n') {
                    //     system("cls"); // Clear the screen
                    //     std::cout << inputBuffer; // Reprint the input buffer
                    // }

                    if (!fileContents.empty()) {

                        if (fileContents.back() == '\n') {

                            fileContents.pop_back();

                            displayFile();
                        }
                        else {

                            fileContents.pop_back();

                            // Erase the character from the console
                            std::cout << "\b \b";
                        }

                    }
                }
                else {

                    fileContents += c;
                    std::cout << c; // Display the entered character
                }
            }
        }


    }

    void displayFile() {

       // clearStream();
        clearScreen();
        
        std::cout << header << std::endl;
        std::cout << std::endl;
        std::cout << fileContents;

    }
    void displayEditor() {

        header = "\t\t\t :::::: " + fileName + "   " + fileAuthor + "    " + fileLanguage + " :::::: ";
        while (true) {

            displayFile();

            readContents();

            displayFile();

            if (saveFile()) {

                break;
                
            }

        }

    }

    bool storeFile() {

        std::string query = "INSERT INTO record VALUES ('" + user->getUsermail() + "', '" + fileAuthor + "', '" + fileName + "', '" + fileLanguage + "', '" + fileContents + "')";

        if (database.Connect(connectionString)) {

            std::vector<std::vector<std::string>> result;

            if (database.ExecuteQuery(query, result)) {

                // query result
            }
            else {

                std::cerr << "Query execution failed." << std::endl;
                return false;
            }
        }
        else {

            std::cerr << "Database connection failed." << std::endl;
            return false;
        }

        return true;
    }

    bool updateFile() {

        std::string query = "UPDATE record SET usermail = '" + user->getUsermail() + "', FileAuthor = '" + fileAuthor + "', FileName = '" + fileName + "', FileLanguage = '" + fileLanguage + "', FileContent = '" + fileContents + "' WHERE RID = " + "'" + std::to_string(fileSerial)+"'";


        if (database.Connect(connectionString)) {

            std::vector<std::vector<std::string>> result;

            if (database.ExecuteQuery(query, result)) {

                // query result
            }
            else {

                std::cerr << "Query execution failed." << std::endl;
                return false;
            }
        }
        else {

            std::cerr << "Database connection failed." << std::endl;
            return false;
        }

        return true;
    }

    bool saveFile() {

        char choice;

        std::cout << "\n\nDo you want to store the file to your space (y/n/e): ";
        std::cin >> choice;

        choice = tolower(choice);
        if (choice == 'n') {

            return false;
        }
        
        if (choice == 'e') {

            return true;
        }

        bool code = false;
        if (existingFile == false) {

             code = storeFile();
        }
        else {

            code = updateFile();
        }
        
        

        return code;

    }
};




// ****************************************************** CODE EDITOR CLASS ********************************* //

class codeEditor {

private : 
    Editor* editor = new Editor();
public : 

       
    int run() {

        printName();
        int status = displayOptions();

        return status;

    }
    
private:

    void printName() {

        system("cls");

        // ANSI escape codes for text formatting
        const char* colorRed = "\033[1;31m";  // Red color
        const char* colorReset = "\033[0m";  // Reset text color to default
        const int screenWidth = 100; // Assuming a screen width of 80 characters

        // Text to display
        const char* text = "ProEditor";

        // Calculate the number of spaces needed for centering
        int padding = (screenWidth - strlen(text)) / 2;

        // Display the centered and colored text
        std::cout << std::string(padding, ' ') << colorRed << text << colorReset << std::endl;

        std::cout << "\n\n\n";

    }


    int displayOptions() {


        std::cout << "\t\t\t 1. CREATE a NEW File\n\n";
        std::cout << "\t\t\t 2. OPEN a EXISTING File\n\n";
        std::cout << "\t\t\t 3. DELETE a File \n\n";
        std::cout << "\t\t\t 4. CLOSE \n\n";

        int option = 0;
        std::cout << "\t\t\t\t Make choice : ";
        std::cin >> option;
        int status = 1;
        switch (option) {

            case 1:
                status = editor->runEditor();
                break;
            case 2:
                status = editor->fileExplorer();
                break;
            case 3:
                status = editor->deleteFile();
                break;
            case 4:
                status = closeApplication();
                break;
            default:
                status = 0;
                break;

        }
        return status;
        
    }
    int closeApplication() {

        std::cout << "Closing application\n";
        return 0;
    }


};

// ****************************************************** MAIN FUNCTION ********************************* //


int main()
{

    user->login();

    bool validUser = 0;


    if (database.Connect(connectionString)) {

        std::string query = "SELECT usermail , password FROM users";

        std::vector<std::vector<std::string>> result;

        if (database.ExecuteQuery(query, result)) {

            for (const auto& row : result) {
                
                
                if (row[0] == user->getUsermail() && row[1] == user->getPassword()) {

                    validUser = 1;
                    break;
                }
            }
        }
        else {
            std::cerr << "Query execution failed." << std::endl;
        }
    }
    else {
        std::cerr << "Database connection failed." << std::endl;
    }


    if (validUser != 1) {

        exit(0);
    }
    
    while (1) {

        clearScreen();
        //clearStream();

        codeEditor* editor = new codeEditor();

        int status = editor->run();
        if (status == 0) {

            system("cls");
            std::cout << "CLOSED SUCCESSFULLY\n";
            break;
        }
    }
    
    return 0;
}
