#include "ORANPacket.h"

// Constructor
ORANPacket::ORANPacket(int packet_size, int slots)
    : packet_size(packet_size) // Initialize payload_size with the value passed to the constructor
    , slots(slots) // Initialize slots with the value passed to the constructor
    {  
    // Allocate buffer with total size
    packet.resize(packet_size, 0);

    // Assign memory location pointers according to Header Struct
    headers = reinterpret_cast<Headers*>(packet.data()); 

    // Finding Maximum ORAN Payload size
    payload_size = packet_size - sizeof(Headers) - CRC_LEN;

    // Assign memory location pointers for payload and CRC
    payload = packet.data() + sizeof(Headers);  // Pointer to Payload Start Byte
    crc = payload + payload_size;               // Pointer to CRC Start Byte

    // Assign Constant Ethernet Headers
    unsigned char preamble[7] = { 0xFB, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55 };
    this->setPreamble(preamble);

    unsigned char sfd[1] = { 0xD5 };
    this->setSFD(sfd);

    this->setLength();

    // Assign Constant eCPRI Headers
    unsigned char eCPRI_VRC[1] = { 0x00 };
    this->set_eCPRI_VRC(eCPRI_VRC);

    unsigned char eCPRI_Msg[1] = { 0x00 };
    this->set_eCPRI_Msg(eCPRI_Msg);
    
    this->set_eCPRI_Pay();

    unsigned char eCPRI_RPc[2] = { 0x00, 0x00 };
    this->set_eCPRI_RPc(eCPRI_RPc);

    this->set_eCPRI_Seq();

    // Assign ORAN Headers
    unsigned char ORAN_DVI[1] = { 0x00 };
    this->set_ORAN_DVI(ORAN_DVI);

    this->set_ORAN_PacketStructure();

    // Assign ORAN section
    unsigned char ORAN_SecId[1] = { 0x95 };
    this->set_ORAN_SecId(ORAN_SecId);

    unsigned char ORAN_ignor[1] = { 0x00 };
    this->set_ORAN_ignor(ORAN_ignor);

}

// Setters for Ethernet Fields
void ORANPacket::setPreamble(const unsigned char preamble[7]) {
    memcpy(headers->preamble, preamble, 7);
}
void ORANPacket::setSFD(const unsigned char sfd[1]) {
    memcpy(headers->sfd, sfd, 1);
}
void ORANPacket::setDestinationMAC(const unsigned char dest_mac[6]) {
    memcpy(headers->dest_mac, dest_mac, 6);
}
void ORANPacket::setSourceMAC(const unsigned char src_mac[6]) {
    memcpy(headers->src_mac, src_mac, 6);
}
void ORANPacket::setLength() {
    headers->length[0] = (packet_size >> 8) & 0xFF;
    headers->length[1] = packet_size & 0xFF;
}

// Setters for eCPRI Fields
void ORANPacket::set_eCPRI_VRC(const unsigned char eCPRI_VRC[1]) {
    memcpy(headers->eCPRI_VRC, eCPRI_VRC, 1);
}
void ORANPacket::set_eCPRI_Msg(const unsigned char eCPRI_Msg[1]) {
    memcpy(headers->eCPRI_Msg, eCPRI_Msg, 1);
}
void ORANPacket::set_eCPRI_Pay() {
    int eCPRI_Pay = packet_size - ETH_HDRLEN - eCPRI_HDRLEN - CRC_LEN;
    headers->eCPRI_Pay[0] = (eCPRI_Pay >> 8) & 0xFF;
    headers->eCPRI_Pay[1] = eCPRI_Pay & 0xFF;
}
void ORANPacket::set_eCPRI_RPc(const unsigned char eCPRI_RPc[2]) {
    memcpy(headers->eCPRI_RPc, eCPRI_RPc, 2);
}
void ORANPacket::set_eCPRI_Seq() {
    headers->eCPRI_Seq[0] = (packet_counter >> 8) & 0xFF;
    headers->eCPRI_Seq[1] = packet_counter & 0xFF;
    packet_counter++;
}

// Setters for ORAN Fields
void ORANPacket::set_ORAN_DVI(unsigned char ORAN_DVI[1]) {
    memcpy(headers->ORAN_DVI, ORAN_DVI, 1);    
}
void ORANPacket::set_ORAN_PacketStructure() {
    // apply Frame, subframe, slot, symbol
    headers->ORAN_PacketStructure[0] = frame;
    headers->ORAN_PacketStructure[1] = (subframe & 0x0F) << 4;
    headers->ORAN_PacketStructure[1] |= (slot >> 2) & 0x0F;
    headers->ORAN_PacketStructure[2] = (slot & 0x03) << 6;
    headers->ORAN_PacketStructure[2] |= (symbol & 0x3F);
    
    symbol++;
    if (symbol == 14) {
        symbol = 0;
        slot++;
        if (slot == slots) {
            slot = 0;
            subframe++;
            if (subframe == 10) {
                subframe = 0;
                frame++;
            }
        }
    }
}

// Setters for ORAN Section Fields
void ORANPacket::set_ORAN_SecId(unsigned char ORAN_SecId[1]) {
    memcpy(headers->ORAN_SecId, ORAN_SecId, 1);    
}
void ORANPacket::set_ORAN_ignor(unsigned char ORAN_ignor[1]) {
    memcpy(headers->ORAN_ignor, ORAN_ignor, 1);    
}
void ORANPacket::set_ORAN_strPrbu(int strPrbu) {
    headers->ORAN_strPrbu[0] = strPrbu;
}
void ORANPacket::set_ORAN_numPrbu(int numPrbu) {
    headers->ORAN_numPrbu[0] = numPrbu;
}


// Setter for ORAN Payload
void ORANPacket::setPayload(vector<signed char> payload_data, int strPrbu, int numPrbu) {
    memcpy(payload, payload_data.data() + strPrbu, numPrbu*12*2);

    // Call the CRC function after payload is filled
    this->applyCRC();
}

// Ethernet CRC Tail
void ORANPacket::applyCRC() {
    // Calculate the CRC using the provided payload
    uint32_t CRC = 0xFFFFFFFF; // Initial CRC value
    CRC = crc32(CRC, payload, payload_size);

    // Convert the CRC value to a byte array
    unsigned char crc_bytes[4];
    crc_bytes[0] = (CRC >> 24) & 0xFF;
    crc_bytes[1] = (CRC >> 16) & 0xFF;
    crc_bytes[2] = (CRC >> 8) & 0xFF;
    crc_bytes[3] = CRC & 0xFF;

    // Copy the CRC bytes to the CRC field in the packet
    memcpy(crc, crc_bytes, CRC_LEN);
}

// Dump packet to a file
void ORANPacket::dumpToFile(const char* filename) {
    ofstream outfile(filename, ios::app | ios::binary);
    for (int i = 0; i < packet_size; i++) {
        outfile << hex << setw(2) << setfill('0') << (int)packet[i];
        if ((i + 1) % 4 == 0) 
            outfile << "\n";    // New Line every 4 Bytes
        else
            outfile << " ";     // Space after every Byte
    }
    outfile.close();
    cout << "Packet Generated in " << filename << endl;
}

// IFG Generator
void genIFG(int minIFG, const char* filename) {
    int remainder = minIFG % 4;
    if (remainder) minIFG = minIFG + 4 - remainder;
    ofstream outfile(filename, ios::app | ios::binary);
    for (int i = 0; i < minIFG; i++) {
        outfile << hex << setw(2) << setfill('0') << IFG;
        if ((i + 1) % 4 == 0) 
            outfile << "\n";    // New Line every 4 Bytes
        else
            outfile << " ";     // Space after every Byte
    }
    outfile.close();
}

// Additional Functions
unsigned char* StrToArr(const string& hexString) {
    unsigned char* byteArray = new unsigned char[(hexString.length() - 2) / 2];

    // Iterate over the hexString in pairs of characters
    for (size_t i = 0, j = 0; i < hexString.length() - 2; i += 2, j++) {

        // Convert the hex byte to an unsigned char
        byteArray[j] = stoi(hexString.substr(i + 2, 2), nullptr, 16);
    }

    return byteArray;
}

uint32_t crc32(uint32_t crc, const void *buf, size_t len) {
    const uint8_t *p = (const uint8_t *)buf;
    while (len--) {
        crc ^= *p++;
        for (int i = 0; i < 8; i++) {
            crc = (crc >> 1) ^ (crc & 1 ? 0xEDB88320 : 0);
        }
    }
    return crc;
}

void generatePacket(
    int packet_size,
    unsigned char* dest_mac,
    unsigned char* src_mac,
    int slots,
    int StartNRB,
    int NRBPerPacket,
    vector<signed char> iq_samples
    ) {
    ORANPacket packet(packet_size, slots);

    packet.setDestinationMAC(dest_mac);

    packet.setSourceMAC(src_mac);

    packet.set_ORAN_strPrbu(StartNRB);

    packet.set_ORAN_numPrbu(NRBPerPacket);
    
    packet.setPayload(iq_samples, StartNRB, NRBPerPacket);

    packet.dumpToFile("oran_packet.bin");
}

int findSlots(int SCS) {
    switch(SCS) {
        case 15:
            return 1;
        case 30:
            return 2;
        case 60:
            return 4;
        case 120:
            return 8;
        default:
            return 16;
    }    
}

// Function to read IQ samples from a file and return them as a single array
vector<signed char> Read_IQs(const string& filename) {
    ifstream file(filename);
    if (!file) {
        cerr << "Unable to open file: " << filename << endl;
        return {};
    }

    vector<signed char> iq_samples;
    string line;

    while (getline(file, line)) {
        istringstream iss(line);
        int i_sample, q_sample;

        // Read two integers from each line (I and Q samples)
        if (iss >> i_sample >> q_sample) {
            iq_samples.push_back(static_cast<signed char>(i_sample)); // I sample
            iq_samples.push_back(static_cast<signed char>(q_sample)); // Q sample
        }
    }

    // Close the file
    file.close();
    return iq_samples;
}