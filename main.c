#include <dirent.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <sys/stat.h>

#define INITIAL_MAX_LEN 128

int rand_range(int max, int min) {
	return rand() % (max + 1 - min) + max;
}

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
		int mode_decimal = special * 1000 + owner * 100 + group * 10 + other;
		printf("%s - %d\n", path, mode_decimal);
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

void roll_recursive(char* base_path, bool special, bool verbose) {
	char* buf = malloc(INITIAL_MAX_LEN * sizeof(char));
	struct dirent* dp;
	DIR* dir = opendir(base_path);

	if (!dir) {
		return;
	}
	
	while ((dp = readdir(dir)) != NULL) {
		if (strcmp(dp->d_name, ".") != 0 && strcmp(dp->d_name, "..") != 0) {
			if (strlen(base_path) >= INITIAL_MAX_LEN) {
				printf("Here %d\n", strlen(base_path));
				buf = realloc(buf, strlen(base_path) + strlen(dp->d_name + 1));
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
