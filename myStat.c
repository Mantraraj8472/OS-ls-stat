#include <sys/stat.h>
#include <stdio.h>
#include <pwd.h>
#include <grp.h>

int main(int argc, char const *argv[])
{
	struct stat statbuf;
	if (argc != 2)
	{
		perror("Invalid arguments");
		return 1;
	}

	for (int i = 1; i < argc; i++)
	{
		if (stat(argv[i], &statbuf) == -1)
		{
			perror("stat not executed successfully");
			return 1;
		}
		printf("File name: %s\n", argv[i]);
		/*
		The following macros shall be provided to test whether a file is
		of the specified type.  The value m supplied to the macros is the value
		of st_mode from a stat structure.  The macro shall evaluate to a non-zero
		value if the test is true; 0 if the test is false.
		*/
		if (S_ISBLK(statbuf.st_mode) != 0)
		{
			printf("Type: Block Device\n");
		}
		else if (S_ISCHR(statbuf.st_mode) != 0)
		{
			printf("Type: Character Device\n");
		}
		else if (S_ISDIR(statbuf.st_mode) != 0)
		{
			printf("Type: Directory\n");
		}
		else if (S_ISFIFO(statbuf.st_mode) != 0)
		{
			printf("Type: FIFO or Socket\n");
		}
		else if (S_ISREG(statbuf.st_mode) != 0)
		{
			printf("Type: Regular File\n");
		}
		else if (S_ISLNK(statbuf.st_mode) != 0)
		{
			printf("Type: Symbolic Link\n");
		}
		else if (S_ISSOCK(statbuf.st_mode) != 0)
		{
			printf("Type: Socket\n");
		}
		else
		{
			printf("Type: Unknown File\n");
		}
		printf("User ID: %lu\n", statbuf.st_uid);
		printf("Group ID: %lu\n", statbuf.st_gid);

		struct passwd *presentWorkingDirectory;
		struct group *grp;

		if ((presentWorkingDirectory = getpwuid(statbuf.st_uid)) == NULL)
		{
			printf("Can't find owner name from UID\n");
		}
		else
		{
			printf("Owner name: %s\n", presentWorkingDirectory->pw_name);
		}
		if ((grp = getpwuid(statbuf.st_gid)) == NULL)
		{
			printf("Can't find group name from GID\n");
		}
		else
		{
			printf("Group name: %s\n", grp->gr_name);
		}
		printf("Inode number: %ld\n", statbuf.st_ino);
		printf("File size: %zu bytes\n", statbuf.st_size);
		printf("Number of blocks allocated: %ld\n", statbuf.st_blocks);
		printf("Number of hard links: %ld\n", statbuf.st_nlink);
	}
	return 0;
}
