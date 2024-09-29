#include "ORANPacket.h"

// Initialize all the static counters
int ORANPacket::packet_counter = 0;
int ORANPacket::frame = 0;
int ORANPacket::subframe = 0;
int ORANPacket::slot = 0;
int ORANPacket::symbol = 0;

int main() {
    // Assign Default Configurations
    int lineRate = 10;                                      // Line Rate in Gbps
    int captureSize = 10;                                   // Capture size in MilliSeconds (10 ms)
    int minIFG = 12;                                        // Number of IFGs at packet end
    unsigned char* dest_mac = StrToArr("0x010101010101");   // Destination Address
    unsigned char*  src_mac = StrToArr("0x333333333333");   // Source Address
    int packet_size = 1500;                                 // Max Packet Size in Bytes

    int SCS = 30;                                           // SubCarrier Spacing
    int MaxNrb = 273;                                       // Total NRBs to be sent
    int NrbPerPacket = 30;                                  // Number of 12 IQ Samples per packet

    string PayloadType = "Fixed";                           // Fixed means read from file, Random can be old data in memory
    string Payload = "iq_file.txt";                         // File to read from

    // Load data from config file if it exists
    PacketConfig config;
    if (parseConfig("PacketConfig.ini", config)) {
        lineRate = config.eth.LineRate;
        captureSize = config.eth.CaptureSizeMs;
        minIFG = config.eth.MinNumOfIFGsPerPacket;
        dest_mac = StrToArr(config.eth.DestAddress);
        src_mac =  StrToArr(config.eth.SourceAddress);
        packet_size = config.eth.MaxPacketSize;
        SCS = config.oran.SCS;
        MaxNrb = config.oran.MaxNrb;
        NrbPerPacket = config.oran.NrbPerPacket;
        PayloadType = config.oran.PayloadType;
        Payload = config.oran.Payload;
    } else cout << "Configuration file not found or could not be loaded." << endl;


    vector<signed char> iq_samples;
    if (PayloadType == "fixed") iq_samples = Read_IQs(Payload); // Read IQ samples from .txt
    else iq_samples = Read_IQs("random_numbers.txt");           // Random IQ samples

    // Additional Properties for Packets
    int slots = findSlots(SCS);
    int StartNRB = 0;
    int totalPackets = ceil(MaxNrb/(float)NrbPerPacket);
    int lastPacketNRBs = MaxNrb % NrbPerPacket;

    // Time Simulation
    long long byteTime = 1e3/lineRate;                     // Byte Time in PicoSeconds
    long long pktTime = (packet_size + minIFG)*byteTime;   // Packet Time in PicoSeconds
    long long time = 0;                                    // Program time in PicoSeconds
    long long captureTime = captureSize*1e9;               // Total capture windows in PicoSeconds
    while (time < captureTime) {
        if (ORANPacket::packet_counter < totalPackets) {
            cout <<  setw(10) << setfill('0') << time << " ps : ";
            time += pktTime;
            if (time < captureTime) { // Generate within Capture window
                if (ORANPacket::packet_counter == (totalPackets-1))
                    NrbPerPacket = lastPacketNRBs;
                generatePacket(
                    packet_size, dest_mac, src_mac,
                    slots, StartNRB, NrbPerPacket, iq_samples
                );
                StartNRB += NrbPerPacket;
                genIFG(minIFG, "oran_packet.bin");
            } else // Not enough time to send the Packet
                genIFG(packet_size + minIFG, "oran_packet.bin");
        } else { // send IFGs when not sending packets
            long long remainder = captureTime - time;
            genIFG(remainder/byteTime, "oran_packet.bin");
            time += remainder;
        }
    }
    return 0;
}
