#include "include/fileReader.h"
#include <stdlib.h>
#include <stdio.h>

char* get_file_contents(const char* filepath)
{
    char* buffer = 0;
    long length;

    // Open the file in text mode
    FILE* file = fopen(filepath, "r");

    if (file)
    {
        // Seek to the end to find the file size
        fseek(file, 0, SEEK_END);
        length = ftell(file);
        fseek(file, 0, SEEK_SET); // Rewind the file to the beginning

        // Allocate memory for the entire content plus a null-terminator
        buffer = calloc(length + 1, sizeof(char));

        if (buffer)
        {
            // Read the file into the buffer
            fread(buffer, sizeof(char), length, file);
            buffer[length] = '\0'; // Ensure the string is null-terminated
        }
        fclose(file);

        // Remove all carriage return '\r' characters from the buffer
        int writeIndex = 0;
        for (int readIndex = 0; readIndex <= length; readIndex++)
        {
            if (buffer[readIndex] != '\r') // Check if it's not a carriage return
            {
                buffer[writeIndex++] = buffer[readIndex];
            }
        }
    }
    else
    {
        printf("Error opening file: %s\n", filepath);
        exit(EXIT_FAILURE);
    }

    return buffer;
}
