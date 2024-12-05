/*
Written by ninjs-grey @ https://github.com/AlexBestoso/Base64/tree/main.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BASE_64_INVERSE_ERROR -1
#define BASE_64_PADDING -2
class base64Class {
private:
	/*
	 * Variables used throughout the class
	 * */
	char* inputBuffer = NULL;
	size_t inputBufferSize = 0;

	char* outputBuffer = NULL;
	size_t outputBufferSize = 0;

	const char encodeTable[64] = {
		'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H',
		'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P',
		'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X',
		'Y', 'Z', 'a', 'b', 'c', 'd', 'e', 'f',
		'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n',
		'o', 'p', 'q', 'r', 's', 't', 'u', 'v',
		'w', 'x', 'y', 'z', '0', '1', '2', '3',
		'4', '5', '6', '7', '8', '9', '+', '/'
	};



	/*
	 * Encoding-Specific Functions
	 * */
	int encodeBuildProcessBuffer(char encodeBuffer[3]) {
		int processBuffer = 0;
		processBuffer = (processBuffer ^ encodeBuffer[0]) << 8;
		processBuffer = (processBuffer ^ encodeBuffer[1]) << 8;
		processBuffer = (processBuffer ^ encodeBuffer[2]);
		return processBuffer;
	}

	void encodeBuildConversionBuffer(char conversionBuffer[4], int processBuffer, int leftOvers) {
		int shifter = 18;
		int shiftMovement = 6;
		unsigned int ander = 0b111111000000000000000000;
		for (int j = 0; j < 4; j++) {
			if (leftOvers == 2 && j >= 3) {
				conversionBuffer[3] = '=';
				break;
			}
			else if (leftOvers == 1 && j >= 2) {
				conversionBuffer[2] = '=';
				conversionBuffer[3] = '=';
				break;
			}
			conversionBuffer[j] = this->encodeTable[(processBuffer & ander) >> shifter];
			ander = ander >> shiftMovement;
			shifter -= shiftMovement;
		}
	}
	void encodeStoreResult(char conversionBuffer[4]) {
		if (this->outputBuffer == NULL) {
			this->outputBufferSize = 4;
			this->outputBuffer = (char*)malloc(this->outputBufferSize + 1); // +1 for null termination.
			for (int j = 0; j < this->outputBufferSize; j++)
				this->outputBuffer[j] = conversionBuffer[j];
			this->outputBuffer[this->outputBufferSize] = 0x00; // Null terminate result
		}
		else {
			this->outputBufferSize += 4;
			this->outputBuffer = (char*)realloc(this->outputBuffer, this->outputBufferSize + 1);
			for (int j = this->outputBufferSize - 4; j < this->outputBufferSize; j++)
				this->outputBuffer[j] = conversionBuffer[j % 4];
			this->outputBuffer[this->outputBufferSize] = 0x00; // Null terminate result
		}
	}

	/*
	 * Decoding-Specfic Functions
	 * */
	int inverseEncodeTable(char value) {
		if (value == '=') {
			return BASE_64_PADDING;
		}
		for (int i = 0; i < 64; i++) {
			if (value == encodeTable[i]) {
				return i;
			}
		}
		return BASE_64_INVERSE_ERROR;
	}

	int decodeBuildProcessBuffer(int decodeBuffer[4]) {
		int processBuffer = 0;
		processBuffer = (processBuffer ^ (decodeBuffer[0] & 0b00111111)) << 6;
		processBuffer = (processBuffer ^ (decodeBuffer[1] & 0b00111111)) << 6;
		processBuffer = (processBuffer ^ (decodeBuffer[2] & 0b00111111)) << 6;
		processBuffer = processBuffer ^ (decodeBuffer[3] & 0b00111111);
		return processBuffer;
	}

	void decodeBuildConversionBuffer(char conversionBuffer[3], int processBuffer) {
		conversionBuffer[0] = (processBuffer & 0b111111110000000000000000) >> 16;
		conversionBuffer[1] = (processBuffer & 0b000000001111111100000000) >> 8;
		conversionBuffer[2] = (processBuffer & 0b000000000000000011111111);
	}

	void decodeStoreResult(char conversionBuffer[3], int paddingCount) {
		if (this->outputBuffer == NULL) {
			this->outputBufferSize = 3 - paddingCount;
			this->outputBuffer = (char*)malloc(this->outputBufferSize);
			for (int j = 0; j < this->outputBufferSize; j++)
				this->outputBuffer[j] = conversionBuffer[j];
		}
		else {
			this->outputBufferSize += 3 - paddingCount;
			this->outputBuffer = (char*)realloc(this->outputBuffer, this->outputBufferSize);
			for (int j = this->outputBufferSize - 3 + paddingCount; j < this->outputBufferSize; j++) {
				this->outputBuffer[j] = conversionBuffer[j % 3];
			}
		}
	}

public:
	base64Class() { }
	base64Class(char* input, size_t inputSize) {
		this->inputBufferSize = inputSize;
		this->inputBuffer = (char*)malloc(this->inputBufferSize);
		for (int i = 0; i < this->inputBufferSize; i++)
			this->inputBuffer[i] = input[i];
	}

	char* getResult(void) {
		return this->outputBuffer;
	}
	size_t getResultSize(void) {
		return this->outputBufferSize;
	}
	void printResult(void) {
		for (int i = 0; i < this->outputBufferSize; i++) {
			printf("%c", this->outputBuffer[i]);
		}
	}

	int decode(void) {
		if (this->inputBufferSize <= 0) {
			fprintf(stderr, "(base64Class.decode | Error )> InputBufferSize is not greater than 0\n");
			return EXIT_FAILURE;
		}

		if (this->inputBuffer == NULL) {
			fprintf(stderr, "(base64Class.decode | Error )> InputBuffer is null\n");
			return EXIT_FAILURE;
		}

		if (this->outputBuffer != NULL)
			free(this->outputBuffer);
		this->outputBufferSize = 0;

		int decodeBuffer[4] = { 0 };
		int paddingCount = 0;
		for (int i = 0; i < this->inputBufferSize; i++) {
			/* Get Inverse Table Value */
			decodeBuffer[i % 4] = this->inverseEncodeTable(this->inputBuffer[i]);
			/* Account For Padding Chars */
			if (decodeBuffer[i % 4] == BASE_64_PADDING) {
				paddingCount++;
				if (paddingCount >= 3) {
					fprintf(stderr, "(base64Class::decode | Error )> Too many padding chars, improper base64 encoding.\n");
					return EXIT_FAILURE;
				}
			}
			else if (decodeBuffer[i % 4] == BASE_64_INVERSE_ERROR) {
				fprintf(stderr, "(base64Class::decode | Error )> Invalid value sent to decode table.\n");
				return EXIT_FAILURE;
			}

			if ((i % 4) == 3) {
				/* Build Process Buffer */
				int processBuffer = this->decodeBuildProcessBuffer(decodeBuffer);

				/* Build Conversion Buffer */
				char conversionBuffer[3] = { 0 };
				this->decodeBuildConversionBuffer(conversionBuffer, processBuffer);

				/* Store result in the output buffer. */
				this->decodeStoreResult(conversionBuffer, paddingCount);

				/* Reset Buffers */
				decodeBuffer[0] = 0x00;
				decodeBuffer[1] = 0x00;
				decodeBuffer[2] = 0x00;
				decodeBuffer[3] = 0x00;
				/* Stop processing if padding is found. */
				if (paddingCount > 0) {
					break;
				}
			}
		}
	}

	int encode(void) {
		if (this->inputBufferSize <= 0) {
			fprintf(stderr, "(base64Class.encode | Error )> InputBufferSize is not greater than 0\n");
			return EXIT_FAILURE;
		}

		if (this->inputBuffer == NULL) {
			fprintf(stderr, "(base64Class.encode | Error )> InputBuffer is null\n");
			return EXIT_FAILURE;
		}

		if (this->outputBuffer != NULL)
			free(this->outputBuffer);
		this->outputBufferSize = 0;

		char encodeBuffer[3] = { 0 };
		int leftOvers = 0;
		for (int i = 0; i < this->inputBufferSize; i++) {
			encodeBuffer[i % 3] = this->inputBuffer[i];
			if ((i % 3) == 2) {
				/* Concationate the three found binary values. */
				int processBuffer = this->encodeBuildProcessBuffer(encodeBuffer);

				/* Convert the binary values into 4 new binary numbers.*/
				char conversionBuffer[4] = { 0 };
				this->encodeBuildConversionBuffer(conversionBuffer, processBuffer, 0);

				/* Store result in the output buffer. */
				this->encodeStoreResult(conversionBuffer);

				/* Reset values for loop */
				leftOvers = 0;
				encodeBuffer[0] = 0x00;
				encodeBuffer[1] = 0x00;
				encodeBuffer[2] = 0x00;
			}
			else {
				leftOvers++;
			}
		}

		if (leftOvers <= 0)
			return EXIT_SUCCESS;

		/* Concationate the three found binary values. */
		int processBuffer = this->encodeBuildProcessBuffer(encodeBuffer);

		/* Convert the binary values into 4 new binary numbers.*/
		char conversionBuffer[4] = { 0 };
		this->encodeBuildConversionBuffer(conversionBuffer, processBuffer, leftOvers);

		/* Store result in the output buffer. */
		this->encodeStoreResult(conversionBuffer);

		return EXIT_SUCCESS;
	}

	void setInputs(char* input, size_t inputSize) {
		this->inputBufferSize = inputSize;
		this->inputBuffer = (char*)malloc(inputSize);
		for (int i = 0; i < inputSize; i++)
			this->inputBuffer[i] = input[i];
	}

	void freeClass(void) {
		if (this->inputBuffer != NULL)
			free(this->inputBuffer);
		this->inputBufferSize = 0;

		if (this->outputBuffer != NULL)
			free(this->outputBuffer);
		this->outputBufferSize = 0;
	}
};
