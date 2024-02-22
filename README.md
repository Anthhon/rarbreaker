# rarbreaker

The **rarbreaker** is a command-line utility designed to crack passwords of encrypted files using a brute-force approach. It generates random passwords based on user-defined characters and length, attempting to unlock the encrypted file.

## Table of Contents

- [Installation](#installation)
- [Usage](#usage)
- [Options](#options)
- [Examples](#examples)
- [License](#license)

## Installation

To use the **Password Cracker**, you'll need to compile the source code provided. Ensure you have a C compiler installed on your system.

1. Clone the repository:

    ```bash
    git clone https://github.com/your_username/password-cracker.git
    ```

2. Navigate to the project directory:

    ```bash
    cd password-cracker
    ```

3. Compile the source code:

    ```bash
    make install-linux -B
    # or
    make install-windows -B
    ```

This will generate an executable file named `password-cracker`.

## Usage

The general usage of the **Password Cracker** utility is as follows:

```bash
./password-cracker -c <characters> -l <pass_len> -f <filepath>
```

Replace `<characters>` with the range of characters to be used for password generation, `<pass_len>` with the desired password length, and `<filepath>` with the path to the encrypted file you want to crack.

## Options

- `-c <characters>`: Specifies the range of characters to use for password generation.
- `-l <pass_len>`: Specifies the length of the password to generate.
- `-f <filepath>`: Specifies the path to the encrypted file to crack.
- `-h`: Displays the help message.

## Examples

Here are some examples demonstrating how to use the **Password Cracker**:

1. Crack an encrypted ZIP file using lowercase letters, uppercase letters, and digits with a password length of 8 characters:

    ```
    ./password-cracker -c abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789 -l 8 -f encrypted.zip
    ```

2. Crack a password-protected PDF file using only lowercase letters with a password length of 6 characters:

    ```
    ./password-cracker -c abcdefghijklmnopqrstuvwxyz -l 6 -f protected.pdf
    ```

## License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.
