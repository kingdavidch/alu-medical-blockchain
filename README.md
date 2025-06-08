# ALU Medical Blockchain System

A blockchain-based medical records system implemented in C for the ALU Clinic. This system provides a secure, immutable, and transparent way of managing patient medical records.

## Features

- Secure blockchain implementation for medical records
- Proof of Work mining mechanism
- Command Line Interface (CLI) for system interaction
- SHA-256 hashing for data integrity
- Transaction handling for medical records
- Chain verification and integrity checking

## Project Structure

```
.
├── src/
│   ├── block.c
│   ├── block.h
│   ├── blockchain.c
│   ├── blockchain.h
│   ├── transaction.c
│   ├── transaction.h
│   ├── cli.c
│   ├── cli.h
│   ├── utils.c
│   └── utils.h
├── include/
│   └── sha256.h
├── Makefile
└── README.md
```

## Requirements

- GCC compiler (version 7.0 or higher)
- Make build system
- OpenSSL development libraries (for SHA-256)

## Installation

1. Install required dependencies:
   ```bash
   # For Ubuntu/Debian
   sudo apt-get install build-essential libssl-dev
   
   # For macOS
   brew install openssl
   ```

2. Clone the repository:
   ```bash
   git clone [repository-url]
   cd alu-medical-blockchain
   ```

3. Compile the project:
   ```bash
   make
   ```

## Usage

Run the program:
```bash
./medblockchain
```

Available commands:
- `add` - Add a new medical record
- `mine` - Mine a new block
- `view` - View the entire blockchain
- `verify` - Verify chain integrity
- `help` - Show available commands
- `exit` - Exit the program

## System Limitations

- Currently supports only local storage (no networking)
- Limited to basic Proof of Work consensus
- No user authentication system
- No encryption of sensitive data (basic implementation)

## Contributors

King Chukwumere - ALU Student

## License

This project is created for educational purposes as part of the ALU Software Engineering Low-Level Specialization coursework. 