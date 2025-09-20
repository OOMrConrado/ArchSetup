#ifndef SYSMON_H
#define SYSMON_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <dirent.h>
#include <ctype.h>
#include <sys/statvfs.h>
#include <sys/sysinfo.h>

// Maximum number of CPU cores to track
#define MAX_CPU_CORES 32
// Maximum line length for file reading
#define MAX_LINE_LEN 512
// Maximum process name length
#define MAX_PROC_NAME 32

// ANSI color codes for terminal output
#define COLOR_RESET   "\033[0m"
#define COLOR_RED     "\033[31m"
#define COLOR_GREEN   "\033[32m"
#define COLOR_YELLOW  "\033[33m"
#define COLOR_BLUE    "\033[34m"
#define COLOR_MAGENTA "\033[35m"
#define COLOR_CYAN    "\033[36m"
#define COLOR_WHITE   "\033[37m"
#define COLOR_BOLD    "\033[1m"

// CPU information structure
typedef struct {
    char model[128];                    // CPU model name
    int cores;                          // Number of CPU cores
    double usage[MAX_CPU_CORES];        // Per-core usage percentages
    double total_usage;                 // Overall CPU usage percentage
    double temperature;                 // CPU temperature in Celsius
} cpu_info_t;

// Memory information structure
typedef struct {
    unsigned long total;                // Total RAM in KB
    unsigned long available;            // Available RAM in KB
    unsigned long used;                 // Used RAM in KB
    unsigned long free;                 // Free RAM in KB
    unsigned long buffers;              // Buffer memory in KB
    unsigned long cached;               // Cached memory in KB
    unsigned long swap_total;           // Total swap in KB
    unsigned long swap_used;            // Used swap in KB
    unsigned long swap_free;            // Free swap in KB
    double usage_percent;               // RAM usage percentage
    double swap_percent;                // Swap usage percentage
} memory_info_t;

// System uptime information structure
typedef struct {
    unsigned long uptime_seconds;       // System uptime in seconds
    char uptime_formatted[64];          // Human-readable uptime string
    unsigned long session_time;         // Current session time
} uptime_info_t;

// Disk usage information structure
typedef struct {
    char filesystem[64];                // Filesystem name
    unsigned long total_bytes;          // Total disk space in bytes
    unsigned long used_bytes;           // Used disk space in bytes
    unsigned long available_bytes;      // Available disk space in bytes
    double usage_percent;               // Disk usage percentage
} disk_info_t;

// Process information structure
typedef struct {
    char name[MAX_PROC_NAME];           // Process name
    int pid;                            // Process ID
    double cpu_percent;                 // CPU usage percentage
    unsigned long memory_kb;            // Memory usage in KB
} process_info_t;

// Main system information structure containing all metrics
typedef struct {
    cpu_info_t cpu;                     // CPU information
    memory_info_t memory;               // Memory information
    uptime_info_t uptime;               // Uptime information
    disk_info_t disk;                   // Disk information
    process_info_t top_processes[10];   // Top 10 processes
    int process_count;                  // Number of processes found
} system_info_t;

// Function prototypes for data collection
int read_cpu_info(cpu_info_t *cpu);
int read_memory_info(memory_info_t *memory);
int read_uptime_info(uptime_info_t *uptime);
int read_disk_info(disk_info_t *disk);
int read_top_processes(process_info_t *processes, int max_count);

// Function prototypes for display
void display_system_info(const system_info_t *info, int show_flags);
void display_header(void);
void display_cpu_info(const cpu_info_t *cpu);
void display_memory_info(const memory_info_t *memory);
void display_uptime_info(const uptime_info_t *uptime);
void display_disk_info(const disk_info_t *disk);
void display_processes(const process_info_t *processes, int count);

// Utility function prototypes
const char* get_color_by_percentage(double percent);
void format_bytes(unsigned long bytes, char *output);
void clear_screen(void);

// Display flags for modular output (bit flags)
#define SHOW_CPU     (1 << 0)    // Show CPU information
#define SHOW_MEMORY  (1 << 1)    // Show memory information
#define SHOW_UPTIME  (1 << 2)    // Show uptime information
#define SHOW_DISK    (1 << 3)    // Show disk information
#define SHOW_PROC    (1 << 4)    // Show process information
#define SHOW_ALL     (SHOW_CPU | SHOW_MEMORY | SHOW_UPTIME | SHOW_DISK | SHOW_PROC)

#endif