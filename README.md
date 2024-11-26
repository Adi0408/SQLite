# SQLite-Lite: A Lightweight SQLite-Inspired Database  

**By Aditya Senapati**
SQLite-Lite is a simple database system implemented in C++ that mimics basic functionality of SQLite. It provides users with the ability to create, manage, and interact with tables using SQL-like commands, all through a command-line interface.  

## **Features**  
- **Create, Drop, and Manage Tables**  
- **CRUD Operations**: Insert, select, and delete rows in tables.  
- **Data Persistence**: Table data is stored persistently in text files using serialization and deserialization.  
- **Interactive Meta-Commands**: Commands like `.help` and `.exit` provide guidance and manage the session.  

## **Setup Instructions**  
1. Clone or download the repository.  
2. Open the `sqlite_lite.cpp` file in your favorite C++ IDE or text editor.  
3. Compile and run the program using any C++ compiler (e.g., GCC, Visual Studio, or CLion).  

## **How to Use**  
Start the program and interact with the database using the following commands:  

### **Meta-Commands**  
- `.help` — Displays the help guide.  
- `.exit` — Exits the program and saves the database.  

### **SQL-like Commands**  
- **Create Table**:  
  ```sql
  create table table_name

