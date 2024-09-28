#include "ORANPacket.h"

// Initialize all the static counters
int ORANPacket::packet_counter = 0;
int ORANPacket::frame = 0;
int ORANPacket::subframe = 0;
int ORANPacket::slot = 0;
int ORANPacket::symbol = 0;

int main() {
    int packet_size = 1500; // in Bytes
    unsigned char* dest_mac = StrToArr("0x010101010101");
    unsigned char*  src_mac = StrToArr("0x333333333333");
    int minIFG = 12; // Number of IFGs at packet end

    int lineRate = 10; // in Gbps
    long long int captureSize = 1*1e9; // Capture size in PicoSeconds (10 ms)

    int SCS = 30;           // SubCarrier Spacing
    int MaxNrb = 273;       // Total NRBs to be sent
    int NrbPerPacket = 30;  // Number of 12 IQ Samples per packet

    string PayloadType = "Fixed";   // Fixed means read from file, Random can be old data in memory
    string Payload = "iq_file.txt"; // File to read from
    vector<signed char> iq_samples; // Random data from memory
    if (PayloadType == "Fixed") iq_samples = Read_IQs(Payload); // Read IQ samples from .txt

    // Packets Properties
    int slots = findSlots(SCS);
    int StartNRB = 0;
    int totalPackets = ceil(MaxNrb/(float)NrbPerPacket);
    int lastPacketNRBs = MaxNrb % NrbPerPacket;

    // Time Simulation
    long long int byteTime = 1e3/lineRate; // Byte Time in PicoSeconds
    long long int pktTime = (packet_size + minIFG)*byteTime; // Packet Time in PicoSeconds
    long long int time = 0; // time in PicoSeconds
    while (time < captureSize) {
        if (ORANPacket::packet_counter < totalPackets) {
            cout <<  setw(10) << setfill('0') << time << " : ";
            time += pktTime;
            if (time < captureSize) {   // Generate within Capture window
                if (ORANPacket::packet_counter == (totalPackets-1))
                    NrbPerPacket = lastPacketNRBs;
                generatePacket(
                    packet_size, dest_mac, src_mac,
                    slots, StartNRB, NrbPerPacket, iq_samples
                );
                StartNRB += NrbPerPacket;
                genIFG(minIFG, "oran_packet.bin");
            } else  // Not enough time to send the Packet
                genIFG(packet_size + minIFG, "oran_packet.bin");
        } else {    // send IFGs when not sending packets
            int remainder = captureSize - time;
            genIFG(remainder/byteTime, "oran_packet.bin");
            time += remainder;
        }
    }
    return 0;
}
