/* fread example: read an entire file */
// http://www.cplusplus.com/reference/cstdio/fread/

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <errno.h>
#include <string.h>

#define int64 long long
#define min(x,y) (((x)<(y))?(x):(y))

int main()
{
	int64 totalSize = (int64)1024 * 1024 * 1024 * 2 + 100;
	const size_t bufferSize = 4096;
	// const size_t bufferSize = 0x7ffff000 + 64;
	printf("totalSize  is %lld\n", totalSize);
	printf("bufferSize is %zu\n", bufferSize);

	FILE* pFile = fopen("test.txt", "rb+");
	if (pFile == NULL)
	{
		printf("fopen failed, errno is %d\n", errno);
		return -1;
	}

	char* buffer = (char*)malloc(bufferSize * sizeof(char));
	memset(buffer, '1', bufferSize);


	int64 i = 0;  // write time for log
	int64 hadWrittenSize = 0;
	bool hadError = false;
	while (hadWrittenSize < totalSize)
	{
		i++;

		int64 needsWriteSize = min(totalSize - hadWrittenSize, bufferSize);
		size_t wroteSize = fwrite(buffer, sizeof(char), needsWriteSize, pFile);
		if (wroteSize != needsWriteSize)
		{
			printf("warning: %lld times: write %zu != %lld, errno is %d\n",
				i, wroteSize, needsWriteSize, errno);
		}
		hadWrittenSize += wroteSize;

		int64 offSet = (int64)ftell(pFile);
		if (offSet != hadWrittenSize)
		{
			hadError = true;
			printf("error: %lld times: lseek had error(%lld != %lld), errno is %d\n",
				i, offSet, hadWrittenSize, errno);
		}

		// 27: EFBIG           File too large
		if (hadError && errno == 27)
		{
			printf("please use _FILE_OFFSET_BITS=64 for support big file\n");
			break;
		}
	}
	fclose(pFile);
	printf("after write test, hadError? %d\n", hadError);
	free(buffer);

	return 0;
}