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

#include "aes_128_ecb.h" //challenge 7 dependencies
#include "base64.h"     //challenge 7 dependencies
 
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

// Encrypts with repeating key XOR
void repeatingKeyXor(const char* plaintext, const char* key, char* ciphertext) {
    size_t keyLength = strlen(key);
    size_t plaintextLength = strlen(plaintext);

    // Iterate over each character in the plaintext
    for (size_t i = 0; i < plaintextLength; ++i) {
        ciphertext[i] = plaintext[i] ^ key[i % keyLength];  // XOR with repeating key
    }

    // Null-terminate the ciphertext
    ciphertext[plaintextLength] = '\0';
}

// Converts the encrypted string to hex
void hexEncode(const char* input, char* output) {
    static const char* const lut = "0123456789abcdef";

    // Loop through each character in the input string
    for (int i = 0; input[i] != '\0'; i++) {
        output[i * 2] = lut[(input[i] & 0xF0) >> 4]; // Appends higher 4 bits
        output[i * 2 + 1] = lut[(input[i] & 0x0F)];  // Appends lower 4 bits
    }
    output[strlen(input) * 2] = '\0'; // Null-terminate hex output
}

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
        cout << endl;
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
                    {
                        // Allows the input of multiple lines
                        std::string plaintext;
                        std::string key = "ICE";
                        std::string line;
    
                        std::cout << "Challenge 5: Implement repeating-key XOR \nEnter the plaintext (type 'END' on a new line to finish):\n";
    
                        // Clear the input buffer before using getline
                        std::cin.ignore();  // Ignore any leftover newlines in the buffer

                        // Reads the input line by line
                        while (true) {
                            std::getline(std::cin, line);  // Read a line
                            // Once END is typed the loop is broken
                            if (line == "END") {
                                break;
                            }

                            // Append the line to the plaintext string
                            plaintext += line + "\n"; // Adds a newline between the lines
                        }

                        // Remove the last newline character if it exists
                        if (!plaintext.empty() && plaintext[plaintext.size() - 1] == '\n') {
                            plaintext.pop_back();
                        }
  
                        const char* plaintextCStr = plaintext.c_str();

                        char ciphertext[3000];
                        char hexCiphertext[6000];

                        repeatingKeyXor(plaintextCStr, key.c_str(), ciphertext); // Encrypt text
                        hexEncode(ciphertext, hexCiphertext); // Convert to hex

                        printf("Repeating Key XOR Encryption:\n%s\n", hexCiphertext);
                    }
                    break;
                case 6:
                {
                    cout << "Challenge 6: Break repeating-key XOR";
                    
                    string key;
                    string filename = "Challenge6Text.txt"; //file to decode
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
                }
                    break;
                    
                case 7:
                    cout << "Challenge 7: AES in ECB mode" << endl;
                    {
                        AES_CTX ctx;
                        std::string outStr = R"(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)";
                        unsigned char key[] = "YELLOW SUBMARINE";

                        size_t outLen = outStr.length();

                        char* outPtr = new char[outLen + 1];
                        char* finalPtr = new char[outLen + 1];
                        for (int i = 0; i < outLen; i++)
                        {
                            *(outPtr + i) = outStr.at(i);
                        }

                        base64Class base64(outPtr, outLen);
                        base64.decode();
                        outPtr = base64.getResult();

                        AES_DecryptInit(&ctx, key);
                        AES_Decrypt(&ctx, reinterpret_cast<const unsigned char*>(outPtr), outLen, (unsigned char*)finalPtr);

                        for (int i = 0; i < outLen - 960; i++) //should've used OpenSSL after all
                        {
                            std::cout << *(finalPtr + i);
                        }

                        delete[] outPtr;
                        delete[] finalPtr;
                        AES_CTX_Free(&ctx);
                    }
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
