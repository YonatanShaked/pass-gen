#include <ctype.h>
#include <openssl/rand.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#define VERSION "1.2"

#define MIN_LEN 8
#define MAX_LEN 64

#define RED "\033[1;31m"
#define BLUE "\033[1;34m"
#define NC "\033[0m"

size_t get_random_seed(void)
{
	size_t ssl_seed;
	if (RAND_bytes((uint8_t*)&ssl_seed, sizeof(ssl_seed)) != 1)
	{
		fprintf(stderr, "Error generating random seed - Aborting\n");
		exit(1);
	}

	size_t seed = ssl_seed ^ (size_t)time(NULL) ^ (size_t)getpid();

	return seed;
}

char* get_random_password(size_t pass_len)
{
	const char* charsets[] = {"!@#$%^&*", "0123456789", "ABCDEFGHIJKLMNOPQRSTUVWXYZ", "abcdefghijklmnopqrstuvwxyz"};
	const size_t charsets_len = sizeof(charsets) / sizeof(charsets[0]);

	char* pass = (char*)malloc((pass_len + 1) * sizeof(char));
	if (pass == NULL)
	{
		fprintf(stderr, "Failed to allocate memory - Aborting\n");
		exit(1);
	}

	bool has_symbol = false;
	bool has_digit = false;

	while(!has_digit || !has_symbol)
	{
		for (size_t i = 0u; i < pass_len; i++)
		{
			size_t charset_index = rand() % charsets_len;

			if (charset_index == 0)
				has_symbol = true;

			if (charset_index == 1)
				has_digit = true;

			const char* charset = charsets[charset_index];
			pass[i] = charset[rand() % strlen(charset)];
		}
	}

	pass[pass_len] = '\0';

	return pass;
}

void pretty_print(const char* str)
{
	while (*str)
	{
		if (isdigit(*str))
			printf("%s%c%s", BLUE, *str, NC);
		else if (ispunct(*str))
			printf("%s%c%s", RED, *str, NC);
		else
			printf("%c", *str);

		str++;
	}
	printf("\n");
}

void print_usage(const char* exec)
{
	printf("<Usage>: %s [OPTIONS]\n", exec);
	printf("Options:\n");
	printf(" -l, --length \t Set the length of the password (between %d and %d).\n", MIN_LEN, MAX_LEN);
	printf(" -h, --help \t Display this help message.\n");
	printf(" -v, --version \t Display version information.\n");
}

size_t parse_args(int argc, char** argv)
{
	size_t pass_len = 0;

	switch (argc)
	{
		case 1:
			pass_len = 14;
			break;

		case 2:
			if (strcmp(argv[1], "-h") == 0 || strcmp(argv[1], "--help") == 0)
			{
				print_usage(argv[0]);
				exit(0);
			}
			else if (strcmp(argv[1], "-v") == 0 || strcmp(argv[1], "--version") == 0)
			{
				printf("%s\n", VERSION);
				exit(0);
			}
			else
			{
				fprintf(stderr, "Invalid arguments - Aborting\n");
				print_usage(argv[0]);
				exit(1);
			}
			break;

		case 3:
			if (strcmp(argv[1], "-l") == 0 || strcmp(argv[1], "--length") == 0)
			{
				char* endptr;
				long length = strtol(argv[2], &endptr, 10);
				if (endptr == argv[2] || *endptr != '\0' || length < MIN_LEN || length > MAX_LEN)
				{
					fprintf(stderr, "Invalid password length: %s - Aborting\n", argv[2]);
					print_usage(argv[0]);
					exit(1);
				}
				pass_len = (size_t)length;
			}
			break;

		default:
			fprintf(stderr, "Invalid arguments - Aborting\n");
			print_usage(argv[0]);
			exit(1);
	}

	return pass_len;
}

int main(int argc, char** argv)
{
	if (RAND_poll() != 1)
	{
		fprintf(stderr, "Error initializing OpenSSL PRNG - Aborting\n");
		exit(1);
	}

	srand(get_random_seed());

	size_t pass_len = parse_args(argc, argv);
	char* pass = get_random_password(pass_len);
	if (pass != NULL)
	{
		printf("Generated password length: %zu characters\n", pass_len);
		pretty_print(pass);
		free(pass);
	}

	return 0;
}

