#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

int main() {
    // File pointers for different input/output files
    FILE* intermediate, * optab, * symtab, * length, * object, * listing;

    // Variables to store data read from files
    char label[20], opcode[20], operand[20];
    char optab_code[20], optab_addr[20];
    char sym_label[20], sym_addr[20];
    char object_code[30] = "";   // will hold generated object code for each instruction

    int locctr, start_addr = 0, prog_length, text_len;
    bool opcode_found, sym_found;

    // Open all necessary files
    intermediate = fopen("intermediate.txt", "r");  // from pass 1
    optab = fopen("optab.txt", "r");                // opcode table
    symtab = fopen("symtab.txt", "r");              // symbol table
    length = fopen("length.txt", "r");              // contains program length
    object = fopen("object_program.txt", "w+");     // output: object program
    listing = fopen("listing.txt", "w");            // output: listing file (human readable)

    // Check if all files opened successfully
    if (!intermediate || !optab || !symtab || !length || !object || !listing) {
        printf("Error: Could not open one or more files.\n");
        return 1;
    }

    // Read total program length (from pass1)
    fscanf(length, "%d", &prog_length);

    // Read first line from intermediate file (it should be the START line)
    fscanf(intermediate, "%s%s%s", label, opcode, operand);

    // If the first line is START
    if (strcmp(opcode, "START") == 0) {
        // Convert operand (which is starting address) from hex to int
        start_addr = (int)strtol(operand, NULL, 16);

        // Write Header record: H^PROGNAME^STARTADDR^LENGTH
        fprintf(object, "H^%s^%06X^%06X\n", label, start_addr, prog_length);

        // Write the same line in listing file (for display)
        fprintf(listing, "\t\t%s\t%s\t%s\n", label, opcode, operand);

        // Move to next line in intermediate file
        fscanf(intermediate, "%x%s%s%s", &locctr, label, opcode, operand);
    }

    // Start a new Text record (T) from current location
    text_len = 0;
    long text_len_pos = ftell(object);  // remember position to later fill length
    fprintf(object, "T^%06X^00", locctr);  // temporarily write "00" for length (we’ll fix later)

    // Process each instruction until END
    while (strcmp(opcode, "END") != 0) {
        strcpy(object_code, "");     // clear object_code
        opcode_found = false;

        // Search opcode in OPTAB
        rewind(optab);  // go to start of optab file
        while (fscanf(optab, "%s%s", optab_code, optab_addr) == 2) {
            if (strcmp(optab_code, opcode) == 0) {
                opcode_found = true;
                break;
            }
        }

        // If opcode is found in OPTAB
        if (opcode_found) {
            char operand_addr[10] = "0000";   // default address if no operand

            // If operand is not ** (meaning not empty)
            if (strcmp(operand, "**") != 0) {
                rewind(symtab);  // go to start of symbol table
                sym_found = false;

                // Search for operand in SYMTAB
                while (fscanf(symtab, "%s%s", sym_label, sym_addr) == 2) {
                    if (strcmp(sym_label, operand) == 0) {
                        strcpy(operand_addr, sym_addr);  // copy address from symtab
                        sym_found = true;
                        break;
                    }
                }

                // If symbol not found
                if (!sym_found) {
                    printf("Error: Undefined symbol %s\n", operand);
                }
            }

            // Combine opcode machine code + operand address → object code
            sprintf(object_code, "%s%s", optab_addr, operand_addr);
        }

        // If opcode = BYTE
        else if (strcmp(opcode, "BYTE") == 0) {
            if (operand[0] == 'C') {   // e.g., C'EOF'
                for (int i = 2; i < strlen(operand) - 1; i++) {
                    char temp[5];
                    sprintf(temp, "%X", operand[i]);  // convert each char to hex
                    strcat(object_code, temp);
                }
            }
            else if (operand[0] == 'X') {  // e.g., X'F1'
                strncpy(object_code, operand + 2, strlen(operand) - 3);
                object_code[strlen(operand) - 3] = '\0';
            }
        }

        // If opcode = WORD (3-byte constant)
        else if (strcmp(opcode, "WORD") == 0) {
            sprintf(object_code, "%06X", atoi(operand)); // convert to 6-digit hex
        }

        // Write generated object code into text record
        if (strlen(object_code) > 0) {
            fprintf(object, "^%s", object_code);
            text_len += strlen(object_code) / 2;  // add its byte length
        }

        // Write line into listing file
        fprintf(listing, "%04X\t%-8s%-8s%-6s\t%s\n", locctr, label, opcode, operand, object_code);

        // Read next line from intermediate
        fscanf(intermediate, "%x%s%s%s", &locctr, label, opcode, operand);
    }

    // Now we have reached END, we need to fix the text record length
    long curr_pos = ftell(object);        // current position in file
    fseek(object, text_len_pos + 9, SEEK_SET);  // go to where length was written
    fprintf(object, "%02X", text_len);    // overwrite the "00" with actual length
    fseek(object, curr_pos, SEEK_SET);    // move back to end of file

    // Write END record (program end)
    fprintf(listing, "%04X\t%-8s%-8s%-6s\n", locctr, label, opcode, operand);
    fprintf(object, "\nE^%06X\n", start_addr);

    // Close all files
    fclose(intermediate);
    fclose(optab);
    fclose(symtab);
    fclose(length);
    fclose(object);
    fclose(listing);

    printf("Pass 2 completed.\n");
    return 0;
}
