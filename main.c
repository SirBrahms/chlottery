#include <dirent.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <sys/stat.h>

#define INITIAL_MAX_LEN 128

// returns a random number between min and max
int rand_range(int max, int min) {
	return rand() % (max + 1 - min) + max;
}

// randomises the access permissions of one file
// if special is true, it also randomises the special bits
// if verbose is true, it outputs the path and new mode
void roll_chmod(char* path, bool special, bool verbose) {
	int owner = rand_range(0, 7);
	int group = rand_range(0, 7);
	int other = rand_range(0, 7);
	int spec_perm = 0;

	if (special) 
		spec_perm = rand_range(0, 7);
	else
		spec_perm = 0;
	
	if (verbose) {
		const int mode_decimal = special * 1000 + owner * 100 + group * 10 + other;
		printf("%s - %04d\n", path, mode_decimal);
	}

	mode_t mode = spec_perm << 9 | owner << 6 | group << 3 | other;
	if (chmod(path, mode) != -1) {
		if (owner == 7 && group == 7 && other == 7)
			printf("JACKPOT!\n");
		else if (owner == 0 && group == 0 && other == 0)
			printf("COLOSSAL LOSS!\n");

		return;
	}
	else {
		perror("Couldn't set mode on file");
		exit(EXIT_FAILURE);
	}
}

// iterates through all subdirectories of a base directory and changes permissions
// if special is true, it also randomises the special bits
// if verbose is true, it outputs the path and the new mode
// (this function uses roll_chmod to set the modes on single files)
void roll_recursive(char* base_path, bool special, bool verbose) {
	char* buf = malloc(INITIAL_MAX_LEN * sizeof(char));
	struct dirent* dp;
	DIR* dir = opendir(base_path);

	if (!dir) {
		return;
	}
	
	while ((dp = readdir(dir)) != NULL) {
		if (strcmp(dp->d_name, ".") && strcmp(dp->d_name, "..")) {
			const int new_len = strlen(base_path) + strlen(dp->d_name) + 1;
			if (new_len > INITIAL_MAX_LEN) {
				buf = realloc(buf, new_len);
			}
			
			strcpy(buf, base_path);
			strcat(buf, "/");
			strcat(buf, dp->d_name);
			
			roll_recursive(buf, special, verbose);
			roll_chmod(buf, special, verbose);
		}

	}
	closedir(dir);
	free(buf);

	return;
}

// parses the arguments and passes them to the main operation functions
// also seeds the random number generator with the current time
int main(int argc, char** argv) {
	srand(time(NULL));
	bool recurse = false;
	bool special = false;
	bool verbose = false;
	
	int opt;
	while((opt = getopt(argc, argv, "sRhVv")) != -1) {
		switch (opt) {
			case 's':
				special = true;
				break;
			case 'R':
				recurse = true;
				break;
			case 'v':
				verbose = true;
				break;
			case 'V':
				printf("chlottery 1.0.0\n");
				exit(0);
			case 'h':
			default:
				printf("Usage: chlottery [-sRhvV] <path>\n");
				exit(1);
		}
	}


	if (optind >= argc) {
		fprintf(stderr, "Expected Path after options\n");
		exit(EXIT_FAILURE);
	}

	if (!recurse) {
		roll_chmod(argv[optind], special, verbose);
	}
	else { 
		roll_recursive(argv[optind], special, verbose);
		roll_chmod(argv[optind], special, verbose);
	}
}
