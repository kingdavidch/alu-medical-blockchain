#ifndef BLOCK_H
#define BLOCK_H

#include <time.h>
#include <stdint.h>

#define MAX_TRANSACTIONS 10
#define HASH_SIZE 64  // SHA-256 produces 64 hex characters

// Transaction structure for medical records
typedef struct {
    char patient_id[32];
    char record_type[32];  // e.g., "diagnosis", "prescription", "visit"
    char data[256];        // The actual medical record data
    time_t timestamp;
} Transaction;

// Block structure
typedef struct Block {
    uint32_t id;                    // Block identifier
    time_t timestamp;               // Block creation time
    Transaction transactions[MAX_TRANSACTIONS];  // Array of transactions
    int transaction_count;          // Number of transactions in this block
    char previous_hash[HASH_SIZE + 1];  // Hash of the previous block
    char hash[HASH_SIZE + 1];       // Hash of this block
    uint32_t nonce;                 // Proof of work nonce
    struct Block* next;             // Pointer to the next block
} Block;

// Function declarations
Block* create_block(uint32_t id, const char* previous_hash);
void calculate_block_hash(Block* block);
int add_transaction(Block* block, const Transaction* transaction);
void free_block(Block* block);
int verify_block(const Block* block);
void print_block(const Block* block);

#endif // BLOCK_H 