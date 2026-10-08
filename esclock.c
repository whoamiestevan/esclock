#define _POSIX_C_SOURCE 200809L

#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

#define KEY_LENGTH 200
#define INPUT_SIZE 256
#define FILE_PERMISSION 0600

static const char CHARSET[] =
    "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
    "abcdefghijklmnopqrstuvwxyz"
    "0123456789"
    "!@#$%^&*()_+[]{}";

#define CHARSET_SIZE (sizeof(CHARSET) - 1)

/*
 * Prevents modulo bias.
 *
 * 256 possible byte values are divided into equal groups
 * matching the size of the character set.
 */
#define UNBIASED_LIMIT ((256 / CHARSET_SIZE) * CHARSET_SIZE)

enum {
    MENU_PRINT = 1,
    MENU_SAVE = 2
};

/*
 * Generate a cryptographically strong random key.
 *
 * Returns:
 *   0  = success
 *  -1  = error
 */
static int generate_key(char *buffer, size_t length)
{
    FILE *rng = fopen("/dev/urandom", "rb");

    if (rng == NULL) {
        perror("Cannot open /dev/urandom");
        return -1;
    }

    size_t index = 0;

    while (index < length) {

        unsigned char byte;

        if (fread(&byte, 1, 1, rng) != 1) {
            fprintf(stderr, "Failed to read random data.\n");
            fclose(rng);
            return -1;
        }

        /*
         * Reject values that would cause modulo bias.
         */
        if (byte < UNBIASED_LIMIT) {
            buffer[index] = CHARSET[byte % CHARSET_SIZE];
            index++;
        }
    }

    buffer[length] = '\0';

    fclose(rng);

    return 0;
}

/*
 * Read one line from standard input.
 */
static int read_line(const char *prompt, char *buffer, size_t size)
{
    printf("%s", prompt);
    fflush(stdout);

    if (fgets(buffer, (int)size, stdin) == NULL) {
        return -1;
    }

    buffer[strcspn(buffer, "\n")] = '\0';

    return 0;
}

/*
 * Read and validate the menu choice.
 *
 * Returns:
 *   1 or 2 = valid choice
 *   0       = invalid input
 *  -1       = input closed/error
 */
static int read_choice(void)
{
    char input[INPUT_SIZE];

    if (read_line("> ", input, sizeof(input)) != 0) {
        return -1;
    }

    char *end;

    long value = strtol(input, &end, 10);

    if (end == input || *end != '\0') {
        return 0;
    }

    if (value != MENU_PRINT && value != MENU_SAVE) {
        return 0;
    }

    return (int)value;
}

/*
 * Display the menu.
 */
static void print_menu(void)
{
    printf("\n");
    printf("=================================\n");
    printf("            ESCLOCK\n");
    printf("=================================\n");
    printf("How would you like to receive the key?\n");
    printf("1. Print to terminal\n");
    printf("2. Save to a file\n");
}

/*
 * Print the generated key.
 */
static int print_key(const char *key)
{
    printf("\nEncryption Key:\n");
    printf("%s\n", key);

    return 0;
}

/*
 * Save the generated key to a new file.
 *
 * O_EXCL prevents an existing file from being overwritten.
 */
static int save_key(const char *key)
{
    char filename[INPUT_SIZE];

    if (read_line("Enter filename: ", filename, sizeof(filename)) != 0) {
        fprintf(stderr, "Failed to read filename.\n");
        return -1;
    }

    if (filename[0] == '\0') {
        fprintf(stderr, "Invalid filename.\n");
        return -1;
    }

    int fd = open(
        filename,
        O_WRONLY | O_CREAT | O_EXCL,
        FILE_PERMISSION
    );

    if (fd < 0) {
        fprintf(
            stderr,
            "Cannot create '%s': %s\n",
            filename,
            strerror(errno)
        );

        return -1;
    }

    FILE *file = fdopen(fd, "w");

    if (file == NULL) {
        perror("fdopen");
        close(fd);
        return -1;
    }

    if (fprintf(file, "%s\n", key) < 0) {
        fprintf(stderr, "Failed to write '%s'.\n", filename);
        fclose(file);
        return -1;
    }

    if (fclose(file) != 0) {
        fprintf(stderr, "Failed to close '%s'.\n", filename);
        return -1;
    }

    printf("Key saved to %s\n", filename);

    return 0;
}

int main(void)
{
    char key[KEY_LENGTH + 1];
    int choice;

    /*
     * Display the menu until the user selects
     * a valid option.
     */
    for (;;) {

        print_menu();

        choice = read_choice();

        if (choice == -1) {
            fprintf(stderr, "Input closed.\n");
            return EXIT_FAILURE;
        }

        if (choice == MENU_PRINT || choice == MENU_SAVE) {
            break;
        }

        fprintf(
            stderr,
            "Invalid option. Please enter 1 or 2.\n"
        );
    }

    /*
     * Generate the key.
     */
    if (generate_key(key, KEY_LENGTH) != 0) {
        return EXIT_FAILURE;
    }

    /*
     * Handle the selected option.
     */
    int status;

    if (choice == MENU_PRINT) {
        status = print_key(key);
    } else {
        status = save_key(key);
    }

    /*
     * Clear the key from the buffer after use.
     */
    memset(key, 0, sizeof(key));

    if (status != 0) {
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
