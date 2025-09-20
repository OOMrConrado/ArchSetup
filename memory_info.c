#include "sysmon.h"

int read_memory_info(memory_info_t *memory) {
    FILE *fp;
    char line[MAX_LINE_LEN];
    char key[64];
    unsigned long value;

    memset(memory, 0, sizeof(memory_info_t));

    // Read memory information from /proc/meminfo
    fp = fopen("/proc/meminfo", "r");
    if (!fp) {
        perror("Error opening /proc/meminfo");
        return -1;
    }

    // Parse each line of /proc/meminfo
    while (fgets(line, sizeof(line), fp)) {
        if (sscanf(line, "%63s %lu", key, &value) == 2) {
            // Extract relevant memory statistics
            if (strcmp(key, "MemTotal:") == 0) {
                memory->total = value;
            } else if (strcmp(key, "MemFree:") == 0) {
                memory->free = value;
            } else if (strcmp(key, "MemAvailable:") == 0) {
                memory->available = value;
            } else if (strcmp(key, "Buffers:") == 0) {
                memory->buffers = value;
            } else if (strcmp(key, "Cached:") == 0) {
                memory->cached = value;
            } else if (strcmp(key, "SwapTotal:") == 0) {
                memory->swap_total = value;
            } else if (strcmp(key, "SwapFree:") == 0) {
                memory->swap_free = value;
            }
        }
    }
    fclose(fp);

    // Calculate derived memory statistics
    memory->used = memory->total - memory->free - memory->buffers - memory->cached;
    memory->swap_used = memory->swap_total - memory->swap_free;

    // Calculate usage percentages
    if (memory->total > 0) {
        memory->usage_percent = (double)(memory->total - memory->available) * 100.0 / memory->total;
    }

    if (memory->swap_total > 0) {
        memory->swap_percent = (double)memory->swap_used * 100.0 / memory->swap_total;
    }

    return 0;
}