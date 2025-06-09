#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "blockchain.h"
#include "cli.h"
#include "persistence.h"

#define MAX_INPUT 1024

int main(void) {
    // Try to load existing blockchain, create new one if not found
    Blockchain* chain = load_blockchain();
    if (!chain) {
        printf("No existing blockchain found. Creating new blockchain...\n");
        chain = create_blockchain();
        if (!chain) {
            fprintf(stderr, "Failed to initialize blockchain\n");
            return 1;
        }
    }

    printf("ALU Medical Blockchain System\n");
    printf("Type 'help' for available commands\n\n");

    char input[MAX_INPUT];
    int running = 1;

    while (running) {
        print_prompt();
        if (fgets(input, sizeof(input), stdin) == NULL) {
            break;
        }

        // Remove newline
        input[strcspn(input, "\n")] = 0;

        // Handle command
        running = handle_command(chain, input);
    }

    // Save blockchain before cleanup
    if (!save_blockchain(chain)) {
        fprintf(stderr, "Warning: Failed to save blockchain\n");
    }

    // Cleanup
    free_blockchain(chain);
    return 0;
} 