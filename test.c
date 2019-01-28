#include "include/rl.h"
#include <unistd.h>

#define PROMPT "test > "

int main() {
	char		buf[5000];
	ssize_t		size;

	while ((size = readline(&(struct rl_state) {
		.prompt = PROMPT,
		.prompt_size = sizeof(PROMPT) - 1,
		.buffer = buf,
		.buffer_size = sizeof(buf)
	})) >= 0)
	{
		write(1, "\n", 2);
		if (size)
		{
			write(1, "echo:  ", 7);
			write(1, buf, size);
			write(1, "\n", 1);
		}
	}
	write(1, "\nBye\n", 4);
}
