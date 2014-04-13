#include <stdexcept>
#include <string>
#include <sstream>

//#define TESTING
#ifndef TESTING

#include <iostream>
#include <fstream>

class FileError : public std::runtime_error {
public:

    FileError( const std::string &fileNameIn ) : std::runtime_error( "" ), m_fileName( fileNameIn ) {
    }

    virtual const char* what( ) const throw () {
        return m_msg.c_str( );
    }

    std::string getFileName( ) const {
        return m_fileName;
    }

    virtual ~FileError( ) throw () {

    }

protected:
    std::string m_fileName;
    std::string m_msg;
};

class FileOpenError : public FileError {
public:

    FileOpenError( const std::string &fileNameIn ) : FileError( fileNameIn ) {
        m_msg = "Unable to open " + fileNameIn;
    }
};

class FileReadError : public FileError {
public:

    FileReadError( const std::string &fileNameIn, int lineNumIn ) :
    FileError( fileNameIn ), m_lineNum( lineNumIn ) {
        std::ostringstream ostr;
        ostr << "Error reading " << fileNameIn << " at line " << lineNumIn;
        m_msg = ostr.str( );
    }

    int getLineNum( ) const {
        return m_lineNum;
    }

protected:
    int m_lineNum;
};

class FileWriteError : public FileError {
public:

    FileWriteError( const std::string &fileNameIn ) : FileError( fileNameIn ) {
        m_msg = "Unable to write " + fileNameIn;
    }
};

#endif // TESTING

class LogicError : public std::logic_error {
public:

    LogicError( int argument ) : std::logic_error( "" ), m_argument( argument ) {

    }

    virtual const char *what( ) const throw () {
        return m_message.c_str( );
    }

    virtual ~LogicError( ) throw () {

    }

protected:
    int m_argument;
    std::string m_message;
};

class OutOfRange : public LogicError {
public:

    OutOfRange( int argument, int beginOfRange, int endOfRange ) : LogicError( argument ) {
        std::string str_argument, str_beginOfRange, str_endOfRange;

        str_argument = intToString( argument );
        str_beginOfRange = intToString( beginOfRange );
        str_endOfRange = intToString( endOfRange );

        m_message = "Argument " + str_argument + " doesn't hit in the range [" +
                str_beginOfRange + ", " + str_endOfRange + "]";
    }

private:

    std::string intToString( int number ) {
        std::stringstream stream;
        stream << number;
        return stream.str( );
    }
};

#ifndef TESTING
void readData( const std::string &fileName, int &firstNumber, int &secondNumber )
throw (FileOpenError, FileReadError);

void writeResult( const std::string &fileName, int number )
throw (FileOpenError, FileWriteError);

int sum( int firstNumber, int secondNumber ) throw (OutOfRange);

int main( ) {
    std::string fileNameIn = "input.txt";
    int firstNumber = 0;
    int secondNumber = 0;

    try {
        readData( fileNameIn, firstNumber, secondNumber );
    } catch ( const FileError &e ) {
        std::cerr << e.what( ) << std::endl;
        return 1;
    }

    int result = 0;
    try {
        result = sum( firstNumber, secondNumber );
    } catch ( const LogicError &e ) {
        std::cerr << e.what( ) << std::endl;
        return 1;
    } catch ( ... ) {
        std::cerr << "Uncaught exception." << std::endl;
        return 1;
    }


    std::string fileNameOut = "output.txt";
    try {
        writeResult( fileNameOut, result );
    } catch ( const FileError &e ) {
        std::cerr << e.what( ) << std::endl;
        return 1;
    }

    return 0;
}

void readData( const std::string &fileName, int &firstNumber, int &secondNumber )
throw ( FileOpenError, FileReadError) {
    std::ifstream file;
    file.open( fileName.c_str( ) );
    if ( file.fail( ) ) {
        throw FileOpenError( fileName );
    }

    int lineNumIn = 0;
    if ( !(file >> firstNumber >> secondNumber) ) {
        lineNumIn++;
        throw FileReadError( fileName, lineNumIn );
    }
}

void writeResult( const std::string &fileName, int number )
throw (FileOpenError, FileWriteError) {
    std::ofstream file;
    file.open( fileName.c_str( ) );

    if ( file.fail( ) ) {
        throw FileOpenError( fileName );
    }

    if ( !(file << number) ) {
        throw FileWriteError( fileName );
    }

    file << std::endl;
}
#endif // TESTING

int sum( int firstNumber, int secondNumber ) throw (OutOfRange) {
    int const beginOfRange = -1000000000;
    int const endOfRange = 1000000000;

    if ( (firstNumber < beginOfRange) || (endOfRange < firstNumber) ) {
        throw ( OutOfRange( firstNumber, beginOfRange, endOfRange ));
    }

    if ( (secondNumber < beginOfRange) || (endOfRange < secondNumber) ) {
        throw ( OutOfRange( secondNumber, beginOfRange, endOfRange ));
    }

    int result = firstNumber + secondNumber;
    return result;
}
