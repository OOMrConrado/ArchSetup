#include "sysmon.h"
#include <signal.h>
#include <getopt.h>

static volatile int running = 1;

void signal_handler(int sig) {
    if (sig == SIGINT || sig == SIGTERM) {
        running = 0;
    }
}

void print_usage(const char *prog_name) {
    printf("Usage: %s [options]\n", prog_name);
    printf("\nOptions:\n");
    printf("  -w, --watch           Continuous monitor mode (updates every 2s)\n");
    printf("  -c, --cpu             Show only CPU information\n");
    printf("  -m, --memory          Show only memory information\n");
    printf("  -u, --uptime          Show only system uptime\n");
    printf("  -d, --disk            Show only disk information\n");
    printf("  -p, --processes       Show only top processes\n");
    printf("  -a, --all             Show all information (default)\n");
    printf("  -h, --help            Show this help\n");
    printf("\nExamples:\n");
    printf("  %s                    Show all information once\n", prog_name);
    printf("  %s --watch            Continuous monitor mode\n", prog_name);
    printf("  %s --cpu --memory     Show only CPU and memory\n", prog_name);
}

int main(int argc, char *argv[]) {
    int watch_mode = 0;     // Flag for continuous monitoring mode
    int show_flags = 0;     // Bit flags for what information to display

    // Set up signal handlers for graceful exit
    signal(SIGINT, signal_handler);
    signal(SIGTERM, signal_handler);

    // Define command line options
    static struct option long_options[] = {
        {"watch",     no_argument, 0, 'w'},
        {"cpu",       no_argument, 0, 'c'},
        {"memory",    no_argument, 0, 'm'},
        {"uptime",    no_argument, 0, 'u'},
        {"disk",      no_argument, 0, 'd'},
        {"processes", no_argument, 0, 'p'},
        {"all",       no_argument, 0, 'a'},
        {"help",      no_argument, 0, 'h'},
        {0, 0, 0, 0}
    };

    // Parse command line arguments
    int opt;
    while ((opt = getopt_long(argc, argv, "wcmudpah", long_options, NULL)) != -1) {
        switch (opt) {
            case 'w':
                watch_mode = 1;
                break;
            case 'c':
                show_flags |= SHOW_CPU;
                break;
            case 'm':
                show_flags |= SHOW_MEMORY;
                break;
            case 'u':
                show_flags |= SHOW_UPTIME;
                break;
            case 'd':
                show_flags |= SHOW_DISK;
                break;
            case 'p':
                show_flags |= SHOW_PROC;
                break;
            case 'a':
                show_flags = SHOW_ALL;
                break;
            case 'h':
                print_usage(argv[0]);
                return 0;
            default:
                print_usage(argv[0]);
                return 1;
        }
    }

    // If no specific flags set, show everything
    if (show_flags == 0) {
        show_flags = SHOW_ALL;
    }

    system_info_t info;

    // Main monitoring loop
    do {
        if (watch_mode) {
            clear_screen();
        }

        display_header();

        // Initialize system info structure
        memset(&info, 0, sizeof(system_info_t));

        // Read and display CPU information if requested
        if (show_flags & SHOW_CPU) {
            if (read_cpu_info(&info.cpu) == 0) {
                display_cpu_info(&info.cpu);
            }
        }

        // Read and display memory information if requested
        if (show_flags & SHOW_MEMORY) {
            if (read_memory_info(&info.memory) == 0) {
                display_memory_info(&info.memory);
            }
        }

        // Read and display uptime information if requested
        if (show_flags & SHOW_UPTIME) {
            if (read_uptime_info(&info.uptime) == 0) {
                display_uptime_info(&info.uptime);
            }
        }

        // Read and display disk information if requested
        if (show_flags & SHOW_DISK) {
            if (read_disk_info(&info.disk) == 0) {
                display_disk_info(&info.disk);
            }
        }

        // Read and display process information if requested
        if (show_flags & SHOW_PROC) {
            info.process_count = read_top_processes(info.top_processes, 10);
            if (info.process_count > 0) {
                display_processes(info.top_processes, info.process_count);
            }
        }

        // In watch mode, wait before next update
        if (watch_mode && running) {
            fflush(stdout);
            sleep(2);
        }

    } while (watch_mode && running);

    return 0;
}