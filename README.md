# ArchSetup System Monitor

A custom system monitor written in C for Arch Linux that displays detailed system information with a colorful terminal interface.

##  Features

- **CPU Information**: Model, cores, per-core usage and temperature
- **RAM and SWAP Memory**: Total usage, available space and percentages with progress bars
- **System Uptime**: Formatted readable uptime information
- **Disk Information**: Root filesystem usage statistics
- **Top Processes**: Processes consuming the most CPU and memory
- **Colorful Interface**: ANSI color codes with dynamic colors based on usage
- **Watch Mode**: Continuous updates every 2 seconds
- **Modular Options**: Show only the information you need

##  Usage

### Installation

```bash
# Clone and compile
make

# Local installation (no sudo required)
./install_local.sh
source ~/.bashrc
```

### Available Commands

```bash
# Show all information
ArchSetup

# Continuous monitor mode (updates every 2s)
ArchSetup --watch

# Show only specific information
ArchSetup --cpu       # CPU only
ArchSetup --memory    # Memory only
ArchSetup --uptime    # Uptime only
ArchSetup --disk      # Disk only
ArchSetup --processes # Top processes only

# Combinations
ArchSetup --cpu --memory    # CPU and memory
ArchSetup --all             # Everything (default)

# Help
ArchSetup --help
```

##  Manual Compilation

```bash
# Compile
make

# Clean object files
make clean

# Compile with debug information
make debug

# Test directly
make run
make watch
```

##  Requirements

- **System**: Arch Linux (may work on other distributions)
- **Compiler**: GCC with C99 support
- **Dependencies**: Only standard C libraries
- **Permissions**: Read access to `/proc/` and `/sys/`

##  Project Structure

```
ArchSetup/
├── main.c              # Main program and argument parsing
├── sysmon.h           # Header with definitions and structures
├── sysmon.c           # Display functions and interface
├── cpu_info.c         # CPU information reading from /proc/
├── memory_info.c      # Memory reading from /proc/meminfo
├── system_info.c      # Uptime, disk and processes
├── Makefile           # Compilation and tasks
├── install_local.sh   # Local installation script
└── README.md          # This documentation
```

##  Color Codes

- **Green**: Normal usage (< 60%)
- **Yellow**: Moderate usage (60-80%)
- **Red**: High usage (> 80%)
- **Blue**: CPU information
- **Magenta**: Memory information
- **Green**: System uptime
- **Yellow**: Disk information
- **Red**: Top processes

##  Information Sources

- `/proc/cpuinfo` - Processor information
- `/proc/stat` - CPU usage statistics
- `/proc/meminfo` - Memory information
- `/proc/uptime` - System uptime
- `/sys/class/thermal/` - CPU temperature
- `/proc/[pid]/` - Process information
- `statvfs()` - Filesystem information

##  Customization

The code is designed to be easily customizable:

- **Colors**: Modify defines in `sysmon.h`
- **Intervals**: Change update time in `main.c`
- **Format**: Adjust display functions in `sysmon.c`
- **Additional Metrics**: Add new data sources

##  Troubleshooting

**Error: Command not found**
```bash
# Make sure ~/bin is in your PATH
echo $PATH | grep -q "$HOME/bin" || source ~/.bashrc
```

**Permission errors**
```bash
# Check read permissions on /proc/
ls -la /proc/cpuinfo /proc/meminfo
```

**Temperature not available**
```bash
# Check available thermal sensors
find /sys/class/thermal/ -name "temp*_input" 2>/dev/null
```

##  License

Open source project. Free to use, modify and distribute.

##  Contributions

Contributions are welcome! You can:

- Report bugs
- Suggest new features
- Improve documentation
- Optimize code
- Add support for more distributions

##  Support

For issues or questions about the ArchSetup system monitor, you can:

1. Review this README
2. Check system logs
3. Test with specific options to isolate the problem

---

**Enjoy monitoring your Arch Linux system with style!**
