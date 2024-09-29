#ifndef ORANPACKET_H
#define ORANPACKET_H

#include <iostream>
#include <vector>
#include <string>
#include <iostream>
#include <fstream>  // file printing
#include <sstream>  // file reading
#include <iomanip>
#include <cstring>
#include <string>
#include <stdint.h>
#include <cmath>    // ceil()

using namespace std;

// Define Ethernet frame sizes
#define ETH_HDRLEN 22       // Ethernet header length (including Preamble, SFD, and Length)
#define eCPRI_HDRLEN 8      // eCPRI header length
#define ORAN_HDRLEN 4       // ORAN header length
#define ORAN_SECLEN 4       // ORAN section length
#define CRC_LEN 4           // Cyclic Redundancy Check (4 bytes)
#define IFG 0x07            // IFG byte shape

// Structure for Ethernet header
struct Headers {
    unsigned char preamble[7];  // Synchronization
    unsigned char sfd[1];       // Start of Frame Delimiter
    unsigned char dest_mac[6];  // Destination MAC address
    unsigned char src_mac[6];   // Source MAC address
    unsigned char length[2];    // Ethertype Packet Length

    unsigned char eCPRI_VRC[1];    // eCPRI Version, Reserved, and Concatenation
    unsigned char eCPRI_Msg[1];    // eCPRI Message
    unsigned char eCPRI_Pay[2];    // eCPRI Payload 
    unsigned char eCPRI_RPc[2];    // eCPRI RtcId and PcId
    unsigned char eCPRI_Seq[2];    // eCPRI SeqId

    unsigned char ORAN_DVI[1];  // ORAN dataDirection, payloadVersion, and filterIndex
    // ORAN Frame Id (8bits), subframeId (4bits), slotId (6bits), and symbolId (6bits)
    unsigned char ORAN_PacketStructure[3];

    unsigned char ORAN_SecId[1];    // ORAN sectionId
    unsigned char ORAN_ignor[1];    // ORAN ignore for simplicity
    unsigned char ORAN_strPrbu[1];  // ORAN starting RB
    unsigned char ORAN_numPrbu[1];  // ORAN NRB per Packet

};

// Define structs for configuration data
struct EthConfig {
    int LineRate;
    int CaptureSizeMs;
    int MinNumOfIFGsPerPacket;
    string DestAddress;
    string SourceAddress;
    int MaxPacketSize;
};

struct OranConfig {
    int SCS;
    int MaxNrb;
    int NrbPerPacket;
    string PayloadType;
    string Payload;
};

struct PacketConfig {
    EthConfig eth;
    OranConfig oran;
};

// Class for ORAN Packet
class ORANPacket {
public:
    ORANPacket(int packet_size, int slots);

    int packet_size;
    int slots;
    int payload_size;
    
    // Static variable to count packets
    static int packet_counter;

    // ORAN Packet Structure Counters
    static int symbol;
    static int slot;
    static int subframe;
    static int frame;
    
    // Ethernet
    void setPreamble(const unsigned char preamble[7]);
    void setSFD(const unsigned char sfd[1]);
    void setDestinationMAC(const unsigned char dest_mac[6]);
    void setSourceMAC(const unsigned char src_mac[6]);
    void setLength();

    // eCPRI
    void set_eCPRI_VRC(const unsigned char eCPRI_VRC[1]);
    void set_eCPRI_Msg(const unsigned char eCPRI_Msg[1]);
    void set_eCPRI_Pay();
    void set_eCPRI_RPc(const unsigned char eCPRI_RPc[2]);
    void set_eCPRI_Seq();

    // ORAN
    void set_ORAN_DVI(unsigned char ORAN_DVI[1]);
    void set_ORAN_PacketStructure();

    // ORAN Section
    void set_ORAN_SecId(unsigned char ORAN_SecId[1]);
    void set_ORAN_ignor(unsigned char ORAN_ignor[1]);
    void set_ORAN_strPrbu(int strPrbu);
    void set_ORAN_numPrbu(int numPrbu);

    // ORAN Payload
    void setPayload(vector<signed char> payload_data, int strPrbu, int numPrbu);

    // Ethernet CRC
    void applyCRC();

    // Finilize
    void dumpToFile(const char* filename);

private:
    int padding_size;
    int total_size;
    std::vector<unsigned char> packet;
    Headers* headers;
    unsigned char* payload;
    unsigned char* crc;
};

// Additional Functions
void genIFG(int minIFG, const char* filename);

unsigned char* StrToArr(const std::string& hexString);

uint32_t crc32(uint32_t crc, const void *buf, size_t len);

void generatePacket(
    int packet_size,
    unsigned char* dest_mac,
    unsigned char* src_mac,
    int slots,
    int StartNRB,
    int NRBPerPacket,
    vector<signed char> iq_samples);

int findSlots(int SCS);
vector<signed char> Read_IQs(const string& filename);

string trim(const string& str);
bool parseConfig(const string& filePath, PacketConfig &config);

#endif
