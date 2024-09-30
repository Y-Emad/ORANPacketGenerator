# ORAN Packet Generator
## This is the Second Milestone of my Final Project in the 5G Diploma offered by SIEMENS EDA.

1) Use PacketConfig.ini file to modify packet parameters
2) Generate Packets: Run PacketGenerator.exe
3) Parse Packets: Run oran_packet_parse.bat
4) Read parsed packets from oran_packet_parsed.txt

## If you changed the code, you can automate the generation process using makefile.
Be sure to have [g++](https://gcc.gnu.org/) and [Make](https://gnuwin32.sourceforge.net/packages/make.htm) installed.

### 1. Navigate to the Project Directory
Use **PowerShell** or the **Terminal** to navigate to your project directory:
```bash
cd <path_to_your_project>
```

### 2. Build the Project
If you have modified any configuration files (like `PacketConfig`), recompile the project using:
```bash
make
```

### 3. Run the Programs
To execute the compiled programs, use the following command:
```bash
make run
```

### 4. Clean Up
To delete the generated executables and any `.bin` or `.txt` files, run:
```bash
make clean
```


## Compiling without Makefile
```bash
g++ -o PacketGenerator main.cpp ORANPacket.cpp
g++ -o PacketParser PacketsParser.cpp
```
