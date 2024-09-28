#include <iostream>
#include <fstream>
#include <iomanip>
#include <vector>
#include <algorithm> // for remove_if

using namespace std;

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

void printUnparsed(const vector<unsigned char>& data) {
    cout << endl;
    for (size_t i = 0; i < data.size(); i++) {
        if ( (i)%8 == 0) cout << endl;
        else if ( (i)%2 == 0) cout << " ";
        cout << hex << (unsigned char)data[i];
    }
    cout << dec << endl;
}

void printHex(const vector<unsigned char>& data, int start, int length) {
    cout << "0x";
    for (int i = start; i < start + length; i++) {
        cout << hex << (unsigned char)data[i];
    }
    cout << endl;
}

void printPayload(const vector<unsigned char>& data, int start, int length) {
    for (int i = start; i < start + length; i++) {
        if (i%4 == 0) cout << "\n            ";
        if (i%2 == 0) cout << " ";
        cout << hex << (signed char)data[i];
    }
    cout << endl;
}

int main() {
    // Open the .bin file in binary mode
    ifstream infile("oran_packet.bin", ios::binary);
    if (!infile) {
        cerr << "Error opening file!" << endl;
        return 1;
    }

    // Read the file contents into a vector
    vector<unsigned char> fileData((istreambuf_iterator<char>(infile)), istreambuf_iterator<char>());

    // Close the file
    infile.close();

    // Remove spaces and newline characters
    fileData.erase(
        remove_if(fileData.begin(), fileData.end(), [](unsigned char c) {
            return c == ' ' || c == '\n';  // Remove spaces and newlines
        }),
        fileData.end()
    );

    // Print the original shape of the file
    // printUnparsed(fileData);

    const int PREAMBLE = 8*2;
    const int DEST = 6*2;
    const int SRC = 6*2;
    const int LEN = 2*2;
    const int ETH_HEADER = PREAMBLE + DEST + SRC + LEN;

    const int ECPRI_VRC = 1*2;
    const int ECPRI_MSG = 1*2;
    const int ECPRI_PAY = 2*2;
    const int ECPRI_RPC = 2*2;
    const int ECPRI_SEQ = 2*2;
    const int ECPRI_HEADER = ECPRI_VRC+ECPRI_MSG+ECPRI_PAY+ECPRI_RPC+ECPRI_SEQ;

    const int ORAN_DVI = 1*2;
    const int ORAN_FID = 1*2;
    const int ORAN_SSS = 2*2;
    const int ORAN_HEADER = ORAN_DVI+ORAN_DVI+ORAN_SSS;

    const int ORAN_SID = 1*2;
    const int ORAN_IGN = 1*2;
    const int ORAN_strPRB = 1*2;
    const int ORAN_numPRB = 1*2;
    const int ORAN_SECTION = ORAN_SID+ORAN_IGN+ORAN_strPRB+ORAN_numPRB;

    const int FULL_HEADERS = ETH_HEADER + ECPRI_HEADER + ORAN_HEADER + ORAN_SECTION;

    const int CRC = 4*2;

    size_t i = 0;
    while (i < fileData.size()) {
        // Check if we're at an IFG line
        if (fileData[i] == '0' && fileData[i+1] == '7' && fileData[i+2] == '0' && fileData[i+3] == '7' 
        && fileData[i+4] == '0' && fileData[i+5] == '7' && fileData[i+6] == '0' && fileData[i+7] == '7') {
            // Skip IFG
            i += 8;
            continue;
        }

        // Extract the Ethernet Packet Fields (preamble can be skipped)
        if (i + ETH_HEADER <= fileData.size()) {
            // Skip the preamble (8 bytes)
            cout << "   > ETHERNET HEADER <" << endl;
            i += PREAMBLE;

            // Destination address (6 bytes)
            cout << "Destination: ";
            printHex(fileData, i, DEST);
            i += DEST;

            // Source address (6 bytes)
            cout << "     Source: ";
            printHex(fileData, i, SRC);
            i += SRC;

            // Length field (2 bytes) - Convert length from bytes to integer
            int length = (hexCharToInt(fileData[i])  << 12) |   // Shift the first nibble by 12 bits
                        (hexCharToInt(fileData[i+1]) << 8) |   // Shift the second nibble by 8 bits
                        (hexCharToInt(fileData[i+2]) << 4) |   // Shift the third nibble by 4 bits
                        hexCharToInt(fileData[i+3]);           // No shift for the fourth nibble
            cout << "     Length: " << length << " or ";
            printHex(fileData, i, LEN);
            i += LEN;

            // Start of eCPRI Header
            cout << endl << "      > ECPRI HEADER <" << endl;

            // Version, Reserved, Concatenation
            cout << "        VRC: ";
            printHex(fileData, i, ECPRI_VRC);
            i += ECPRI_VRC;

            // Message
            cout << "    Message: ";
            printHex(fileData, i, ECPRI_MSG);
            i += ECPRI_MSG;

            // Payload Size
            cout << "    Payload: ";
            printHex(fileData, i, ECPRI_PAY);
            i += ECPRI_PAY;

            // RtcId / PcId
            cout << " RtcId/PcId: ";
            printHex(fileData, i, ECPRI_RPC);
            i += ECPRI_RPC;

            // Sequence Id
            cout << "      SeqId: ";
            printHex(fileData, i, ECPRI_SEQ);
            i += ECPRI_SEQ;

            // Start of ORAN Header
            cout << endl << "       > ORAN HEADER <" << endl;

            // data Direction, payloadVersion, filterIndex
            cout << "        DVI: ";
            printHex(fileData, i, ORAN_DVI);
            i += ORAN_DVI;

            // Frame Id
            cout << "      Frame: ";
            printHex(fileData, i, ORAN_FID);
            i += ORAN_FID;

            // subframe, slot, symbol
            cout << "        SSS: ";
            printHex(fileData, i, ORAN_SSS);
            i += ORAN_SSS;

            // Start of ORAN Section
            cout << endl << "       > ORAN SECTION <" << endl;

            // Section Id
            cout << "        SID: ";
            printHex(fileData, i, ORAN_SID);
            i += ORAN_SID;

            // Ignored
            cout << "    Ignored: ";
            printHex(fileData, i, ORAN_IGN);
            i += ORAN_IGN;

            // subframe, slot, symbol
            cout << "  Start PRB: ";
            printHex(fileData, i, ORAN_strPRB);
            i += ORAN_strPRB;

            // subframe, slot, symbol
            cout << " PRB Number: ";
            printHex(fileData, i, ORAN_numPRB);
            i += ORAN_numPRB;

            // Calculate payload size
            int payloadSize = length*2 - (FULL_HEADERS + CRC);

            // Extract and print payload
            cout << endl << "    Payload: ";
            if (i + payloadSize <= fileData.size()) {
                printPayload(fileData, i, payloadSize);
                i += payloadSize;
            } else {
                cout << "Incomplete payload (file truncated)." << endl;
                break;
            }

            // CRC (4 bytes)
            cout << "CRC: ";
            printHex(fileData, i, CRC);
            i += CRC;

            cout << "---------------------------------" << endl;
        } else {
            // If the remaining data is too short to form a full Ethernet frame, we stop
            cout << "Incomplete packet data, stopping parsing." << endl;
            break;
        }
    }


    // Stop program from closing
    cout << "Press any key to continue...";
    cin.get();     // Consume the newline character
    return 0;
}
