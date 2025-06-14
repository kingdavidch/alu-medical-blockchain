#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "block.h"
#include "blockchain.h"
#include "utils.h"

Blockchain* create_blockchain(void) {
    Blockchain* chain = (Blockchain*)malloc(sizeof(Blockchain));
    if (!chain) {
        return NULL;
    }

    // Create genesis block
    chain->genesis = create_block(0, NULL);
    if (!chain->genesis) {
        free(chain);
        return NULL;
    }

    chain->latest = chain->genesis;
    chain->block_count = 1;
    chain->difficulty = DIFFICULTY;

    // Mine genesis block
    mine_block(chain, chain->genesis);
    return chain;
}

void free_blockchain(Blockchain* chain) {
    if (!chain) {
        return;
    }

    Block* current = chain->genesis;
    while (current) {
        Block* next = current->next;
        free_block(current);
        current = next;
    }

    free(chain);
}

int add_block(Blockchain* chain, Block* block) {
    if (!chain || !block) {
        return 0;
    }

    // Verify block's previous hash matches the latest block
    if (strcmp(block->previous_hash, chain->latest->hash) != 0) {
        return 0;
    }

    // Add block to chain
    chain->latest->next = block;
    chain->latest = block;
    chain->block_count++;

    return 1;
}

int mine_block(Blockchain* chain, Block* block) {
    if (!chain || !block) {
        return 0;
    }

    // Set previous hash
    strncpy(block->previous_hash, chain->latest->hash, HASH_SIZE);
    block->previous_hash[HASH_SIZE] = '\0';

    // Mine block
    do {
        block->nonce++;
        calculate_block_hash(block);
    } while (!is_valid_hash(block->hash, chain->difficulty));

    return 1;
}

int verify_chain(const Blockchain* chain) {
    if (!chain || !chain->genesis) {
        return 0;
    }

    Block* current = chain->genesis;
    while (current) {
        // Verify block hash
        if (!verify_block(current)) {
            return 0;
        }

        // Verify previous hash (except for genesis block)
        if (current != chain->genesis) {
            if (strcmp(current->previous_hash, current->next->hash) != 0) {
                return 0;
            }
        }

        current = current->next;
    }

    return 1;
}

void print_blockchain(const Blockchain* chain) {
    if (!chain) {
        return;
    }

    printf("\nBlockchain Status:\n");
    printf("Total Blocks: %u\n", chain->block_count);
    printf("Current Difficulty: %d\n", chain->difficulty);
    printf("Chain Valid: %s\n\n", verify_chain(chain) ? "Yes" : "No");

    printf("\nBlocks:\n");

    Block* current = chain->genesis;
    while (current) {
        // For now, we'll print blocks without decryption
        printf("\nBlock #%u\n", current->id);
        printf("Timestamp: %s", get_timestamp_str(current->timestamp));
        printf("Previous Hash: %s\n", current->previous_hash);
        printf("Hash: %s\n", current->hash);
        printf("Nonce: %u\n", current->nonce);
        printf("Transactions: %d\n", current->transaction_count);
        current = current->next;
    }
}

Block* get_block_by_id(const Blockchain* chain, uint32_t id) {
    if (!chain) {
        return NULL;
    }

    Block* current = chain->genesis;
    while (current) {
        if (current->id == id) {
            return current;
        }
        current = current->next;
    }

    return NULL;
}

int get_transaction_count(const Blockchain* chain) {
    if (!chain) {
        return 0;
    }

    int count = 0;
    Block* current = chain->genesis;
    while (current) {
        count += current->transaction_count;
        current = current->next;
    }

    return count;
} 