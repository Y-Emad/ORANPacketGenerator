# ORAN Packet Generator
## This is the Second Milestone of my Final Project in the 5G Diploma offered by SIEMENS EDA.

1) Use PacketConfig.ini file to modify packet parameters
2) If you modified any parameters, recompile the PacketGenerator.exe using:
```bash
g++ -o PacketGenerator main.cpp ORANPacket.cpp
```
3) Generate Packets: Run PacketGenerator.exe
4) Parse Packets: Run oran_packet_parse.bat
5) Read parsed packets from oran_packet_parsed.txt

Steps 2 to 4 can be automated using makefile
### 1. Navigate to the Project Directory
>Use **PowerShell** or the **Terminal** to navigate to your project directory:
```bash
cd <path_to_your_project>
```

### 2. Build the Project
>If you have modified any configuration files (like `PacketConfig`), recompile the project using:
```bash
make
```

### 3. Run the Programs
>To execute the compiled programs, use the following command:
```bash
make run
```

### 4. Clean Up
>To delete the generated executables and any `.bin` or `.txt` files, run:
```bash
make clean
```


### Additional Information
- Ensure you have the necessary tools installed (like `g++` and `make`).