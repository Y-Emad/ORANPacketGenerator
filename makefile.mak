# Use Windows Command Shell
SHELL := cmd.exe

# Define the compiler
CXX = g++
# Define the flags for compilation
CXXFLAGS = -Wall
# Define the linker flags
LDFLAGS = -static

# Define the output binary names
GENERATOR = PacketGenerator
PARSER = PacketParser

# Define the source files
GENERATOR_SRC = main.cpp ORANPacket.cpp
PARSER_SRC = PacketsParser.cpp

# Define the target names
all: $(GENERATOR) $(PARSER)

# Rule for PacketGenerator
$(GENERATOR): $(GENERATOR_SRC)
	$(CXX) $(CXXFLAGS) $(LDFLAGS) -o $(GENERATOR) $(GENERATOR_SRC)

# Rule for PacketParser
$(PARSER): $(PARSER_SRC)
	$(CXX) $(CXXFLAGS) $(LDFLAGS) -o $(PARSER) $(PARSER_SRC)

# Rule to run the PacketGenerator and PacketParser
run:
	if exist oran_packet.bin del oran_packet.bin
	if exist oran_packet_parsed.txt del oran_packet_parsed.txt
	$(GENERATOR).exe
	cmd /c oran_packet_parse.bat

# Clean up the generated files (Windows-specific)
clean:
	if exist $(GENERATOR).exe del $(GENERATOR).exe
	if exist $(PARSER).exe del $(PARSER).exe
	if exist oran_packet.bin del oran_packet.bin
	if exist oran_packet_parsed.txt del oran_packet_parsed.txt
