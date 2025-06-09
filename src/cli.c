#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "cli.h"
#include "security.h"

// Static key for demonstration (in a real system, load securely)
static unsigned char CLI_KEY[AES_KEY_SIZE] = {0};

// Command definitions
Command commands[] = {
    {"add", "Add a new medical record", cmd_add},
    {"mine", "Mine a new block", cmd_mine},
    {"view", "View the entire blockchain", cmd_view},
    {"verify", "Verify chain integrity", cmd_verify},
    {"help", "Show this help message", cmd_help},
    {"exit", "Exit the program", cmd_exit},
    {NULL, NULL, NULL}  // Terminator
};

void print_help(void) {
    printf("\nAvailable commands:\n");
    for (Command* cmd = commands; cmd->name != NULL; cmd++) {
        printf("  %-10s - %s\n", cmd->name, cmd->description);
    }
    printf("\n");
}

int handle_command(Blockchain* chain, const char* input) {
    char cmd[32];
    char* args[10];
    int argc = 0;

    // Parse command and arguments
    char* input_copy = strdup(input);
    char* token = strtok(input_copy, " \t\n");
    
    if (!token) {
        free(input_copy);
        return 1;
    }

    strncpy(cmd, token, sizeof(cmd) - 1);
    cmd[sizeof(cmd) - 1] = '\0';

    // Convert command to lowercase
    for (char* p = cmd; *p; p++) {
        *p = tolower(*p);
    }

    // Parse arguments
    while ((token = strtok(NULL, " \t\n")) && argc < 9) {
        args[argc++] = token;
    }
    args[argc] = NULL;

    // Find and execute command
    for (Command* command = commands; command->name != NULL; command++) {
        if (strcmp(cmd, command->name) == 0) {
            int result = command->handler(chain, argc, args);
            free(input_copy);
            return result;
        }
    }

    printf("Unknown command: %s\n", cmd);
    print_help();
    free(input_copy);
    return 1;
}

void print_prompt(void) {
    printf("medblockchain> ");
    fflush(stdout);
}

void print_error(const char* message) {
    printf("Error: %s\n", message);
}

void print_success(const char* message) {
    printf("Success: %s\n", message);
}

// Command handlers
int cmd_add(Blockchain* chain, int argc, char** argv) {
    if (argc < 3) {
        print_error("Usage: add <patient_id> <record_type> <data>");
        return 1;
    }

    Transaction transaction;
    memset(&transaction, 0, sizeof(Transaction));
    strncpy(transaction.patient_id, argv[0], sizeof(transaction.patient_id) - 1);
    strncpy(transaction.record_type, argv[1], sizeof(transaction.record_type) - 1);
    transaction.timestamp = time(NULL);
    transaction.encrypted_data = encrypt_data(argv[2], CLI_KEY);
    if (!transaction.encrypted_data) {
        print_error("Failed to encrypt data");
        return 1;
    }

    if (add_transaction(chain->latest, &transaction, CLI_KEY)) {
        print_success("Transaction added successfully");
    } else {
        print_error("Failed to add transaction");
        free_encrypted_data(transaction.encrypted_data);
    }

    return 1;
}

int cmd_mine(Blockchain* chain, int argc, char** argv) {
    if (chain->latest->transaction_count == 0) {
        print_error("No transactions to mine");
        return 1;
    }

    Block* new_block = create_block(chain->block_count, chain->latest->hash);
    if (!new_block) {
        print_error("Failed to create new block");
        return 1;
    }

    if (mine_block(chain, new_block)) {
        if (add_block(chain, new_block)) {
            print_success("New block mined successfully");
        } else {
            print_error("Failed to add new block to chain");
            free_block(new_block);
        }
    } else {
        print_error("Failed to mine new block");
        free_block(new_block);
    }

    return 1;
}

int cmd_view(Blockchain* chain, int argc, char** argv) {
    print_blockchain(chain);
    return 1;
}

int cmd_verify(Blockchain* chain, int argc, char** argv) {
    if (verify_chain(chain)) {
        print_success("Blockchain is valid");
    } else {
        print_error("Blockchain is invalid");
    }
    return 1;
}

int cmd_help(Blockchain* chain, int argc, char** argv) {
    print_help();
    return 1;
}

int cmd_exit(Blockchain* chain, int argc, char** argv) {
    return 0;
} 