#ifndef UTILS_H
#define UTILS_H

#include <stdint.h>
#include <stddef.h>  // for size_t
#include <time.h>    // for time_t

// SHA-256 hash function
void sha256(const char* input, char* output);

// Proof of work functions
int is_valid_hash(const char* hash, int difficulty);
void increment_nonce(uint32_t* nonce);

// String utilities
void str_to_hex(const unsigned char* input, char* output, size_t length);
void hex_to_str(const char* input, unsigned char* output, size_t length);

// Time utilities
char* get_timestamp_str(time_t timestamp);

// Input validation
int validate_patient_id(const char* patient_id);
int validate_record_type(const char* record_type);
int validate_record_data(const char* data);

#endif // UTILS_H 