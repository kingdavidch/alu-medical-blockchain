#ifndef CLI_H
#define CLI_H

#include "blockchain.h"

// Command handler function type
typedef int (*CommandHandler)(Blockchain* chain, int argc, char** argv);

// Command structure
typedef struct {
    const char* name;
    const char* description;
    CommandHandler handler;
} Command;

// Available commands
extern Command commands[];

// Function declarations
void print_help(void);
int handle_command(Blockchain* chain, const char* input);
void print_prompt(void);
void print_error(const char* message);
void print_success(const char* message);

// Command handlers
int cmd_add(Blockchain* chain, int argc, char** argv);
int cmd_mine(Blockchain* chain, int argc, char** argv);
int cmd_view(Blockchain* chain, int argc, char** argv);
int cmd_verify(Blockchain* chain, int argc, char** argv);
int cmd_backup(Blockchain* chain, int argc, char** argv);
int cmd_restore(Blockchain* chain, int argc, char** argv);
int cmd_help(Blockchain* chain, int argc, char** argv);
int cmd_exit(Blockchain* chain, int argc, char** argv);

#endif // CLI_H 