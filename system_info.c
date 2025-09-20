#include "sysmon.h"

int read_uptime_info(uptime_info_t *uptime) {
    FILE *fp;
    double uptime_seconds;

    memset(uptime, 0, sizeof(uptime_info_t));

    // Read system uptime from /proc/uptime
    fp = fopen("/proc/uptime", "r");
    if (!fp) {
        perror("Error opening /proc/uptime");
        return -1;
    }

    if (fscanf(fp, "%lf", &uptime_seconds) != 1) {
        fclose(fp);
        return -1;
    }
    fclose(fp);

    uptime->uptime_seconds = (unsigned long)uptime_seconds;

    // Format uptime into human-readable string
    unsigned long days = uptime->uptime_seconds / 86400;
    unsigned long hours = (uptime->uptime_seconds % 86400) / 3600;
    unsigned long minutes = (uptime->uptime_seconds % 3600) / 60;
    unsigned long seconds = uptime->uptime_seconds % 60;

    if (days > 0) {
        snprintf(uptime->uptime_formatted, sizeof(uptime->uptime_formatted),
                "%lu days, %lu hours, %lu minutes", days, hours, minutes);
    } else if (hours > 0) {
        snprintf(uptime->uptime_formatted, sizeof(uptime->uptime_formatted),
                "%lu hours, %lu minutes", hours, minutes);
    } else {
        snprintf(uptime->uptime_formatted, sizeof(uptime->uptime_formatted),
                "%lu minutes, %lu seconds", minutes, seconds);
    }

    return 0;
}

int read_disk_info(disk_info_t *disk) {
    struct statvfs stat_buf;

    memset(disk, 0, sizeof(disk_info_t));

    // Get filesystem statistics for root directory
    if (statvfs("/", &stat_buf) != 0) {
        perror("Error getting disk information");
        return -1;
    }

    strcpy(disk->filesystem, "/");

    // Calculate disk space statistics
    disk->total_bytes = stat_buf.f_blocks * stat_buf.f_frsize;
    disk->available_bytes = stat_buf.f_bavail * stat_buf.f_frsize;
    disk->used_bytes = disk->total_bytes - (stat_buf.f_bfree * stat_buf.f_frsize);

    // Calculate usage percentage
    if (disk->total_bytes > 0) {
        disk->usage_percent = (double)disk->used_bytes * 100.0 / disk->total_bytes;
    }

    return 0;
}

// Comparison function for sorting processes by CPU usage
int compare_processes_by_cpu(const void *a, const void *b) {
    const process_info_t *proc_a = (const process_info_t *)a;
    const process_info_t *proc_b = (const process_info_t *)b;

    if (proc_b->cpu_percent > proc_a->cpu_percent) return 1;
    if (proc_b->cpu_percent < proc_a->cpu_percent) return -1;
    return 0;
}

int read_top_processes(process_info_t *processes, int max_count) {
    DIR *proc_dir;
    struct dirent *entry;
    FILE *fp;
    char path[512];
    char line[MAX_LINE_LEN];
    int count = 0;

    memset(processes, 0, max_count * sizeof(process_info_t));

    // Open /proc directory to read process information
    proc_dir = opendir("/proc");
    if (!proc_dir) {
        perror("Error opening /proc");
        return 0;
    }

    // Iterate through /proc directory entries
    while ((entry = readdir(proc_dir)) && count < max_count) {
        // Skip non-numeric entries (only process IDs are numeric)
        if (!isdigit(entry->d_name[0])) continue;

        int pid = atoi(entry->d_name);
        if (pid <= 0) continue;

        // Read process name from /proc/[pid]/comm
        snprintf(path, sizeof(path), "/proc/%d/comm", pid);
        fp = fopen(path, "r");
        if (!fp) continue;

        memset(line, 0, sizeof(line));
        if (fgets(line, sizeof(line), fp)) {
            char *newline = strchr(line, '\n');
            if (newline) *newline = '\0';
            size_t name_len = strlen(line);
            if (name_len >= MAX_PROC_NAME) name_len = MAX_PROC_NAME - 1;
            memcpy(processes[count].name, line, name_len);
            processes[count].name[name_len] = '\0';
        } else {
            strcpy(processes[count].name, "unknown");
        }
        fclose(fp);

        processes[count].pid = pid;

        // Read process statistics from /proc/[pid]/stat
        snprintf(path, sizeof(path), "/proc/%d/stat", pid);
        fp = fopen(path, "r");
        if (!fp) continue;

        unsigned long utime = 0, stime = 0, vsize = 0, rss = 0;
        if (fscanf(fp, "%*d %*s %*c %*d %*d %*d %*d %*d %*u %*u %*u %*u %*u %lu %lu %*d %*d %*d %*d %*d %*d %*u %lu %ld",
                   &utime, &stime, &vsize, &rss) >= 2) {

            // Calculate CPU usage and memory consumption
            processes[count].cpu_percent = (double)(utime + stime) / 10000.0;
            processes[count].memory_kb = rss * 4;  // RSS is in pages, convert to KB
        }
        fclose(fp);

        count++;
    }
    closedir(proc_dir);

    // Sort processes by CPU usage (highest first)
    qsort(processes, count, sizeof(process_info_t), compare_processes_by_cpu);

    return count;
}