#include <sys/types.h>
#include <dirent.h>
#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <pwd.h>
#include <grp.h>

struct direntNode
{
	struct dirent *directoryEntry;
	struct direntNode *prev;
	struct direntNode *next;
};

struct dirent_queue
{
	struct direntNode *front, *rear;
};

void dirent_queue_init(struct dirent_queue *queue)
{
	queue->front = queue->rear = NULL;
}

void enqueue(struct dirent_queue *queue, struct dirent *data)
{
	struct direntNode *node = (struct direntNode *)malloc(sizeof(struct direntNode));
	if (queue->front == NULL)
	{
		queue->front = node;
		queue->rear = node;
		node->directoryEntry = data;
		node->next = NULL;
		node->prev = NULL;
		return;
	}
	node->directoryEntry = data;
	node->next = NULL;
	node->prev = queue->rear;
	queue->rear->next = node;
	queue->rear = node;
}

void dequeueAll(struct dirent_queue *queue)
{
	if (queue->front == NULL)
		return;
	struct direntNode *node = queue->front->next;
	free(queue->front);
	while (node != NULL)
	{
		queue->front = node;
		node = queue->front->next;
		free(queue->front);
	}
}

void sortDirentQueue(struct dirent_queue *queue)
{
	struct direntNode *i_node;
	struct direntNode *j_node;
	for (i_node = queue->front; i_node != NULL; i_node = i_node->next)
	{
		for (j_node = queue->front; j_node->next != NULL; j_node = j_node->next)
		{
			if (strcmp(j_node->directoryEntry->d_name, j_node->next->directoryEntry->d_name) > 0)
			{
				struct dirent *temp = i_node->directoryEntry;
				i_node->directoryEntry = j_node->directoryEntry;
				j_node->directoryEntry = temp;
			}
		}
	}
}

int main(int argc, char const *argv[])
{
	errno = 0;

	if (argc > 3)
	{
		perror("Invalid arguments");
		return 1;
	}

	char currentWorkingDirectory[300];
	getcwd(currentWorkingDirectory, 300);

	struct dirent *directoryEntry;

	DIR *directoryStream = opendir(currentWorkingDirectory);
	if (directoryStream == NULL)
	{
		perror("Stream not accessed");
		return 1;
	}

	struct dirent_queue queue;
	dirent_queue_init(&queue);
	while ((directoryEntry = readdir(directoryStream)) != NULL && errno == 0)
	{
		enqueue(&queue, directoryEntry);
	}

	sortDirentQueue(&queue);

	struct direntNode *node = queue.front;

	if (argc == 1)
	{
		while (node != NULL)
		{
			printf("%s\n", node->directoryEntry->d_name);
			node = node->next;
		}
	}
	else if (argc >= 2)
	{
		while (node != NULL)
		{
			char permissions[] = "----------";
			struct stat statbuf;
			if (argc == 2)
			{
				if (stat(node->directoryEntry->d_name, &statbuf) == -1)
				{
					perror("stat not executed successfully");
					return 1;
				}
			}
			else if (argc == 3)
			{
				if (stat(currentWorkingDirectory, &statbuf) == -1)
				{
					perror("stat not executed successfully");
					return 1;
				}
			}
			// Owner permissions
			if (statbuf.st_mode & S_IRUSR)
				permissions[1] = 'r';
			if (statbuf.st_mode & S_IWUSR)
				permissions[2] = 'w';
			if (statbuf.st_mode & S_IXUSR)
			{
				if (statbuf.st_mode & S_ISUID)
					permissions[3] = 's';
				else
					permissions[3] = 'x';
			}
			// Group permissions
			if (statbuf.st_mode & S_IRGRP)
				permissions[4] = 'r';
			if (statbuf.st_mode & S_IWGRP)
				permissions[5] = 'w';
			if (statbuf.st_mode & S_IXGRP)
			{
				if (statbuf.st_mode & S_ISGID)
					permissions[6] = 's';
				else
					permissions[6] = 'x';
			}
			// Others permissions
			if (statbuf.st_mode & S_IROTH)
				permissions[1] = 'r';
			if (statbuf.st_mode & S_IWOTH)
				permissions[2] = 'w';
			if (statbuf.st_mode & S_IXOTH)
				permissions[9] = 'x';

			printf("%s ", permissions);
			printf("Number of hard links: %hu ", statbuf.st_nlink);

			struct passwd *presentWorkingDirectory;
			struct group *grp;
			if ((presentWorkingDirectory = getpwuid(statbuf.st_uid)) == NULL)
			{
				printf("Can't find ");
			}
			else
			{
				printf("%s ", presentWorkingDirectory->pw_name);
			}
			if ((grp = getpwuid(statbuf.st_gid)) == NULL)
			{
				printf("Can't find ");
			}
			else
			{
				printf("%s ", grp->gr_name);
			}

			printf("%lld ", statbuf.st_size);
			printf("%s\n", node->directoryEntry->d_name);
			node = node->next;
		}
	}
	dequeueAll(&queue);
	closedir(directoryStream);
	return 0;
}
