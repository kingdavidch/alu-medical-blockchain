#ifndef PERSISTENCE_H
#define PERSISTENCE_H

#include "blockchain.h"

// File paths for blockchain storage
#define BLOCKCHAIN_FILE "blockchain.dat"
#define BLOCKCHAIN_META_FILE "blockchain_meta.dat"

// Function declarations
int save_blockchain(const Blockchain* chain);
Blockchain* load_blockchain(void);
int backup_blockchain(const Blockchain* chain);
int restore_blockchain(Blockchain* chain);

#endif // PERSISTENCE_H 