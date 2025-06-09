#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <openssl/sha.h>
#include <openssl/evp.h>
#include "utils.h"

void sha256(const char* input, char* output) {
    unsigned char hash[SHA256_DIGEST_LENGTH];
    EVP_MD_CTX *mdctx = EVP_MD_CTX_new();
    if (!mdctx) {
        // Handle error, e.g., log or return early if applicable
        return;
    }

    if (EVP_DigestInit_ex(mdctx, EVP_sha256(), NULL) != 1) {
        EVP_MD_CTX_free(mdctx);
        return;
    }
    if (EVP_DigestUpdate(mdctx, input, strlen(input)) != 1) {
        EVP_MD_CTX_free(mdctx);
        return;
    }
    unsigned int md_len;
    if (EVP_DigestFinal_ex(mdctx, hash, &md_len) != 1) {
        EVP_MD_CTX_free(mdctx);
        return;
    }
    EVP_MD_CTX_free(mdctx);

    // Convert to hex string
    for (int i = 0; i < SHA256_DIGEST_LENGTH; i++) {
        sprintf(output + (i * 2), "%02x", hash[i]);
    }
    output[SHA256_DIGEST_LENGTH * 2] = '\0';
}

int is_valid_hash(const char* hash, int difficulty) {
    for (int i = 0; i < difficulty; i++) {
        if (hash[i] != '0') {
            return 0;
        }
    }
    return 1;
}

void increment_nonce(uint32_t* nonce) {
    (*nonce)++;
}

void str_to_hex(const unsigned char* input, char* output, size_t length) {
    for (size_t i = 0; i < length; i++) {
        sprintf(output + (i * 2), "%02x", input[i]);
    }
    output[length * 2] = '\0';
}

void hex_to_str(const char* input, unsigned char* output, size_t length) {
    for (size_t i = 0; i < length; i++) {
        sscanf(input + (i * 2), "%2hhx", &output[i]);
    }
}

char* get_timestamp_str(time_t timestamp) {
    static char buffer[32];
    struct tm* tm_info = localtime(&timestamp);
    strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S\n", tm_info);
    return buffer;
}

int validate_patient_id(const char* patient_id) {
    if (!patient_id || strlen(patient_id) == 0 || strlen(patient_id) > 31) {
        return 0;
    }
    // Add more validation rules as needed
    return 1;
}

int validate_record_type(const char* record_type) {
    if (!record_type || strlen(record_type) == 0 || strlen(record_type) > 31) {
        return 0;
    }
    // Add more validation rules as needed
    return 1;
}

int validate_record_data(const char* data) {
    if (!data || strlen(data) == 0 || strlen(data) > 255) {
        return 0;
    }
    // Add more validation rules as needed
    return 1;
} 