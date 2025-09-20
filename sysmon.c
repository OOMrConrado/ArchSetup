#include "sysmon.h"

// Returns appropriate color based on usage percentage
const char* get_color_by_percentage(double percent) {
    if (percent >= 80.0) return COLOR_RED;      // High usage - red
    if (percent >= 60.0) return COLOR_YELLOW;   // Medium usage - yellow
    return COLOR_GREEN;                         // Low usage - green
}

// Formats byte values into human-readable units (B, KB, MB, GB, TB)
void format_bytes(unsigned long bytes, char *output) {
    const char *units[] = {"B", "KB", "MB", "GB", "TB"};
    int unit = 0;
    double size = (double)bytes;

    // Convert to appropriate unit
    while (size >= 1024.0 && unit < 4) {
        size /= 1024.0;
        unit++;
    }

    if (unit == 0) {
        snprintf(output, 32, "%.0f %s", size, units[unit]);
    } else {
        snprintf(output, 32, "%.1f %s", size, units[unit]);
    }
}

// Clears the terminal screen and moves cursor to top-left
void clear_screen(void) {
    printf("\033[2J\033[H");
}

// Displays the main header with title and timestamp
void display_header(void) {
    time_t now;
    struct tm *timeinfo;
    char timestamp[64];

    // Get current time and format it
    time(&now);
    timeinfo = localtime(&now);
    strftime(timestamp, sizeof(timestamp), "%Y-%m-%d %H:%M:%S", timeinfo);

    // Display header with fancy Unicode box characters
    printf("%s╔══════════════════════════════════════════════════════════════════════════════╗%s\n",
           COLOR_CYAN, COLOR_RESET);
    printf("%s║%s                           %sArchSetup System Monitor%s                           %s║%s\n",
           COLOR_CYAN, COLOR_RESET, COLOR_BOLD COLOR_WHITE, COLOR_RESET, COLOR_CYAN, COLOR_RESET);
    printf("%s║%s                              %s%s%s                              %s║%s\n",
           COLOR_CYAN, COLOR_RESET, COLOR_BLUE, timestamp, COLOR_RESET, COLOR_CYAN, COLOR_RESET);
    printf("%s╚══════════════════════════════════════════════════════════════════════════════╝%s\n\n",
           COLOR_CYAN, COLOR_RESET);
}

// Displays CPU information with usage bars and temperature
void display_cpu_info(const cpu_info_t *cpu) {
    printf("%s┌─ CPU Information ─────────────────────────────────────────────────────────────┐%s\n",
           COLOR_BLUE, COLOR_RESET);

    // Truncate CPU model if too long
    char truncated_model[61];
    if (strlen(cpu->model) > 60) {
        strncpy(truncated_model, cpu->model, 57);
        truncated_model[57] = '.';
        truncated_model[58] = '.';
        truncated_model[59] = '.';
        truncated_model[60] = '\0';
    } else {
        strcpy(truncated_model, cpu->model);
    }

    printf("%s│%s Model: %s%-60s%s %s│%s\n",
           COLOR_BLUE, COLOR_RESET, COLOR_WHITE, truncated_model, COLOR_RESET, COLOR_BLUE, COLOR_RESET);
    printf("%s│%s Cores: %s%-60d%s %s│%s\n",
           COLOR_BLUE, COLOR_RESET, COLOR_WHITE, cpu->cores, COLOR_RESET, COLOR_BLUE, COLOR_RESET);

    // Display total CPU usage with progress bar
    printf("%s│%s Total usage: %s%6.1f%%%s ",
           COLOR_BLUE, COLOR_RESET, get_color_by_percentage(cpu->total_usage),
           cpu->total_usage, COLOR_RESET);

    // Draw usage progress bar
    int bar_length = 35;
    int filled = (int)(cpu->total_usage * bar_length / 100.0);
    printf("[");
    for (int i = 0; i < bar_length; i++) {
        if (i < filled) {
            printf("█");  // Filled portion
        } else {
            printf("░");  // Empty portion
        }
    }
    printf("] %s│%s\n", COLOR_BLUE, COLOR_RESET);

    // Display temperature if available
    if (cpu->temperature > 0) {
        printf("%s│%s Temperature: %s%6.1f°C%s %53s %s│%s\n",
               COLOR_BLUE, COLOR_RESET,
               get_color_by_percentage(cpu->temperature > 70 ? 80 : cpu->temperature),
               cpu->temperature, COLOR_RESET, "", COLOR_BLUE, COLOR_RESET);
    }

    printf("%s└───────────────────────────────────────────────────────────────────────────────┘%s\n\n",
           COLOR_BLUE, COLOR_RESET);
}

void display_memory_info(const memory_info_t *memory) {
    char total_str[32], used_str[32], free_str[32], available_str[32];
    char swap_total_str[32], swap_used_str[32];

    format_bytes(memory->total * 1024, total_str);
    format_bytes(memory->used * 1024, used_str);
    format_bytes(memory->free * 1024, free_str);
    format_bytes(memory->available * 1024, available_str);
    format_bytes(memory->swap_total * 1024, swap_total_str);
    format_bytes(memory->swap_used * 1024, swap_used_str);

    printf("%s┌─ Memory Information ──────────────────────────────────────────────────────────┐%s\n",
           COLOR_MAGENTA, COLOR_RESET);

    printf("%s│%s RAM Total: %s%-10s%s Used: %s%-10s%s Available: %s%-10s%s %17s %s│%s\n",
           COLOR_MAGENTA, COLOR_RESET, COLOR_WHITE, total_str, COLOR_RESET,
           get_color_by_percentage(memory->usage_percent), used_str, COLOR_RESET,
           COLOR_WHITE, available_str, COLOR_RESET, "", COLOR_MAGENTA, COLOR_RESET);

    printf("%s│%s RAM usage: %s%6.1f%%%s ",
           COLOR_MAGENTA, COLOR_RESET, get_color_by_percentage(memory->usage_percent),
           memory->usage_percent, COLOR_RESET);

    int bar_length = 35;
    int filled = (int)(memory->usage_percent * bar_length / 100.0);
    printf("[");
    for (int i = 0; i < bar_length; i++) {
        if (i < filled) {
            printf("█");
        } else {
            printf("░");
        }
    }
    printf("] %s│%s\n", COLOR_MAGENTA, COLOR_RESET);

    if (memory->swap_total > 0) {
        printf("%s│%s SWAP Total: %s%-10s%s Used: %s%-10s%s Usage: %s%6.1f%%%s %16s %s│%s\n",
               COLOR_MAGENTA, COLOR_RESET, COLOR_WHITE, swap_total_str, COLOR_RESET,
               get_color_by_percentage(memory->swap_percent), swap_used_str, COLOR_RESET,
               get_color_by_percentage(memory->swap_percent), memory->swap_percent, COLOR_RESET,
               "", COLOR_MAGENTA, COLOR_RESET);
    }

    printf("%s└───────────────────────────────────────────────────────────────────────────────┘%s\n\n",
           COLOR_MAGENTA, COLOR_RESET);
}

void display_uptime_info(const uptime_info_t *uptime) {
    printf("%s┌─ System Uptime ───────────────────────────────────────────────────────────────┐%s\n",
           COLOR_GREEN, COLOR_RESET);
    printf("%s│%s System uptime: %s%-60s%s %s│%s\n",
           COLOR_GREEN, COLOR_RESET, COLOR_WHITE, uptime->uptime_formatted, COLOR_RESET,
           COLOR_GREEN, COLOR_RESET);
    printf("%s└───────────────────────────────────────────────────────────────────────────────┘%s\n\n",
           COLOR_GREEN, COLOR_RESET);
}

void display_disk_info(const disk_info_t *disk) {
    char total_str[32], used_str[32], available_str[32];

    format_bytes(disk->total_bytes, total_str);
    format_bytes(disk->used_bytes, used_str);
    format_bytes(disk->available_bytes, available_str);

    printf("%s┌─ Disk Information ────────────────────────────────────────────────────────────┐%s\n",
           COLOR_YELLOW, COLOR_RESET);
    printf("%s│%s Filesystem: %s%-8s%s Total: %s%-10s%s Used: %s%-10s%s %19s %s│%s\n",
           COLOR_YELLOW, COLOR_RESET, COLOR_WHITE, disk->filesystem, COLOR_RESET,
           COLOR_WHITE, total_str, COLOR_RESET,
           get_color_by_percentage(disk->usage_percent), used_str, COLOR_RESET,
           "", COLOR_YELLOW, COLOR_RESET);

    printf("%s│%s Disk usage: %s%6.1f%%%s ",
           COLOR_YELLOW, COLOR_RESET, get_color_by_percentage(disk->usage_percent),
           disk->usage_percent, COLOR_RESET);

    int bar_length = 35;
    int filled = (int)(disk->usage_percent * bar_length / 100.0);
    printf("[");
    for (int i = 0; i < bar_length; i++) {
        if (i < filled) {
            printf("█");
        } else {
            printf("░");
        }
    }
    printf("] %s│%s\n", COLOR_YELLOW, COLOR_RESET);

    printf("%s└───────────────────────────────────────────────────────────────────────────────┘%s\n\n",
           COLOR_YELLOW, COLOR_RESET);
}

void display_processes(const process_info_t *processes, int count) {
    printf("%s┌─ Top Processes ───────────────────────────────────────────────────────────────┐%s\n",
           COLOR_RED, COLOR_RESET);
    printf("%s│%s %5s %-16s %8s %12s %34s %s│%s\n",
           COLOR_RED, COLOR_RESET, "PID", "NAME", "CPU%", "MEMORY", "", COLOR_RED, COLOR_RESET);
    printf("%s│%s────────────────────────────────────────────────────────────────────────────── %s│%s\n",
           COLOR_RED, COLOR_RESET, COLOR_RED, COLOR_RESET);

    for (int i = 0; i < count && i < 10; i++) {
        char mem_str[32];
        char truncated_name[17]; // 16 chars + null terminator

        format_bytes(processes[i].memory_kb * 1024, mem_str);

        // Truncate process name if too long and add ellipsis
        if (strlen(processes[i].name) > 16) {
            strncpy(truncated_name, processes[i].name, 13);
            truncated_name[13] = '.';
            truncated_name[14] = '.';
            truncated_name[15] = '.';
            truncated_name[16] = '\0';
        } else {
            strcpy(truncated_name, processes[i].name);
        }

        printf("%s│%s %5d %-16s %s%7.1f%%%s %12s %34s %s│%s\n",
               COLOR_RED, COLOR_RESET, processes[i].pid, truncated_name,
               get_color_by_percentage(processes[i].cpu_percent), processes[i].cpu_percent, COLOR_RESET,
               mem_str, "", COLOR_RED, COLOR_RESET);
    }

    printf("%s└───────────────────────────────────────────────────────────────────────────────┘%s\n\n",
           COLOR_RED, COLOR_RESET);
}