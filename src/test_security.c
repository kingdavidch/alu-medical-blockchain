#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "block.h"
#include "blockchain.h"
#include "security.h"

// Test data
const char* TEST_PATIENT_ID = "P12345";
const char* TEST_RECORD_TYPE = "diagnosis";
const char* TEST_MEDICAL_DATA = "Patient shows symptoms of fever and cough. Prescribed rest and medication.";
const char* TEST_PASSWORD = "secure_password123";

void test_encryption(const unsigned char* key) {
    printf("\n=== Testing Encryption ===\n");
    
    // Test data encryption
    EncryptedData* encrypted = encrypt_data(TEST_MEDICAL_DATA, key);
    if (!encrypted) {
        printf("❌ Encryption failed\n");
        return;
    }
    printf("✅ Data encrypted successfully\n");
    
    // Test data decryption
    char* decrypted = decrypt_data(encrypted, key);
    if (!decrypted) {
        printf("❌ Decryption failed\n");
        free_encrypted_data(encrypted);
        return;
    }
    
    // Verify decrypted data matches original
    if (strcmp(decrypted, TEST_MEDICAL_DATA) == 0) {
        printf("✅ Decryption successful - Data matches original\n");
    } else {
        printf("❌ Decryption failed - Data mismatch\n");
    }
    
    free(decrypted);
    free_encrypted_data(encrypted);
}

void test_user_management(void) {
    printf("\n=== Testing User Management ===\n");
    
    // Create test users with different roles
    User* admin = create_user("admin", TEST_PASSWORD, 0);
    User* doctor = create_user("dr.smith", TEST_PASSWORD, 1);
    User* nurse = create_user("nurse.jones", TEST_PASSWORD, 2);
    User* viewer = create_user("viewer", TEST_PASSWORD, 3);
    
    if (!admin || !doctor || !nurse || !viewer) {
        printf("❌ User creation failed\n");
        return;
    }
    printf("✅ Users created successfully\n");
    
    // Test password verification
    if (verify_user(admin, TEST_PASSWORD) &&
        verify_user(doctor, TEST_PASSWORD) &&
        verify_user(nurse, TEST_PASSWORD) &&
        verify_user(viewer, TEST_PASSWORD)) {
        printf("✅ Password verification successful\n");
    } else {
        printf("❌ Password verification failed\n");
    }
    
    // Test incorrect password
    if (!verify_user(admin, "wrong_password")) {
        printf("✅ Incorrect password rejected\n");
    } else {
        printf("❌ Incorrect password accepted\n");
    }
    
    // Cleanup
    free_user(admin);
    free_user(doctor);
    free_user(nurse);
    free_user(viewer);
}

void test_access_control(void) {
    printf("\n=== Testing Access Control ===\n");
    
    // Create test users
    User* admin = create_user("admin", TEST_PASSWORD, 0);
    User* doctor = create_user("dr.smith", TEST_PASSWORD, 1);
    User* nurse = create_user("nurse.jones", TEST_PASSWORD, 2);
    User* viewer = create_user("viewer", TEST_PASSWORD, 3);
    
    // Test admin access
    printf("\nTesting Admin Access:\n");
    printf("Admin can read: %s\n", check_access(admin, "records", "read") ? "✅" : "❌");
    printf("Admin can write: %s\n", check_access(admin, "records", "write") ? "✅" : "❌");
    
    // Test doctor access
    printf("\nTesting Doctor Access:\n");
    printf("Doctor can read: %s\n", check_access(doctor, "records", "read") ? "✅" : "❌");
    printf("Doctor can write: %s\n", check_access(doctor, "records", "write") ? "✅" : "❌");
    
    // Test nurse access
    printf("\nTesting Nurse Access:\n");
    printf("Nurse can read: %s\n", check_access(nurse, "records", "read") ? "✅" : "❌");
    printf("Nurse can write vitals: %s\n", check_access(nurse, "vitals", "write") ? "✅" : "❌");
    printf("Nurse can write diagnosis: %s\n", check_access(nurse, "diagnosis", "write") ? "✅" : "❌");
    
    // Test viewer access
    printf("\nTesting Viewer Access:\n");
    printf("Viewer can read: %s\n", check_access(viewer, "records", "read") ? "✅" : "❌");
    printf("Viewer can write: %s\n", check_access(viewer, "records", "write") ? "✅" : "❌");
    
    // Cleanup
    free_user(admin);
    free_user(doctor);
    free_user(nurse);
    free_user(viewer);
}

void test_blockchain_security(const unsigned char* key) {
    printf("\n=== Testing Blockchain Security ===\n");
    
    // Create blockchain
    Blockchain* chain = create_blockchain();
    if (!chain) {
        printf("❌ Blockchain creation failed\n");
        return;
    }
    printf("✅ Blockchain created successfully\n");
    
    // Create and add a transaction
    Transaction transaction;
    strncpy(transaction.patient_id, TEST_PATIENT_ID, sizeof(transaction.patient_id) - 1);
    strncpy(transaction.record_type, TEST_RECORD_TYPE, sizeof(transaction.record_type) - 1);
    transaction.timestamp = time(NULL);
    
    // Encrypt the medical data
    transaction.encrypted_data = encrypt_data(TEST_MEDICAL_DATA, key);
    if (!transaction.encrypted_data) {
        printf("❌ Transaction encryption failed\n");
        free_blockchain(chain);
        return;
    }
    
    // Add transaction to the latest block
    if (add_transaction(chain->latest, &transaction, key)) {
        printf("✅ Transaction added successfully\n");
    } else {
        printf("❌ Transaction addition failed\n");
        free_encrypted_data(transaction.encrypted_data);
        free_blockchain(chain);
        return;
    }
    
    // Mine the block
    if (mine_block(chain, chain->latest)) {
        printf("✅ Block mined successfully\n");
    } else {
        printf("❌ Block mining failed\n");
    }
    
    // Verify chain integrity
    if (verify_chain(chain)) {
        printf("✅ Chain integrity verified\n");
    } else {
        printf("❌ Chain integrity check failed\n");
    }
    
    // Print the block to verify encryption/decryption
    printf("\nBlock Contents:\n");
    print_block(chain->latest, key);
    
    // Cleanup
    free_encrypted_data(transaction.encrypted_data);
    free_blockchain(chain);
}

int main(void) {
    printf("=== Medical Blockchain Security Test ===\n");
    
    // Generate a test key
    unsigned char key[AES_KEY_SIZE];
    if (!generate_key(key)) {
        printf("❌ Key generation failed\n");
        return 1;
    }
    printf("✅ Test key generated successfully\n");
    
    // Run security tests
    test_encryption(key);
    test_user_management();
    test_access_control();
    test_blockchain_security(key);
    
    printf("\n=== Security Tests Completed ===\n");
    return 0;
} 