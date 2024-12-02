/*
CSC 414 Intro to Software Engineering H001
Group 3 Project

Solving CryptoPals Crypto Challenge

Group Members: Michael Brown, Chance Davis,
    Kristen Gilmer, Rubina Guragain, Patrick Heard,
    Feizal Mjidho, Railyn Wingfield
*/

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
#include <unordered_set>
#include <algorithm>
 
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

vector<tuple < char, float> > frequencies; // A global variable to keep track of the average expect frequency of each character

/*
    Reads a text file of a book determine the average expected frequency of occurance for each character that is considered
*/
vector <tuple <char, float> > getFrequencies(string fileName) {
    vector <tuple <char, float>> frequencies;
    for (char letter = 32; letter < 127; letter++) {
        ifstream file(fileName);
        char c; //Specific character within the text file
        float length = 0; // total number of characters in the text file
        float total = 0; // total occurances of 'letter' within the text document
        float freq; // calculated frequency of 'letter' in the text document
        while (file.get(c)) {
            length++; // adding 1 for each character that is seen
            if (letter == c)
                total++; // adding 1 for each time 'letter' appears in the text document
        }
        freq = total / length;
        frequencies.push_back(make_tuple(letter, freq)); // Pushing the letter and frequency to the frequency vector for later reference
        file.close(); // closing the file to be opened again in the next loop
    }
    return frequencies;
}

/*
    Takes a string and gives it a score based on how often each character within it appears.
    The lower the score, the less likely it is to consist of random characters.
*/
float textScore(string text) {
    float score = 0; // score of the string
    float length = text.length();

    // Traversing through each character within the frequency vector
    for (tuple <char, float> letter : frequencies) {
        float total = 0; // total number of times that the letter appears in the string
        for (char c : text) {
            if (c == get<0>(letter))
                total++; // adds one each time the specified charecter in the frequency vector appears in the string
        }
        float frequency = total / length;
        float error = abs(frequency - get<1>(letter)); // takes the calculated frequcny of the character int he string and compares it to the expected frequency from the text document
        score += error;

    }
    return score;
}

/*
    Takes an encoded string in hexadecimal format and returns the key character, the score and the decoded message as a tuple
*/
tuple<unsigned char, float, string> crackCipher(string text) {
    vector<uint8_t> rawBytes = hexToBytes(text); // converts the string into bytes
    tuple <unsigned char, float, string> best = make_tuple(255, 999, ""); // creates an arbitrary best guess to start

    // tring out different characters starting at the SPACE character and ending at the ~ character
    for (unsigned char i = 32; i < 127; i++) {
        string plainText = ""; // an empty string to hold the decoded message

        // looping through each byte of the vector
        for (int j = 0; j < rawBytes.size(); j++)
        {
            plainText += i ^ rawBytes[j]; // trying to decode the raw bytes with a character and adding it to plainText
        }
        float score = textScore(plainText); // getting the score for the message

        best = (get<1>(best) < score) ? best : make_tuple(i, score, plainText); // seeing if the calcualted score is the best score
    }
    return best;
}




//Challenge 4 Functions

vector <tuple <unsigned char, float, string>> scoreString(string fileName) {
    ifstream file(fileName);
    string line = "";
    vector<tuple<unsigned char, float, string>> output;
    while (getline(file, line)) {
        output.push_back(crackCipher(line));
    }
    return output;
}


void printDecodedString(string fileName) {
    vector<tuple<unsigned char, float, string>> messages = scoreString(fileName);
    tuple<unsigned char, float, string> best = make_tuple(255, 999, "");
    for (tuple<unsigned char, float, string> message : messages) {
        best = (get<1>(best) < get<1>(message)) ? best : message;
    }
    cout << get<0>(best) << " : " << get<2>(best) << endl;
}

//Challenge 5 Functions

//Challenge 6 Functions

//Challenge 7 Functions

//Challenge 8 Functions

int hasDuplicateBlocks(const vector<uint8_t>& bytes, size_t blockSize) {
    unordered_set<string>blocks;
    int dupes = 0;
    for (size_t i = 0; i < bytes.size(); i += blockSize) {
        string block(bytes.begin() + i, bytes.begin() + i + blockSize);
        if (blocks.find(block) != blocks.end()) {
            dupes++;
        }
        blocks.insert(block);
    }
  
    return dupes;
}




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
                {
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
                }

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
                {
                    cout << "Challenge 3: Single-byte XOR cipher" << endl;
                    string cipherText;
                    cout << "Enter the encrypted string: ";
                    cin >> cipherText;
                    cout << endl;
                    cout << "Cracking the cipher (this may take a minute):  " << endl;
                    frequencies = getFrequencies("Challenge3Text.txt");
                    tuple<unsigned char, float, string> key = crackCipher(cipherText);
                    cout << "Key: " << get<0>(key) << "\nMessage: " << get<2>(key) << endl;
                    break;
                }

                case 4:
                {
                    cout << "Challenge 4: Detect single-character XOR" << endl;
                    cout << "Cracking the cipher (this may take a minute):  " << endl;
                    frequencies = getFrequencies("Challenge3Text.txt");
                    printDecodedString("Challenge4Text.txt");
                    break;
                }

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
                {
                    cout << "Challenge 8: Detect AES in ECB mode";
                    ifstream file("Challenge8Text.txt");
                    if (!file.is_open()) {
                        cerr << "Failed to open Challenge8Text.txt" << endl;
                        return 1;
                    }

                    string line;
                    size_t blockSize = 16; // AES block size in bytes
                    int lineNum = 0;

                    while (getline(file, line)) {
                        lineNum++;
                        vector<uint8_t> bytes = hexToBytes(line);
                        int dupes = hasDuplicateBlocks(bytes, blockSize);
                        if (dupes) {
                            cout << "Detected ECB mode in line: " << lineNum << " containing " << dupes << " duplicates." << endl;
                            break;
                        }
                    }
                

                file.close();
                break;
                }
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
