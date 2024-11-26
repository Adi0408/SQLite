#include <iostream>
#include <string>
#include <vector>
#include <sstream>
#include <cstdlib>
#include <fstream>
#include <unordered_map>

using namespace std;

// meta command function for help
// Function to display the help guide
void show_help() {
    cout << "\nSQLite Help Guide:\n";
    cout << "==================\n";
    cout << "Meta-Commands:\n";
    cout << "  .exit           - Exit the program and save the database.\n";
    cout << "  .help           - Show this help guide.\n";
    cout << "\nCommands:\n";
    cout << "  create table <table_name>   - Create a new table.\n";
    cout << "  drop table <table_name>     - Delete an existing table.\n";
    cout << "  list tables                 - List all tables in the database.\n";
    cout << "  open table <table_name>     - Open a table to perform operations.\n";
    cout << "  close table <table_name>    - Close a table after operations.\n";
    cout << "  insert <table_name> <id> <username> <email> - Insert a new row into the table.\n";
    cout << "  select <table_name>         - Display all rows in the table.\n";
    cout << "  delete <table_name>         - Delete all rows in the table.\n";
    cout << "\nNotes:\n";
    cout << "  - Primary Key (id) must be unique in a table.\n";
    cout << "  - Tables are stored in files named 'database_<table_name>.txt'.\n";
    cout << "  - Use valid syntax for commands to avoid errors.\n";
    cout << endl;
}

enum class MetaCommandResult { SUCCESS, UNRECOGNIZED_META_COMMAND };
enum class StatementType { INSERT, SELECT, DELETE, CREATE_TABLE, DROP_TABLE, LIST_TABLE, OPEN_TBALE, CLOSE_TABLE };
enum class PrepareResult { SUCCESS, SYNTAX_ERROR, UNRECOGNIZED_STATEMENT };
enum class ExecuteResult { SUCCESS, FAILED, TABLE_FULL };

// Constants for table limits
const unsigned int COLUMN_USERNAME_SIZE = 32;
const unsigned int COLUMN_EMAIL_SIZE = 255;
const unsigned int TABLE_MAX_ROWS = 100; // Example limit for simplicity

// Row structure
class Row {
public:
    unsigned int id;
    string username;
    string email;

    Row() : id(0), username(""), email("") {}

    // Serialize row to a human-readable string
    string serialize() const {
        stringstream ss;
        ss << id << "," << username << "," << email << "\n";
        return ss.str();
    }

    // Deserialize row from a string
    void deserialize(const string& data) {
        stringstream ss(data);
        string temp;
        getline(ss, temp, ','); id = stoi(temp) ;
        getline(ss, username, ',');
        getline(ss, email, ',');
    }
};

// Table class to store and manage rows
class Table {
public:
    vector<Row> rows;      // In-memory storage of rows

    // Add a new row to the table
    ExecuteResult insert_row(const Row& row) {
        if (rows.size() >= TABLE_MAX_ROWS) {
            return ExecuteResult::TABLE_FULL;
        }

        // Setting it as primary key
        // check for duplicates
        for (const auto& existing_row : rows) {
            if (existing_row.id == row.id) {
                cout << "Error: Duplicate ID " << row.id << " not allowed. \n";
                return ExecuteResult::SUCCESS; // returning without inserting
            }
        }
        rows.push_back(row);
        return ExecuteResult::SUCCESS;
    }

    // Display all rows
    void select_rows() const {
        for (const auto& row : rows) {
            cout << "(" << row.id << ", " << row.username << ", " << row.email << ")\n";
        }
    }

    void delete_rows (const string& filename) {
        rows.clear();
        ofstream file(filename, ios::trunc);
        if (!file.is_open()) {
            cerr << "Error: Unable to open for Deleting. \n";
        }
        else {
            cout << "All rows deleted. \n";
        }
    }

    // function to save data into file at the end of loop
    void save_to_file (const string& filename) {
        ofstream file(filename, ios::out);

        if (!file.is_open()) {
            cerr << "Error: Unable to open file for writing. \n";
            return;
        }

        for (const auto& row : rows) {
            file << row.serialize();
        }

        file.close();
    }

    // to Load the table data 
    void load_from_file (const string& filename) {
        ifstream file(filename, ios::in);

        if (!file.is_open()) {
            cerr << "Info: File not found. Starting with an empty table. \n";
            return;
        }

        string line;
        while (getline(file, line)) {
            Row row;
            row.deserialize(line);
            rows.push_back(row);
        }
        file.close();
    }
};

// creating a database for tables PK Fk 
class Database {
public:
    unordered_map<string, Table> tables; // map to manage tables
    unordered_map<string, bool> open_tables; // map to track open tables
    // map to show parent chid table relationship child -> parent

    const string table_metadata_file = "listoftables.txt";
    // loading tables
    void load_tables() {
        ifstream file(table_metadata_file);
        if (!file.is_open()) {
            cerr << "Info: No metadata file found. Starting with an empty database.\n";
            return; 
        }

        string table_name;
        while (getline(file, table_name)) {
            tables[table_name] = Table();
            /*string table_file = "database_" + table_name + ".txt";
            tables[table_name].load_from_file(table_file); // loading table data maybe i am doing this twice*/
        }
        file.close();
    }
    
    // while exiting we save table names to the file
    void save_tables() {
        ofstream file (table_metadata_file, ios::out);
        if (!file.is_open()) {
            cerr << "Error: Unable to open metadata file for writing.\n";
            return;
        }

        for (const auto& pair : tables) {
            file << pair.first << "\n"; // Save table name
            /*string table_file = "database_" + pair.first + ".txt";
            Table temptable = pair.second;
            temptable.save_to_file(table_file);*/ // Save table data
        }
        file.close();
    }

    // we create a table
    bool create_table(const string& table_name) {
        if (tables.find(table_name) != tables.end()) {
            cout << "Error: Table " << table_name << " already exists. \n";
            return false;
        }
        tables[table_name] = Table(); // create a new table
        cout<< "Table " << table_name << " created. \n";
        return true;
    }

    // function to drop existing table
    bool drop_table (const string& table_name) {
        if (tables.erase(table_name) == 0) {
            cout << "Error: Table " << table_name << " doesnt exist. \n";
            return false;
        }
        cout<<"Table " << table_name << " deleted successfully. \n";
        return true;
    }

    // listing all the table
    void list_tables() const {
        if (tables.empty()) {
            cout<< "No tables found. \n";
            return;
        }
        cout<<"Tables:"<<endl;
        for (const auto& pair : tables) {
            cout << "-" << pair.first << endl;
        }
    }


    // open_table
    bool open_table (const string& table_name, const string& filename) {
        if (tables.find(table_name) == tables.end()) {
            cout<<"Error: Table " << table_name << " not found.\n";
            return false;
        }

        if (open_tables[table_name]) {
            cout<<"Error: Table " << table_name << " already open.\n";
            return false;
        }
        tables[table_name].load_from_file(filename);
        open_tables[table_name] = true;
        cout<<"Table " << table_name << " opened.\n";
        return true;
    }

    // close table
    bool close_table(const string& table_name, const string& filename) {
        if (tables.find(table_name) == tables.end()) {
            cout << "Error: Table " << table_name << " not found.\n";
            return false;
        }
        if (!open_tables[table_name]) {
            cout << "Error: Table " << table_name << " is not open.\n";
            return false;
        }
        cout<< "Saving to file. \n";
        tables[table_name].save_to_file(filename);
        open_tables[table_name] = false;
        cout<<"Table " << table_name << " closed.\n";
        return true;
    }

    // accessing the table
    Table* get_table(const string& table_name) {
        if (open_tables[table_name]) {
            return &tables[table_name];
        }
        cout<< "Error: Table " << table_name << " is not open.\n";
        return nullptr;
    }
};

// Statement class for parsed commands
class Statement {
public:
    StatementType type;
    string table_name; // to store the table name
    Row row_to_insert; // used while inserting a new row
};

// Meta-command processing (e.g., `.exit`)
MetaCommandResult do_meta_command(const string& input, Database& database) {
    if (input == ".exit") {
        cout<< "Saving tables to file.\n";
        database.save_tables();
        exit(EXIT_SUCCESS);
    }
    if (input == ".help") {
        show_help();
        return MetaCommandResult::SUCCESS;
    }
    return MetaCommandResult::UNRECOGNIZED_META_COMMAND;
}

// Prepare SQL-like statements
PrepareResult prepare_statement(const string& input, Statement& statement) {
    if (input.rfind("create table", 0) == 0) { // condition begin from 0, if true return 0
        statement.type = StatementType::CREATE_TABLE;
        statement.table_name = input.substr(13); // extracting table name
        return PrepareResult::SUCCESS;
    }
    if (input.rfind("drop table", 0) == 0) {
        statement.type = StatementType::DROP_TABLE;
        statement.table_name = input.substr(11);
        return PrepareResult::SUCCESS;
    }
    if (input == "list tables") {
        statement.type = StatementType::LIST_TABLE;
        return PrepareResult::SUCCESS;
    }
    if (input.rfind("open table", 0) == 0) {
        statement.type = StatementType::OPEN_TBALE;

        istringstream ss(input);
        string command1, command2;
        ss >> command1 >> command2 >> statement.table_name;
        return PrepareResult::SUCCESS;
    }
    if (input.rfind("close table", 0) == 0) {
        statement.type = StatementType::CLOSE_TABLE;

        istringstream ss(input);
        string command1, command2;
        ss >> command1 >> command2 >> statement.table_name;
        return PrepareResult::SUCCESS;
    }
    if (input.rfind("insert", 0) == 0) {
        statement.type = StatementType::INSERT;

        istringstream ss(input);
        string command;
        ss >> command >> statement.table_name >> statement.row_to_insert.id
           >> statement.row_to_insert.username
           >> statement.row_to_insert.email;

        if (ss.fail() || statement.row_to_insert.username.empty() || statement.row_to_insert.email.empty()) {
            return PrepareResult::SYNTAX_ERROR;
        }

        return PrepareResult::SUCCESS;
    } 
    if (input.rfind("select", 0) == 0) {
        statement.type = StatementType::SELECT;

        istringstream ss(input);
        string command;
        ss >> command >> statement.table_name;

        return PrepareResult::SUCCESS;
    }
    if (input.rfind("delete", 0) == 0) {
        statement.type = StatementType::DELETE;

        istringstream ss(input);
        string command;
        ss >> command >> statement.table_name;

        return PrepareResult::SUCCESS;
    }
    return PrepareResult::UNRECOGNIZED_STATEMENT;
}


// Execute a statement
ExecuteResult execute_statement(const Statement& statement, const string& base_filename, Database& database) {
    Table* table = nullptr;
    string table_filename = base_filename + "_" + statement.table_name + ".txt"; // file for each table

    switch (statement.type) {
    case StatementType::CREATE_TABLE:
        database.create_table(statement.table_name);
        return ExecuteResult::SUCCESS;

    case StatementType::DROP_TABLE:
        database.drop_table(statement.table_name);
        return ExecuteResult::SUCCESS;

    case StatementType::LIST_TABLE:
        database.list_tables();
        return ExecuteResult::SUCCESS;

    case StatementType::INSERT:
        table = database.get_table(statement.table_name);
        if (table) {
            return table -> insert_row(statement.row_to_insert);
        }
        return ExecuteResult::SUCCESS;

    case StatementType::SELECT:
        table = database.get_table(statement.table_name);
        if (table) {
            table -> select_rows();
        }
        return ExecuteResult::SUCCESS;
    case StatementType::DELETE:
        table = database.get_table(statement.table_name);
        if (table) {
            table -> delete_rows(table_filename);
        }
        return ExecuteResult::SUCCESS;

    case StatementType::OPEN_TBALE:
        database.open_table(statement.table_name, table_filename);
        return ExecuteResult::SUCCESS; 

    case StatementType::CLOSE_TABLE:
        database.close_table(statement.table_name, table_filename);
        return ExecuteResult::SUCCESS;
           
    default:
        return ExecuteResult::SUCCESS;
    }
}

int main() {
    cout << "Welcome.\nUse .help meta command for help.\n";  
    const string base_filename = "database"; // only 1 database
    
    Database database;
    // load existing tables;
    database.load_tables();

    string input_buffer;

    while (true) {
        cout << endl<< "SQLite> ";
        getline(cin, input_buffer);

        // Handle meta-commands (e.g., `.exit`)
        if (input_buffer[0] == '.') {
            switch (do_meta_command(input_buffer, database)) {
            case MetaCommandResult::SUCCESS:
                continue;
            case MetaCommandResult::UNRECOGNIZED_META_COMMAND:
                cout << "Unrecognized command '" << input_buffer << "'\n";
                continue;
            }
        }

        // Prepare SQL-like statements
        Statement statement;
        switch (prepare_statement(input_buffer, statement)) {
            case PrepareResult::SUCCESS:
                break;
            case PrepareResult::SYNTAX_ERROR:
                cout << "Syntax error. Could not parse statement.\n";
                continue;
            case PrepareResult::UNRECOGNIZED_STATEMENT:
                cout << "Unrecognized keyword at start of '" << input_buffer << "'.\n";
                continue;
        }

        // Execute the statement
        switch (execute_statement(statement, base_filename, database)) {
            case ExecuteResult::SUCCESS:
                cout << "Executed.\n";
                break;
            case ExecuteResult::TABLE_FULL:
                cout << "Error: Table full.\n";
                break;
        }
    }
}
