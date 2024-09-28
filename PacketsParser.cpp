#include <iostream>
#include <fstream>
#include <iomanip>
#include <vector>
#include <algorithm> // for std::remove_if

// Function to convert a hex character (ASCII) to its integer equivalent
int hexCharToInt(unsigned char c) {
    if (c >= '0' && c <= '9')
        return c - '0';         // Converts '0'-'9' to 0-9
    else if (c >= 'a' && c <= 'f')
        return c - 'a' + 10;    // Converts 'a'-'f' to 10-15
    else if (c >= 'A' && c <= 'F')
        return c - 'A' + 10;    // Converts 'A'-'F' to 10-15
        
    return -1;  // Error case for invalid hex digit
}

void printUnparsed(const std::vector<unsigned char>& data) {
    std::cout << std::dec << std::endl;
    for (size_t i = 0; i < data.size(); i++) {
        if ( (i)%8 == 0) std::cout << std::endl;
        else if ( (i)%2 == 0) std::cout << " ";
        std::cout << std::hex << (unsigned char)data[i];
    }
    std::cout << std::dec << std::endl;
}

void printHex(const std::vector<unsigned char>& data, int start, int length) {
    std::cout << "0x";
    for (int i = start; i < start + length; i++) {
        std::cout << std::hex << (unsigned char)data[i];
    }
    std::cout << std::dec << std::endl;
}

void printPayload(const std::vector<unsigned char>& data, int start, int length) {
    std::cout << "0x";
    for (int i = start; i < start + length; i++) {
        if ( (i)%2 == 0) std::cout << " ";
        std::cout << std::hex << (unsigned char)data[i];
    }
    std::cout << std::dec << std::endl;
}

int main() {
    // Open the .bin file in binary mode
    std::ifstream infile("ethernet_packet.bin", std::ios::binary);
    if (!infile) {
        std::cerr << "Error opening file!" << std::endl;
        return 1;
    }

    // Read the file contents into a vector
    std::vector<unsigned char> fileData((std::istreambuf_iterator<char>(infile)), std::istreambuf_iterator<char>());

    // Close the file
    infile.close();

    // Remove spaces and newline characters
    fileData.erase(
        std::remove_if(fileData.begin(), fileData.end(), [](unsigned char c) {
            return c == ' ' || c == '\n';  // Remove spaces and newlines
        }),
        fileData.end()
    );

    // Print the original shape of the file
    // printUnparsed(fileData);

    const int PREAMBLE_SIZE = 8*2;
    const int DEST_SIZE = 6*2;
    const int SRC_SIZE = 6*2;
    const int LEN_SIZE = 2*2;
    const int CRC_SIZE = 4*2;
    const int HEADER_SIZE = PREAMBLE_SIZE + DEST_SIZE + SRC_SIZE + LEN_SIZE;

    size_t i = 0;
    while (i < fileData.size()) {
        // Check if we're at an IFG line
        if (fileData[i] == '0' && fileData[i+1] == '7' && fileData[i+2] == '0' && fileData[i+3] == '7' 
        && fileData[i+4] == '0' && fileData[i+5] == '7' && fileData[i+6] == '0' && fileData[i+7] == '7') {
            // Skip IFG
            i += 8;
            continue;
        }

        // Extract the Ethernet frame fields (preamble can be skipped)
        if (i + HEADER_SIZE <= fileData.size()) {
            // Skip the preamble (8 bytes)
            i += PREAMBLE_SIZE;

            // Destination address (6 bytes)
            std::cout << "Destin: ";
            printHex(fileData, i, DEST_SIZE);
            i += DEST_SIZE;

            // Source address (6 bytes)
            std::cout << "Source: ";
            printHex(fileData, i, SRC_SIZE);
            i += SRC_SIZE;

            // Length field (2 bytes)
            // Convert length from bytes to integer
            int length = (hexCharToInt(fileData[i])  << 12) |   // Shift the first nibble by 12 bits
                        (hexCharToInt(fileData[i+1]) << 8) |   // Shift the second nibble by 8 bits
                        (hexCharToInt(fileData[i+2]) << 4) |   // Shift the third nibble by 4 bits
                        hexCharToInt(fileData[i+3]);           // No shift for the fourth nibble
            std::cout << "Length: " << length << " or ";
            printHex(fileData, i, LEN_SIZE);
            i += LEN_SIZE;

            // Calculate payload size
            int payloadSize = length*2 - (HEADER_SIZE + CRC_SIZE);

            // Extract and print payload
            std::cout << "Payload: ";
            if (i + payloadSize <= fileData.size()) {
                printPayload(fileData, i, payloadSize);
                i += payloadSize;
            } else {
                std::cout << "Incomplete payload (file truncated)." << std::endl;
                break;
            }

            // CRC (4 bytes)
            std::cout << "CRC: ";
            printHex(fileData, i, CRC_SIZE);
            i += CRC_SIZE;

            std::cout << "---------------------------------" << std::endl;
        } else {
            // If the remaining data is too short to form a full Ethernet frame, we stop
            std::cout << "Incomplete packet data, stopping parsing." << std::endl;
            break;
        }
    }


    // Stop program from closing
    std::cout << "Press any key to continue...";
    std::cin.get();     // Consume the newline character
    return 0;
}
