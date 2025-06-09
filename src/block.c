#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "block.h"
#include "utils.h"
#include "security.h"

Block* create_block(uint32_t id, const char* previous_hash) {
    Block* block = (Block*)malloc(sizeof(Block));
    if (!block) {
        return NULL;
    }

    block->id = id;
    block->timestamp = time(NULL);
    block->transaction_count = 0;
    block->nonce = 0;
    block->next = NULL;

    if (previous_hash) {
        strncpy(block->previous_hash, previous_hash, HASH_SIZE);
        block->previous_hash[HASH_SIZE] = '\0';
    } else {
        memset(block->previous_hash, '0', HASH_SIZE);
        block->previous_hash[HASH_SIZE] = '\0';
    }

    memset(block->hash, 0, HASH_SIZE + 1);
    return block;
}

void calculate_block_hash(Block* block) {
    char data[1024];
    int offset = 0;

    // Combine block data into a single string
    offset += snprintf(data + offset, sizeof(data) - offset, "%u", block->id);
    offset += snprintf(data + offset, sizeof(data) - offset, "%ld", block->timestamp);
    offset += snprintf(data + offset, sizeof(data) - offset, "%u", block->nonce);
    offset += snprintf(data + offset, sizeof(data) - offset, "%s", block->previous_hash);

    // Add transaction data (only non-sensitive data)
    for (int i = 0; i < block->transaction_count; i++) {
        offset += snprintf(data + offset, sizeof(data) - offset, "%s%s%ld",
            block->transactions[i].patient_id,
            block->transactions[i].record_type,
            block->transactions[i].timestamp);
    }

    // Calculate SHA-256 hash
    sha256(data, block->hash);
}

int add_transaction(Block* block, const Transaction* transaction, const unsigned char* key) {
    if (!block || !transaction || !key || block->transaction_count >= MAX_TRANSACTIONS) {
        return 0;
    }

    // Validate transaction data
    if (!validate_patient_id(transaction->patient_id) ||
        !validate_record_type(transaction->record_type)) {
        return 0;
    }

    // Create new transaction
    Transaction* new_transaction = &block->transactions[block->transaction_count];
    strncpy(new_transaction->patient_id, transaction->patient_id, sizeof(new_transaction->patient_id) - 1);
    strncpy(new_transaction->record_type, transaction->record_type, sizeof(new_transaction->record_type) - 1);
    new_transaction->timestamp = transaction->timestamp;

    // If the input transaction already has encrypted data, copy it
    if (transaction->encrypted_data) {
        new_transaction->encrypted_data = (EncryptedData*)malloc(sizeof(EncryptedData));
        if (!new_transaction->encrypted_data) {
            return 0;
        }
        memcpy(new_transaction->encrypted_data, transaction->encrypted_data, sizeof(EncryptedData));
    } else {
        // Encrypt the data
        new_transaction->encrypted_data = encrypt_data((const char*)transaction->encrypted_data, key);
        if (!new_transaction->encrypted_data) {
            return 0;
        }
    }

    block->transaction_count++;

    // Recalculate block hash
    calculate_block_hash(block);
    return 1;
}

void free_block(Block* block) {
    if (block) {
        // Free encrypted data in transactions
        for (int i = 0; i < block->transaction_count; i++) {
            if (block->transactions[i].encrypted_data) {
                free_encrypted_data(block->transactions[i].encrypted_data);
            }
        }
        free(block);
    }
}

int verify_block(const Block* block) {
    if (!block) {
        return 0;
    }

    // Create a temporary copy of the block
    Block temp = *block;
    char original_hash[HASH_SIZE + 1];
    strcpy(original_hash, block->hash);

    // Recalculate hash
    calculate_block_hash(&temp);

    // Compare hashes
    return strcmp(original_hash, temp.hash) == 0;
}

void print_block(const Block* block, const unsigned char* key) {
    if (!block) {
        return;
    }

    printf("\nBlock #%u\n", block->id);
    printf("Timestamp: %s", get_timestamp_str(block->timestamp));
    printf("Previous Hash: %s\n", block->previous_hash);
    printf("Hash: %s\n", block->hash);
    printf("Nonce: %u\n", block->nonce);
    printf("Transactions: %d\n", block->transaction_count);

    for (int i = 0; i < block->transaction_count; i++) {
        printf("\nTransaction #%d:\n", i + 1);
        printf("  Patient ID: %s\n", block->transactions[i].patient_id);
        printf("  Type: %s\n", block->transactions[i].record_type);
        
        if (key && block->transactions[i].encrypted_data) {
            // Decrypt and print the data
            char* decrypted_data = decrypt_data(block->transactions[i].encrypted_data, key);
            if (decrypted_data) {
                printf("  Data: %s\n", decrypted_data);
                free(decrypted_data);
            } else {
                printf("  Data: [Encrypted]\n");
            }
        } else {
            printf("  Data: [Encrypted]\n");
        }
        
        printf("  Timestamp: %s", get_timestamp_str(block->transactions[i].timestamp));
    }
    printf("\n");
} 