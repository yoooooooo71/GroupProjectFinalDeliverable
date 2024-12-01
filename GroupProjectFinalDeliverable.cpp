// GroupProjectFinalDeliverable.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <vector>
#include <string>
#include <bitset>
#include <sstream>
#include <fstream>
#include <tuple>
#include <iomanip>
#include <cstdint>
#include <regex>
#include <stdlib.h> //for Windows
#include <cstdlib> //for a Mac function
 
using namespace std;

//Challenge 1 Functions
// Code set to convert hex number to base64
string hexToBase64(const string& hex)
{
    vector<uint8_t> bytes;
    // Note: uint8_t splits the hex string into unsigned 8-bit integers

    // Create variables
    string base64;
    int hexBits = 0;
    int bits = 0;

    // Creating the Base64 encoding table
    const string base64Table =
        "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

    // Convert hex string to bytes
    // Loops through the hex number and converts the hex string to the bytes vector
    for (size_t i = 0; i < hex.length(); i += 2)
    {
        string byteString = hex.substr(i, 2);
        bytes.push_back(stoul(byteString, nullptr, 16));
    }
    // Encode bytes to Base64
    for (uint8_t byte : bytes)
    {
        hexBits = (hexBits << 8) | byte;
        bits += 8;

        while (bits >= 6)
        {
            base64 += base64Table[(hexBits >> (bits - 6)) & 0x3F];
            bits -= 6;
        }
    }
    // Shift for accurate conversion
    if (bits > 0)
    {
        hexBits <<= (6 - bits);
        base64 += base64Table[hexBits & 0x3F];
    }
    // Verify length is a multiple of 4
    while (base64.length() % 4 != 0)
    {
        base64 += "=";
    }
    return base64;
}

// Function to validate hexadecimal input
bool isValidHex(const string& hex)
{
    regex hexPattern("^[0-9a-fA-F]+$");
    return regex_match(hex, hexPattern);
}

//Challenge 2 Functions
bool isValidHex(const std::string& hex)
{
    std::regex hexPattern("^[0-9a-fA-F]+$");
    return regex_match(hex, hexPattern);
}

std::vector<uint8_t> hexToBytes(const std::string& hex)
{
    std::vector<uint8_t> bytes;

    for (size_t i = 0; i < hex.length(); i += 2)
    {
        std::string byteString = hex.substr(i, 2);
        bytes.push_back(stoul(byteString, nullptr, 16));
    }

    return bytes;
}

std::string bytesToHex(std::vector<uint8_t> bytes)
{
    std::stringstream convert;

    for (int i = 0; i < bytes.size(); i++)
    {
        convert << std::hex << int(bytes.at(i));
    }

    return convert.str();
}

std::string fixedXOR(const std::string& temp1, const std::string& temp2)
{
    std::vector<uint8_t> newtemp1 = hexToBytes(temp1);
    std::vector<uint8_t> newtemp2 = hexToBytes(temp2);

    std::vector<uint8_t> temp;

    for (int i = 0; i < newtemp1.size(); i++)
    {
        temp.push_back(newtemp1.at(i) ^ newtemp2.at(i));
    }

    return bytesToHex(temp);

}

//Challenge 3 Functions

//Challenge 4 Functions

//Challenge 5 Functions

//Challenge 6 Functions

//Challenge 7 Functions

//Challenge 8 Functions

//Main
int main()
{
    cout << "Welcome to the CryptoPals Set 1 Challenges";
    cout << endl;
    cout << endl;

    int choice = 9;

    //while loop for the menu
    while (choice != 0) {
        cout << "Choose your challenge. Enter 1 for challenge 1, 2 for challenge 2,..., 8 for challenge 8";
        cout << "To exit out of the menu, enter 0";
        cout << endl;
        cout << endl;
        cout << "Challenge 1: Convert hex to base64"; cout << endl;
        cout << "Challenge 2: Fixed XOR"; cout << endl;
        cout << "Challenge 3: Single-byte XOR cipher"; cout << endl;
        cout << "Challenge 4: Detect single-character XOR"; cout << endl;
        cout << "Challenge 5: Implement repeating-key XOR"; cout << endl;
        cout << "Challenge 6: Break repeating-key XOR"; cout << endl;
        cout << "Challenge 7: AES in ECB mode"; cout << endl;
        cout << "Challenge 8: Detect AES in ECB mode"; cout << endl;
        cout << "--: "; cin >> choice;

        //Clear console supported by three different OS.
        //Just to make things prettier.

        #if __linux__ || __APPLE__
                system("clear");
        #elif _WIN32 || _WIN64
                system("cls");
        #endif
        

        //switch statements to choose each challenge
        /*
        //While loop for letting the user have multiple attempts with an if statement before the challenge to ask for a hint
        */
        switch (choice) {
        case 0:
            cout << "Now exiting. Goodbye!";
            break;
        case 1:
            cout << "Challenge 1: Convert hex to base64";
               //Request hex number
              string hexInput;
              while (true)
              {
              // Request hex number
              cout << "Enter a hexadecimal number: ";
              cin >> hexInput;
              // Validate input
              if (isValidHex(hexInput))
              {
                  break;
              }
              else
              {
                  cout << "Error: Invalid hexadecimal input." << endl;
              }
              }
          
              // Show the input and output
              cout << "Hex input: " << hexInput << endl;
              cout << "Base64 output: " << hexToBase64(hexInput) << endl;
          
              return 0;
                 break;


        case 2:
        {
            cout << "Challenge 2: Fixed XOR";
            cout << endl;
            std::string temp1;
            std::string temp2;
            std::cout << "Enter string 1: ";
            std::cin >> temp1;

            std::cout << "Enter string 2: ";
            std::cin >> temp2;

            while ((temp1.length() != temp2.length())
                || (temp1.length() == 0 || temp2.length() == 0)
                || (!isValidHex(temp1) || !isValidHex(temp2)))
            {
                std::cout << "\nERROR: Invalid strings.\n\n";

                std::cout << "Enter string 1: ";
                std::cin >> temp1;

                std::cout << "Enter string 2: ";
                std::cin >> temp2;
            }

            std::cout << "\nXOR: " << fixedXOR(temp1, temp2);
        }
            break;


        case 3:
            cout << "Challenge 3: Single-byte XOR cipher";
            break;
        case 4:
            cout << "Challenge 4: Detect single-character XOR";
            break;
        case 5:
            cout << "Challenge 5: Implement repeating-key XOR";
            break;
        case 6:
            cout << "Challenge 6: Break repeating-key XOR";
            break;
        case 7:
            cout << "Challenge 7: AES in ECB mode";
            break;
        case 8:
            cout << "Challenge 8: Detect AES in ECB mode";
            break;
        default:
            cout << "Invalid Input. Please choose a challenge or enter 0 to exit!";
            break;
        }

        cout << endl;

        //Wait for user input to continue to next challenge.

        #if __linux__ || __APPLE__
                system("read");
        #elif _WIN32 || _WIN64
                system("pause");
        #endif

        //Again, to clear the console after user is ready.

        #if __linux__ || __APPLE__
                system("clear");
        #elif _WIN32 || _WIN64
                system("cls");
        #endif
    }

    return 0;
}

// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
