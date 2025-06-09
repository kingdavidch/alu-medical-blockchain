#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <dirent.h>
#include "persistence.h"
#include "block.h"
#include "blockchain.h"

// Save blockchain metadata
static int save_metadata(const Blockchain* chain) {
    FILE* file = fopen(BLOCKCHAIN_META_FILE, "wb");
    if (!file) return 0;

    // Write metadata
    fwrite(&chain->block_count, sizeof(uint32_t), 1, file);
    fwrite(&chain->difficulty, sizeof(int), 1, file);
    fclose(file);
    return 1;
}

// Load blockchain metadata
static int load_metadata(Blockchain* chain) {
    FILE* file = fopen(BLOCKCHAIN_META_FILE, "rb");
    if (!file) return 0;

    // Read metadata
    fread(&chain->block_count, sizeof(uint32_t), 1, file);
    fread(&chain->difficulty, sizeof(int), 1, file);
    fclose(file);
    return 1;
}

// Save the entire blockchain to disk
int save_blockchain(const Blockchain* chain) {
    if (!chain) return 0;

    // Save metadata first
    if (!save_metadata(chain)) return 0;

    FILE* file = fopen(BLOCKCHAIN_FILE, "wb");
    if (!file) return 0;

    // Write each block
    Block* current = chain->genesis;
    while (current) {
        // Write block ID
        fwrite(&current->id, sizeof(uint32_t), 1, file);
        
        // Write timestamp
        fwrite(&current->timestamp, sizeof(time_t), 1, file);
        
        // Write previous hash
        fwrite(current->previous_hash, sizeof(char), HASH_SIZE + 1, file);
        
        // Write current hash
        fwrite(current->hash, sizeof(char), HASH_SIZE + 1, file);
        
        // Write nonce
        fwrite(&current->nonce, sizeof(uint32_t), 1, file);
        
        // Write transaction count
        fwrite(&current->transaction_count, sizeof(int), 1, file);
        
        // Write transactions
        for (int i = 0; i < current->transaction_count; i++) {
            fwrite(&current->transactions[i], sizeof(Transaction), 1, file);
        }
        
        current = current->next;
    }

    fclose(file);
    return 1;
}

// Load the blockchain from disk
Blockchain* load_blockchain(void) {
    Blockchain* chain = create_blockchain();
    if (!chain) return NULL;

    // Load metadata
    if (!load_metadata(chain)) {
        free_blockchain(chain);
        return NULL;
    }

    FILE* file = fopen(BLOCKCHAIN_FILE, "rb");
    if (!file) {
        free_blockchain(chain);
        return NULL;
    }

    Block* previous = NULL;
    for (uint32_t i = 0; i < chain->block_count; i++) {
        Block* block = create_block(i, previous ? previous->hash : "");
        if (!block) {
            fclose(file);
            free_blockchain(chain);
            return NULL;
        }

        // Read block ID
        fread(&block->id, sizeof(uint32_t), 1, file);
        
        // Read timestamp
        fread(&block->timestamp, sizeof(time_t), 1, file);
        
        // Read previous hash
        fread(block->previous_hash, sizeof(char), HASH_SIZE + 1, file);
        
        // Read current hash
        fread(block->hash, sizeof(char), HASH_SIZE + 1, file);
        
        // Read nonce
        fread(&block->nonce, sizeof(uint32_t), 1, file);
        
        // Read transaction count
        fread(&block->transaction_count, sizeof(int), 1, file);
        
        // Read transactions
        for (int j = 0; j < block->transaction_count; j++) {
            fread(&block->transactions[j], sizeof(Transaction), 1, file);
        }

        // Link blocks
        if (previous) {
            previous->next = block;
        } else {
            chain->genesis = block;
        }
        previous = block;
    }

    chain->latest = previous;
    fclose(file);
    return chain;
}

// Create a backup of the blockchain
int backup_blockchain(const Blockchain* chain) {
    if (!chain) return 0;
    
    // Create backup filenames with timestamp
    char backup_file[256];
    char backup_meta_file[256];
    time_t now = time(NULL);
    snprintf(backup_file, sizeof(backup_file), "blockchain_backup_%ld.dat", now);
    snprintf(backup_meta_file, sizeof(backup_meta_file), "blockchain_meta_backup_%ld.dat", now);

    // Save to backup files
    FILE* file = fopen(backup_file, "wb");
    FILE* meta_file = fopen(backup_meta_file, "wb");
    if (!file || !meta_file) {
        if (file) fclose(file);
        if (meta_file) fclose(meta_file);
        return 0;
    }

    // Write metadata
    fwrite(&chain->block_count, sizeof(uint32_t), 1, meta_file);
    fwrite(&chain->difficulty, sizeof(int), 1, meta_file);
    fclose(meta_file);

    // Write blockchain data
    Block* current = chain->genesis;
    while (current) {
        fwrite(&current->id, sizeof(uint32_t), 1, file);
        fwrite(&current->timestamp, sizeof(time_t), 1, file);
        fwrite(current->previous_hash, sizeof(char), HASH_SIZE + 1, file);
        fwrite(current->hash, sizeof(char), HASH_SIZE + 1, file);
        fwrite(&current->nonce, sizeof(uint32_t), 1, file);
        fwrite(&current->transaction_count, sizeof(int), 1, file);
        
        for (int i = 0; i < current->transaction_count; i++) {
            fwrite(&current->transactions[i], sizeof(Transaction), 1, file);
        }
        
        current = current->next;
    }

    fclose(file);
    return 1;
}

// Restore blockchain from backup
int restore_blockchain(Blockchain* chain) {
    if (!chain) return 0;

    // Find latest backup
    char backup_file[256];
    char backup_meta_file[256];
    time_t latest_backup = 0;
    DIR* dir = opendir(".");
    if (!dir) return 0;

    struct dirent* entry;
    while ((entry = readdir(dir)) != NULL) {
        if (strncmp(entry->d_name, "blockchain_backup_", 18) == 0) {
            time_t backup_time = atol(entry->d_name + 18);
            if (backup_time > latest_backup) {
                latest_backup = backup_time;
                snprintf(backup_file, sizeof(backup_file), "%s", entry->d_name);
                snprintf(backup_meta_file, sizeof(backup_meta_file), 
                        "blockchain_meta_backup_%ld.dat", backup_time);
            }
        }
    }
    closedir(dir);

    if (latest_backup == 0) return 0;

    // Load from backup
    FILE* file = fopen(backup_file, "rb");
    FILE* meta_file = fopen(backup_meta_file, "rb");
    if (!file || !meta_file) {
        if (file) fclose(file);
        if (meta_file) fclose(meta_file);
        return 0;
    }

    // Read metadata
    fread(&chain->block_count, sizeof(uint32_t), 1, meta_file);
    fread(&chain->difficulty, sizeof(int), 1, meta_file);
    fclose(meta_file);

    // Clear existing blockchain
    Block* current = chain->genesis;
    while (current) {
        Block* next = current->next;
        free(current);
        current = next;
    }
    chain->genesis = NULL;
    chain->latest = NULL;

    // Read blockchain data
    Block* previous = NULL;
    for (uint32_t i = 0; i < chain->block_count; i++) {
        Block* block = create_block(i, previous ? previous->hash : "");
        if (!block) {
            fclose(file);
            return 0;
        }

        fread(&block->id, sizeof(uint32_t), 1, file);
        fread(&block->timestamp, sizeof(time_t), 1, file);
        fread(block->previous_hash, sizeof(char), HASH_SIZE + 1, file);
        fread(block->hash, sizeof(char), HASH_SIZE + 1, file);
        fread(&block->nonce, sizeof(uint32_t), 1, file);
        fread(&block->transaction_count, sizeof(int), 1, file);
        
        for (int j = 0; j < block->transaction_count; j++) {
            fread(&block->transactions[j], sizeof(Transaction), 1, file);
        }

        if (previous) {
            previous->next = block;
        } else {
            chain->genesis = block;
        }
        previous = block;
    }

    chain->latest = previous;
    fclose(file);
    return 1;
} 