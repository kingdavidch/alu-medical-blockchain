#ifndef BLOCKCHAIN_H
#define BLOCKCHAIN_H

#include "block.h"

#define DIFFICULTY 4  // Number of leading zeros required in hash

typedef struct {
    Block* genesis;           // Pointer to the first block
    Block* latest;           // Pointer to the most recent block
    uint32_t block_count;    // Total number of blocks
    int difficulty;          // Current mining difficulty
} Blockchain;

// Function declarations
Blockchain* create_blockchain(void);
void free_blockchain(Blockchain* chain);
int add_block(Blockchain* chain, Block* block);
int mine_block(Blockchain* chain, Block* block);
int verify_chain(const Blockchain* chain);
void print_blockchain(const Blockchain* chain);
Block* get_block_by_id(const Blockchain* chain, uint32_t id);
int get_transaction_count(const Blockchain* chain);

#endif // BLOCKCHAIN_H 