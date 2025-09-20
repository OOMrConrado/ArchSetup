#include "sysmon.h"

// Static variables to track previous CPU stats for usage calculation
static unsigned long prev_total[MAX_CPU_CORES] = {0};
static unsigned long prev_idle[MAX_CPU_CORES] = {0};
static int first_run = 1;

int read_cpu_info(cpu_info_t *cpu) {
    FILE *fp;
    char line[MAX_LINE_LEN];
    int core_count = 0;

    memset(cpu, 0, sizeof(cpu_info_t));

    // Read CPU model and core count from /proc/cpuinfo
    fp = fopen("/proc/cpuinfo", "r");
    if (!fp) {
        perror("Error opening /proc/cpuinfo");
        return -1;
    }

    while (fgets(line, sizeof(line), fp)) {
        // Extract CPU model name (only first occurrence)
        if (strncmp(line, "model name", 10) == 0 && strlen(cpu->model) == 0) {
            char *colon = strchr(line, ':');
            if (colon) {
                colon += 2;  // Skip ": "
                char *newline = strchr(colon, '\n');
                if (newline) *newline = '\0';
                strncpy(cpu->model, colon, sizeof(cpu->model) - 1);
                cpu->model[sizeof(cpu->model) - 1] = '\0';
            }
        }
        // Count processor cores
        if (strncmp(line, "processor", 9) == 0) {
            core_count++;
        }
    }
    fclose(fp);

    cpu->cores = core_count;

    // Read CPU usage statistics from /proc/stat
    fp = fopen("/proc/stat", "r");
    if (!fp) {
        perror("Error opening /proc/stat");
        return -1;
    }

    int core_idx = 0;

    // Parse CPU usage statistics for each core
    while (fgets(line, sizeof(line), fp) && core_idx <= core_count) {
        if (strncmp(line, "cpu", 3) == 0) {
            unsigned long user, nice, system, idle, iowait, irq, softirq, steal;

            // Overall CPU stats (line starts with "cpu ")
            if (line[3] == ' ') {
                sscanf(line, "cpu %lu %lu %lu %lu %lu %lu %lu %lu",
                       &user, &nice, &system, &idle, &iowait, &irq, &softirq, &steal);

                unsigned long total = user + nice + system + idle + iowait + irq + softirq + steal;

                // Calculate usage percentage (skip first run for accurate diff)
                if (!first_run) {
                    unsigned long total_diff = total - prev_total[0];
                    unsigned long idle_diff = idle - prev_idle[0];

                    if (total_diff > 0) {
                        cpu->total_usage = 100.0 * (total_diff - idle_diff) / total_diff;
                    }
                }

                // Store current values for next calculation
                prev_total[0] = total;
                prev_idle[0] = idle;

            // Individual CPU core stats (line starts with "cpu0", "cpu1", etc.)
            } else if (line[3] >= '0' && line[3] <= '9') {
                int cpu_num;
                sscanf(line, "cpu%d %lu %lu %lu %lu %lu %lu %lu %lu",
                       &cpu_num, &user, &nice, &system, &idle, &iowait, &irq, &softirq, &steal);

                if (cpu_num < MAX_CPU_CORES) {
                    unsigned long total = user + nice + system + idle + iowait + irq + softirq + steal;

                    // Calculate per-core usage percentage
                    if (!first_run) {
                        unsigned long total_diff = total - prev_total[cpu_num + 1];
                        unsigned long idle_diff = idle - prev_idle[cpu_num + 1];

                        if (total_diff > 0) {
                            cpu->usage[cpu_num] = 100.0 * (total_diff - idle_diff) / total_diff;
                        }
                    }

                    // Store current values for next calculation
                    prev_total[cpu_num + 1] = total;
                    prev_idle[cpu_num + 1] = idle;
                }
            }
        }
        core_idx++;
    }
    fclose(fp);

    // Try to read CPU temperature from thermal sensors
    fp = fopen("/sys/class/thermal/thermal_zone0/temp", "r");
    if (fp) {
        int temp_millidegrees;
        if (fscanf(fp, "%d", &temp_millidegrees) == 1) {
            cpu->temperature = temp_millidegrees / 1000.0;  // Convert to Celsius
        }
        fclose(fp);
    } else {
        // Try alternative thermal sensor path
        fp = fopen("/sys/devices/platform/coretemp.0/hwmon/hwmon0/temp1_input", "r");
        if (fp) {
            int temp_millidegrees;
            if (fscanf(fp, "%d", &temp_millidegrees) == 1) {
                cpu->temperature = temp_millidegrees / 1000.0;  // Convert to Celsius
            }
            fclose(fp);
        }
    }

    // On first run, initialize values and return (no usage calculation yet)
    if (first_run) {
        first_run = 0;
        cpu->total_usage = 0.0;
        for (int i = 0; i < cpu->cores; i++) {
            cpu->usage[i] = 0.0;
        }
        return 0;
    }

    return 0;
}