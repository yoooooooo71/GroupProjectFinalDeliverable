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
// compute hamming distance between 2 strings
int hamDistance(string a, string b)
{
    int count = 0;
    for (int i = 0; i < a.size(); i++)
    {
        //adds integer leaving off least significant byte
        int byte = (a[i] & 0xFF) ^ (b[i] & 0xFF);
        while (byte)
        {
            count += byte & 1;
            byte = byte >> 1;
        }
    }
    return count;
}

string decode_64(const string& in) //function to decode base 64
{
    string out;
    vector<int> T(256, -1);
    for (int i = 0; i < 64; i++) //base64 characters
        T["ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/"[i]] = i;

    int val = 0, //value of decoded bits
    valb = -8;  //keeps track of how many bits
    for (unsigned char c : in)
    {
        if (T[c] == -1) break;
        val = (val << 6) + T[c];
        valb += 6;
        if (valb >= 0)
        {
            out.push_back(char((val >> valb) & 0xFF));
            valb -= 8;
        }
    }
    return out;
}

string repeatingKeyXOR(const string& plaintext, const string& key) //repeating key function
{
    string ciphertext = "";
    int keyIndex = 0;

    // loop through each character
    for (char c : plaintext) {
        ciphertext += c ^ key[keyIndex]; // XOR character with key character
        keyIndex = (keyIndex + 1) % key.size();
    }
    return ciphertext;
}

//accepts encoded data
int getKeyLength(const string& data)
{
    int bestKeyLength = 2; //smallest hamming distance
    double bestScore = 1e9; /*initialize bestcore to very large value to make sure
                              any avg hamming distance will be smaller than this
                            */
    for (int keyLength = 2; keyLength <= 40; keyLength++) //iterate through key length 2-40
    {
        double totalDistance = 0;
        int numComparisons = 0;

        // Breaks data into chunks of keyLength size
        for (int i = 0; i + 2 * keyLength <= data.size(); i += keyLength) 
        {
            string chunk1 = data.substr(i, keyLength);
            string chunk2 = data.substr(i + keyLength, keyLength);
            totalDistance += hamDistance(chunk1, chunk2) / (double)keyLength;
            numComparisons++;
        }
        //calculate average normalized hamming distance
        double avg_hamDistance = totalDistance / numComparisons; 

        if (avg_hamDistance < bestScore) //if keylength has lower avg distance it updates..
        {                                //..bestkeylength and bestscore
            bestScore = avg_hamDistance;
            bestKeyLength = keyLength;
        }
    }
    return bestKeyLength;
}

string singleByte(string str, char key)
{
    string strXOR(str.size(), '\0'); //initialize string

    for (int i = 0; i < str.size(); ++i) //loop goes through each character in str
    {                                    //does xor operation and stores it in strXor
        strXOR[i] = str[i] ^ key;
    }
    return strXOR;
}

char findKey(string cipherBlock)
{   
    int max = 0;
    char key = 0;
    string dataDecoded;

    for (int i = 0; i <= 256; i++) //loop through 256 possibilities
    {
        char ch = (char)i;
        string attempt = singleByte(cipherBlock, ch); //single byte xor

        int count = 0;
        //finds characters and increments
        for (int j = 0; j < attempt.size(); j++)
        {
            if ((attempt[j] >= 65 && attempt[j] <= 122) || attempt[j] == ' ')
            {
                count++;
            }
        }
        //highest count is the key
        if (count > max)
        {
            max = count;
            key = ch;
        }
    }
    return key;
}

int mainFunction()
{
    string key;
    string filename = "6.txt"; //file to decode
    ifstream infile(filename);
    if (!infile.is_open())
    {
        cout << "Error: Unable to open file " << filename << endl;
        return 1;
    }

    string line;
    string data;

    // Read each line of file
    while (getline(infile, line))
    {
        // Append each line to the encrypted data
        data += line;
    }
    infile.close();

    string decoded_data = decode_64(data); //decode file
    int keyLength = getKeyLength(decoded_data); //find key length of decoded data
    cout << "Key length: " << keyLength << endl;
   
    int blocks = decoded_data.size() / keyLength;

    //transpose blocks 
    for (int i = 0; i < keyLength; ++i) 
    {
        string block;
        char indexKey = '/0';

        for (int j = 0; j < blocks; j++) 
        {
            block += decoded_data.substr((j * keyLength) + i, 1);
        }
        //puts together characters to make the key
        key += findKey(block);
    }
    cout << "KEY: " << key << endl;
    string decrypted_data = repeatingKeyXOR(decoded_data, key); //use repeating key xor to decrypt the string

    // Output the decrypted data
    cout << "Decrypted File:\n" << decrypted_data << endl;

    return 0;
}

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
