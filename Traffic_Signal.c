#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <stdint.h>

// GPIO direction and value paths
#define GPIO_BASE_PATH "/sys/class/gpio"
#define GPIO_DIRECTION_FILE "direction"
#define GPIO_VALUE_FILE "value"

// Function to export a GPIO pin
int32_t export_gpio(int32_t pin) {
    int32_t fd, len;
    char buf[64];
    int32_t retries = 3;

    while (retries > 0) {
        fd = open(GPIO_BASE_PATH "/export", O_WRONLY);
        if (fd < 0) {
            (void)perror("Failed to open export for writing!");
            return -1;
        }

        len = snprintf(buf, sizeof(buf), "%d", pin);
        if (write(fd, buf, len) < 0) {
            (void)perror("Failed to write pin to export file!");
            (void)close(fd);
            retries--;
            sleep(1); // Wait for a second before retrying
        } else {
            (void)close(fd);
            return 0; // Success
        }
    }

    return -1; // Failed after retries
}

// Function to set the direction of a GPIO pin
int32_t set_gpio_direction(int32_t pin, char *direction) {
    int32_t fd;
    char buf[64];
    snprintf(buf, sizeof(buf), GPIO_BASE_PATH "/gpio%d/%s", pin, GPIO_DIRECTION_FILE);

    fd = open(buf, O_WRONLY);
    if (fd < 0) {
        (void)perror("Failed to open direction file for writing!");
        return -1;
    }

    write(fd, direction, strlen(direction));
    (void)close(fd);

    return 0;
}


// Function to initialize GPIO pins
int32_t init_gpio(int32_t red_pin_A, int32_t yellow_pin_A, int32_t green_pin_A, int32_t red_pin_B, int32_t yellow_pin_B, int32_t green_pin_B) {
    int32_t status = 0;

    // Export and set directions for GPIO pins
    status |= export_gpio(red_pin_A);
    status |= export_gpio(yellow_pin_A);
    status |= export_gpio(green_pin_A);
    status |= export_gpio(red_pin_B);
    status |= export_gpio(yellow_pin_B);
    status |= export_gpio(green_pin_B);

    status |= set_gpio_direction(red_pin_A, "out");
    status |= set_gpio_direction(yellow_pin_A, "out");
    status |= set_gpio_direction(green_pin_A, "out");
    status |= set_gpio_direction(red_pin_B, "out");
    status |= set_gpio_direction(yellow_pin_B, "out");
    status |= set_gpio_direction(green_pin_B, "out");

    // Return status indicating success or failure
    return status;
}

int32_t main() {
    int32_t red_pin_A, yellow_pin_A, green_pin_A, red_pin_B, yellow_pin_B, green_pin_B;
    int32_t sleep_interval;

    // Prompt user for GPIO pins and sleep interval
    (void)printf("Enter GPIO pins for Traffic Light Set A (Red Yellow Green): ");
    scanf("%d %d %d", &red_pin_A, &yellow_pin_A, &green_pin_A);
    (void)printf("Enter GPIO pins for Traffic Light Set B (Red Yellow Green): ");
    scanf("%d %d %d", &red_pin_B, &yellow_pin_B, &green_pin_B);
    (void)printf("Enter sleep interval (in seconds): ");
    scanf("%d", &sleep_interval);

    // Initialize GPIO pins
    if (init_gpio(red_pin_A, yellow_pin_A, green_pin_A, red_pin_B, yellow_pin_B, green_pin_B) != 0) {
        (void)printf("Failed to initialize GPIO pins. Exiting.\n");
        return 1;
    }
    return 0;
}
