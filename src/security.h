#ifndef SECURITY_H
#define SECURITY_H

#include <openssl/aes.h>
#include <openssl/rand.h>
#include <openssl/evp.h>
#include <stdint.h>

#define AES_KEY_SIZE 32  // 256 bits
#define AES_IV_SIZE 16   // 128 bits
#define MAX_PASSWORD_LENGTH 64
#define SALT_SIZE 16

// Structure for encrypted data
typedef struct {
    unsigned char iv[AES_IV_SIZE];
    unsigned char* data;
    size_t data_len;
} EncryptedData;

// Structure for user credentials
typedef struct {
    char username[32];
    unsigned char password_hash[32];  // SHA-256 hash
    unsigned char salt[SALT_SIZE];
    int role;  // 0: admin, 1: doctor, 2: nurse, 3: read-only
} User;

// Function declarations
EncryptedData* encrypt_data(const char* data, const unsigned char* key);
char* decrypt_data(const EncryptedData* encrypted, const unsigned char* key);
void free_encrypted_data(EncryptedData* encrypted);

// User management
User* create_user(const char* username, const char* password, int role);
int verify_user(const User* user, const char* password);
void free_user(User* user);

// Key management
int generate_key(unsigned char* key);
int store_key(const unsigned char* key, const char* filename);
int load_key(unsigned char* key, const char* filename);

// Access control
int check_access(const User* user, const char* resource, const char* action);

#endif // SECURITY_H 