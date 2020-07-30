#include <errno.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include <iostream>

size_t fincore(int fd, size_t length) {
  size_t page_size = getpagesize();

  unsigned char *vec;
  unsigned char *mincore_vec;
  vec = reinterpret_cast<unsigned char *>(
      calloc(1, (length + page_size - 1) / page_size));
  if (mincore_vec == nullptr) {
    perror("calloc");
    return 0;
  }

  // NOTE: get memory address for mincore()
  void *file_mmap = mmap(nullptr, length, PROT_NONE, MAP_SHARED, fd, 0);
  auto cleanup    = [&]() {
    if (file_mmap != MAP_FAILED) {
      munmap(file_mmap, length);
    }
    if (vec != nullptr) {
      free(mincore_vec);
    }
    return 0;
  };
  if (file_mmap == MAP_FAILED) {
    perror("mmap");
    return cleanup();
  }

  if (mincore(file_mmap, length, vec) != 0) {
    perror("mincore");
    return cleanup();
  }

  size_t cached = 0;
  for (size_t page_index = 0; page_index <= length / page_size; page_index++) {
    if (vec[page_index] & 1) {
      ++cached;
    }
  }

  size_t size = cached * page_size;
  return size;
}

int main(int argc, char **argv) {
  int fd;
  struct stat st;
  char *file_path;
  if (argc < 2) {
    std::cout << argv[0] << " <filepath>" << std::endl;
    return 1;
  }

  file_path = argv[1];

  fd = open(file_path, O_RDONLY);
  if (fd < 0) {
    std::cerr << file_path << ": ";
    perror("open");
    return 1;
  }
  if (fstat(fd, &st)) {
    std::cerr << file_path << ": ";
    perror("fstat");
    close(fd);
    return 1;
  }
  size_t file_length = st.st_size;

  size_t cached = fincore(fd, file_length);

  size_t page_size = getpagesize();
  fprintf(stdout, "FILE_NAME   : %s\n", file_path);
  fprintf(stdout, "FILE_SIZE   : %ld B\n", file_length);
  fprintf(stdout, "PAGE_SIZE   : %ld B\n", page_size);
  fprintf(stdout, "TOTAL_PAGES : %ld\n", cached / page_size);
  fprintf(stdout, "CACHE_SIZE  : %ld\n", cached);

  close(fd);
  return 0;
}
