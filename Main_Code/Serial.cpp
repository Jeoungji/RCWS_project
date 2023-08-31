#include "Serial.h"
#include <string>
#include <iostream>
#include <vector>

#define RESETCODE 8972


Serial::Serial(unsigned short port, std::string Name) /*COM port number*/
    : Port(port), PortName(Name)
{
    //const char* portName;
    std::string portname = "COM";
    portname = portname + std::to_string(Port);

    std::cout << "[INFO] Connecting " << PortName << " Port Number : " << portname << std::endl;
    //We're not yet connected
    this->connected = false;

    //Try to connect to the given port throuh CreateFile
    this->hSerial = CreateFileA(portname.c_str(),
        GENERIC_READ | GENERIC_WRITE,
        0,
        NULL,
        OPEN_EXISTING,
        FILE_ATTRIBUTE_NORMAL,
        NULL);

    //Check if the connection was successfull
    if (this->hSerial == INVALID_HANDLE_VALUE)
    {
        //If not success full display an Error
        if (GetLastError() == ERROR_FILE_NOT_FOUND) {
            //Print Error if neccessary
            std::cout << "[ERROR] Handle was not attached. Reason: COM" << Port << " not available." << std::endl;
        }
        else
            std::cout << "[ERROR] Handle was not attached" << std::endl;
        exit(0);
    }
    else
    {
        //If connected we try to set the comm parameters
        DCB dcbSerialParams = { 0 };

        //Try to get the current
        if (!GetCommState(this->hSerial, &dcbSerialParams))
        {
            //If impossible, show an error
            std::cout << "[ERROR] failed to get current serial parameters!" << std::endl;
            exit(0);
        }
        else
        {
            //Define serial connection parameters for the arduino board
            dcbSerialParams.BaudRate = CBR_115200;
            dcbSerialParams.ByteSize = 8;
            dcbSerialParams.StopBits = ONESTOPBIT;
            dcbSerialParams.Parity = NOPARITY;
            //Setting the DTR to Control_Enable ensures that the Arduino is properly
            //reset upon establishing a connection
            dcbSerialParams.fDtrControl = DTR_CONTROL_ENABLE;

            //Set the parameters and check for their proper application
            if (!SetCommState(hSerial, &dcbSerialParams))
            {
                std::cout << "[ERROR] ALERT: Could not set Serial Port parameters" << std::endl;
                exit(0);
            }
            else
            {
                //If everything went fine we're connected
                this->connected = true;
                //Flush any remaining characters in the buffers 
                PurgeComm(this->hSerial, PURGE_RXCLEAR | PURGE_TXCLEAR);
                //We wait 2s as the arduino board will be reseting
                Sleep(ARDUINO_WAIT_TIME);
            }
        }
    }
    std::cout << "[INFO] " << PortName << " Serial buffer size : " << Rbuffer.size() << std::endl;
}

Serial::~Serial()
{
    //Check if we are connected before trying to disconnect
    if (this->connected)
    {
        //We're no longer connected
        this->connected = false;
        //Close the serial handler
        CloseHandle(this->hSerial);
    }
}

bool Serial::Reset() {
    std::cout << "[INFO] reset" << std::endl;

    //We're no longer connected
    this->connected = false;
    //Close the serial handler
    CloseHandle(this->hSerial);
    Sleep(500);

    //const char* portName;
    std::string portname = "COM";
    portname = portname + std::to_string(Port);

    //We're not yet connected
    this->connected = false;

    //Try to connect to the given port throuh CreateFile
    this->hSerial = CreateFileA(portname.c_str(),
        GENERIC_READ | GENERIC_WRITE,
        0,
        NULL,
        OPEN_EXISTING,
        FILE_ATTRIBUTE_NORMAL,
        NULL);

    //Check if the connection was successfull
    if (this->hSerial == INVALID_HANDLE_VALUE)
    {
        //If not success full display an Error
        if (GetLastError() == ERROR_FILE_NOT_FOUND) {
            //Print Error if neccessary
            std::cout << "[ERROR] Handle was not attached. Reason: COM" << Port << " not available." << std::endl;
        }
        else
            std::cout << "[ERROR] Handle was not attached" << std::endl;
        exit(0);
    }
    else
    {
        //If connected we try to set the comm parameters
        DCB dcbSerialParams = { 0 };

        //Try to get the current
        if (!GetCommState(this->hSerial, &dcbSerialParams))
        {
            //If impossible, show an error
            std::cout << "[ERROR] failed to get current serial parameters!" << std::endl;
            exit(0);
        }
        else
        {
            //Define serial connection parameters for the arduino board
            dcbSerialParams.BaudRate = CBR_115200;
            dcbSerialParams.ByteSize = 8;
            dcbSerialParams.StopBits = ONESTOPBIT;
            dcbSerialParams.Parity = NOPARITY;
            //Setting the DTR to Control_Enable ensures that the Arduino is properly
            //reset upon establishing a connection
            dcbSerialParams.fDtrControl = DTR_CONTROL_ENABLE;

            //Set the parameters and check for their proper application
            if (!SetCommState(hSerial, &dcbSerialParams))
            {
                std::cout << "[ERROR] ALERT: Could not set Serial Port parameters" << std::endl;
                exit(0);
            }
            else
            {
                //If everything went fine we're connected
                this->connected = true;
                //Flush any remaining characters in the buffers 
                PurgeComm(this->hSerial, PURGE_RXCLEAR | PURGE_TXCLEAR);
                //We wait 2s as the arduino board will be reseting
                Sleep(ARDUINO_WAIT_TIME);
            }

        }
    }
    return true;
}

int Serial::ReadData(char* buffers, const unsigned int nbChar)
{
    //Number of bytes we'll have read
    DWORD bytesRead;
    //Number of bytes we'll really ask to read
    unsigned int toRead = 0;

    //Use the ClearCommError function to get status info on the Serial port
    ClearCommError(this->hSerial, &this->errors, &this->status);

    //Check if there is something to read
    if (this->status.cbInQue > (nbChar - 1))
    {
        //If there is we check if there is enough data to read the required number
        //of characters, if not we'll read only the available characters to prevent
        //locking of the application.

        if (this->status.cbInQue > nbChar)
        {
            toRead = nbChar;
        }
        else
        {
            toRead = this->status.cbInQue;
        }

        //Try to read the require number of chars, and return the number of read bytes on success

        if (ReadFile(this->hSerial, buffers, nbChar, &bytesRead, NULL))
        {
            return bytesRead;
        }
    }
    //If nothing has been read, or that an error was detected return 0
    return 0;
}

bool Serial::WriteData(const char* buffer, const unsigned int nbChar)
{
    DWORD bytesSend;

    //Try to write the buffer on the Serial port
    if (!WriteFile(this->hSerial, (void*)buffer, nbChar, &bytesSend, 0))
    {
        //In case it don't work get comm error and return false
        ClearCommError(this->hSerial, &this->errors, &this->status);

        return false;
    }
    else
        return true;
}

bool Serial::IsConnected()
{
    //Simply return the connection status
    return this->connected;
}

int Serial::ReadData(MainMCURecvcom& data) {
    //Number of bytes we'll have read
    DWORD bytesRead;
    //Number of bytes we'll really ask to read
    unsigned int toRead = 0;

    //Use the ClearCommError function to get status info on the Serial port
    ClearCommError(this->hSerial, &this->errors, &this->status);

    //Check if there is something to read
    if (this->status.cbInQue > (sizeof(data) - 1))
    {
        //If there is we check if there is enough data to read the required number
        //of characters, if not we'll read only the available characters to prevent
        //locking of the application.

        if (this->status.cbInQue > sizeof(data))
        {
            toRead = sizeof(data);
        }
        else
        {
            toRead = this->status.cbInQue;
        }

        //Try to read the require number of chars, and return the number of read bytes on success

        if (ReadFile(this->hSerial, Rbuffer.data(), sizeof(data), &bytesRead, NULL))
        {
            //Decoding
            memcpy(&data, Rbuffer.data(), sizeof(data));
            return bytesRead;
        }
    }
    //If nothing has been read, or that an error was detected return 0
    return 0;
}

int Serial::ReadData(OpticalMCURecvcom& data) {
    //Number of bytes we'll have read
    DWORD bytesRead;
    //Number of bytes we'll really ask to read
    unsigned int toRead = 0;

    //Use the ClearCommError function to get status info on the Serial port
    ClearCommError(this->hSerial, &this->errors, &this->status);

    //Check if there is something to read
    if (this->status.cbInQue > (sizeof(data) - 1))
    {
        //If there is we check if there is enough data to read the required number
        //of characters, if not we'll read only the available characters to prevent
        //locking of the application.

        if (this->status.cbInQue > sizeof(data))
        {
            toRead = sizeof(data);
        }
        else
        {
            toRead = this->status.cbInQue;
        }

        //Try to read the require number of chars, and return the number of read bytes on success

        if (ReadFile(this->hSerial, Rbuffer.data(), sizeof(data), &bytesRead, NULL))
        {
            //Decoding
            memcpy(&data, Rbuffer.data(), sizeof(data));
            return bytesRead;
        }
    }
    //If nothing has been read, or that an error was detected return 0
    return 0;
}

int Serial::ReadData(GunMCURecvcom& data) {
    //Number of bytes we'll have read
    DWORD bytesRead;
    //Number of bytes we'll really ask to read
    unsigned int toRead = 0;

    //Use the ClearCommError function to get status info on the Serial port
    ClearCommError(this->hSerial, &this->errors, &this->status);

    //Check if there is something to read
    if (this->status.cbInQue > (sizeof(data) - 1))
    {
        //If there is we check if there is enough data to read the required number
        //of characters, if not we'll read only the available characters to prevent
        //locking of the application.

        if (this->status.cbInQue > sizeof(data))
        {
            toRead = sizeof(data);
        }
        else
        {
            toRead = this->status.cbInQue;
        }

        //Try to read the require number of chars, and return the number of read bytes on success

        if (ReadFile(this->hSerial, Rbuffer.data(), sizeof(data), &bytesRead, NULL))
        {
            //Decoding
            memcpy(&data, Rbuffer.data(), sizeof(data));
            return bytesRead;
        }
    }
    //If nothing has been read, or that an error was detected return 0
    return 0;
}

bool Serial::WriteData(OpticalMCUSendcom& data) {
    DWORD bytesSend;

    // Encoding
    memcpy(Sbuffer.data(), &data, sizeof(data));

    //Try to write the buffer on the Serial port
    if (!WriteFile(this->hSerial, Sbuffer.data(), sizeof(data), &bytesSend, 0))
    {
        //In case it don't work get comm error and return false
        ClearCommError(this->hSerial, &this->errors, &this->status);

        return false;
    }
    else
        return true;
}

bool Serial::WriteData(MainMCUSendcom& data) {
    DWORD bytesSend;

    // Encoding
    memcpy(Sbuffer.data(), &data, sizeof(data));

    //Try to write the buffer on the Serial port
    if (!WriteFile(this->hSerial, Sbuffer.data(), sizeof(data), &bytesSend, 0))
    {
        //In case it don't work get comm error and return false
        ClearCommError(this->hSerial, &this->errors, &this->status);

        return false;
    }
    else
        return true;
}

bool Serial::WriteData(GunMCUSendcom& data) {
    DWORD bytesSend;

    // Encoding
    memcpy(Sbuffer.data(), &data, sizeof(data));

    //Try to write the buffer on the Serial port
    if (!WriteFile(this->hSerial, Sbuffer.data(), sizeof(data), &bytesSend, 0))
    {
        //In case it don't work get comm error and return false
        ClearCommError(this->hSerial, &this->errors, &this->status);

        return false;
    }
    else
        return true;
}

bool Serial::Checking(const int size) { /* Setting Checker to last data */
    std::vector<char> buffer(size, NULL);
    //char buffer[BUF] = { NULL, };
    int a = 0;
    
    while (!a)
        a = this->ReadData(buffer.data(), size);

    if (buffer[size - 1] == CHECKER) {
        char ack = 'A';
        //this->WriteData(&ack, 1);
        return true;
    }
    std::cout << "[WARNNING] Data is mixed and recovered" << std::endl;
    std::cout << "[INFO] Data : ";
    std::vector <short> find;
    for (short i = 0; i < size; i++) {
        if (buffer[i] == CHECKER) find.push_back(i);
        printf("%d ", buffer[i]);
        buffer[i] = NULL;
    }
    std::cout << std::endl;
    if (find.empty()) {
        std::cout << "[ERROR] Checking Num is not exist" << std::endl;
        return false;
    }
    if (find.size() > 1) {
        std::cout << "[ERROR] " << find.size() << "Checking Num" << std::endl;
        return false;
    }
    std::cout << "[INFO] Checking Num at " << find[0] << std::endl;
    short b = 0;
    while (!b)
        b = this->ReadData(buffer.data(), 1 + find[0]);

    std::cout << "[INFO] Reading " << b << "Byte" << std::endl;
    a = 0;
    while (!a)
        a = this->ReadData(buffer.data(), size);

    std::cout << "[INFO] Reading " << a << "Byte" << std::endl;

    if (buffer[size - 1] == CHECKER) {
        char ack = 'A';
        return true;
    }
    std::cout << "[ERROR] failed to control it ";
    for (short i = 0; i < size; i++)
        printf("%d ", buffer[i]);
    return false;
}