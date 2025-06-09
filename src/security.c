#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <openssl/aes.h>
#include <openssl/rand.h>
#include <openssl/evp.h>
#include <openssl/sha.h>
#include "security.h"

// Encryption functions
EncryptedData* encrypt_data(const char* data, const unsigned char* key) {
    if (!data || !key) return NULL;

    EncryptedData* encrypted = (EncryptedData*)malloc(sizeof(EncryptedData));
    if (!encrypted) return NULL;

    // Generate random IV
    if (RAND_bytes(encrypted->iv, AES_IV_SIZE) != 1) {
        free(encrypted);
        return NULL;
    }

    // Initialize encryption context
    EVP_CIPHER_CTX* ctx = EVP_CIPHER_CTX_new();
    if (!ctx) {
        free(encrypted);
        return NULL;
    }

    // Initialize encryption
    if (EVP_EncryptInit_ex(ctx, EVP_aes_256_cbc(), NULL, key, encrypted->iv) != 1) {
        EVP_CIPHER_CTX_free(ctx);
        free(encrypted);
        return NULL;
    }

    // Calculate output size
    int data_len = strlen(data);
    int block_size = EVP_CIPHER_CTX_block_size(ctx);
    int encrypted_len = data_len + block_size;
    encrypted->data = (unsigned char*)malloc(encrypted_len);
    if (!encrypted->data) {
        EVP_CIPHER_CTX_free(ctx);
        free(encrypted);
        return NULL;
    }

    // Perform encryption
    int len;
    if (EVP_EncryptUpdate(ctx, encrypted->data, &len, (const unsigned char*)data, data_len) != 1) {
        EVP_CIPHER_CTX_free(ctx);
        free(encrypted->data);
        free(encrypted);
        return NULL;
    }
    encrypted->data_len = len;

    // Finalize encryption
    if (EVP_EncryptFinal_ex(ctx, encrypted->data + len, &len) != 1) {
        EVP_CIPHER_CTX_free(ctx);
        free(encrypted->data);
        free(encrypted);
        return NULL;
    }
    encrypted->data_len += len;

    EVP_CIPHER_CTX_free(ctx);
    return encrypted;
}

char* decrypt_data(const EncryptedData* encrypted, const unsigned char* key) {
    if (!encrypted || !key) return NULL;

    // Initialize decryption context
    EVP_CIPHER_CTX* ctx = EVP_CIPHER_CTX_new();
    if (!ctx) return NULL;

    // Initialize decryption
    if (EVP_DecryptInit_ex(ctx, EVP_aes_256_cbc(), NULL, key, encrypted->iv) != 1) {
        EVP_CIPHER_CTX_free(ctx);
        return NULL;
    }

    // Allocate output buffer
    char* decrypted = (char*)malloc(encrypted->data_len + 1);
    if (!decrypted) {
        EVP_CIPHER_CTX_free(ctx);
        return NULL;
    }

    // Perform decryption
    int len;
    if (EVP_DecryptUpdate(ctx, (unsigned char*)decrypted, &len, encrypted->data, encrypted->data_len) != 1) {
        EVP_CIPHER_CTX_free(ctx);
        free(decrypted);
        return NULL;
    }
    int decrypted_len = len;

    // Finalize decryption
    if (EVP_DecryptFinal_ex(ctx, (unsigned char*)decrypted + len, &len) != 1) {
        EVP_CIPHER_CTX_free(ctx);
        free(decrypted);
        return NULL;
    }
    decrypted_len += len;
    decrypted[decrypted_len] = '\0';

    EVP_CIPHER_CTX_free(ctx);
    return decrypted;
}

void free_encrypted_data(EncryptedData* encrypted) {
    if (encrypted) {
        if (encrypted->data) {
            free(encrypted->data);
        }
        free(encrypted);
    }
}

// User management functions
User* create_user(const char* username, const char* password, int role) {
    if (!username || !password) return NULL;

    User* user = (User*)malloc(sizeof(User));
    if (!user) return NULL;

    strncpy(user->username, username, sizeof(user->username) - 1);
    user->username[sizeof(user->username) - 1] = '\0';
    user->role = role;

    // Generate random salt
    if (RAND_bytes(user->salt, SALT_SIZE) != 1) {
        free(user);
        return NULL;
    }

    // Hash password with salt
    SHA256_CTX sha256;
    SHA256_Init(&sha256);
    SHA256_Update(&sha256, password, strlen(password));
    SHA256_Update(&sha256, user->salt, SALT_SIZE);
    SHA256_Final(user->password_hash, &sha256);

    return user;
}

int verify_user(const User* user, const char* password) {
    if (!user || !password) return 0;

    unsigned char hash[32];
    SHA256_CTX sha256;
    SHA256_Init(&sha256);
    SHA256_Update(&sha256, password, strlen(password));
    SHA256_Update(&sha256, user->salt, SALT_SIZE);
    SHA256_Final(hash, &sha256);

    return memcmp(hash, user->password_hash, 32) == 0;
}

void free_user(User* user) {
    if (user) {
        free(user);
    }
}

// Key management functions
int generate_key(unsigned char* key) {
    return RAND_bytes(key, AES_KEY_SIZE) == 1;
}

int store_key(const unsigned char* key, const char* filename) {
    FILE* file = fopen(filename, "wb");
    if (!file) return 0;

    size_t written = fwrite(key, 1, AES_KEY_SIZE, file);
    fclose(file);
    return written == AES_KEY_SIZE;
}

int load_key(unsigned char* key, const char* filename) {
    FILE* file = fopen(filename, "rb");
    if (!file) return 0;

    size_t read = fread(key, 1, AES_KEY_SIZE, file);
    fclose(file);
    return read == AES_KEY_SIZE;
}

// Access control function
int check_access(const User* user, const char* resource, const char* action) {
    if (!user || !resource || !action) return 0;

    // Simple role-based access control
    switch (user->role) {
        case 0: // Admin
            return 1; // Full access
        case 1: // Doctor
            if (strcmp(action, "read") == 0) return 1;
            if (strcmp(action, "write") == 0) return 1;
            return 0;
        case 2: // Nurse
            if (strcmp(action, "read") == 0) return 1;
            if (strcmp(action, "write") == 0 && 
                (strcmp(resource, "vitals") == 0 || 
                 strcmp(resource, "medication") == 0)) return 1;
            return 0;
        case 3: // Read-only
            return strcmp(action, "read") == 0;
        default:
            return 0;
    }
} 