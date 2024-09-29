@echo off
echo Currently parsing packets, do not close window.
PacketParser.exe > oran_packet_parsed.txt
notepad oran_packet_parsed.txt