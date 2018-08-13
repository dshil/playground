#define ELF_NIDENT	16

#include <cstdint>
#include <stdio.h>
#include <string.h>

struct elf_hdr {
	std::uint8_t e_ident[ELF_NIDENT];
	std::uint16_t e_type;
	std::uint16_t e_machine;
	std::uint32_t e_version;
	std::uint64_t e_entry;
	std::uint64_t e_phoff;
	std::uint64_t e_shoff;
	std::uint32_t e_flags;
	std::uint16_t e_ehsize;
	std::uint16_t e_phentsize;
	std::uint16_t e_phnum;
	std::uint16_t e_shentsize;
	std::uint16_t e_shnum;
	std::uint16_t e_shstrndx;
} __attribute__((packed));

std::uintptr_t entry_point(const char *name)
{
    FILE *file = fopen(name, "r");
    if (!file)
        goto error;

    struct elf_hdr header;
    memset(&header, 0, sizeof(elf_hdr));

    if (!fread(&header, sizeof(elf_hdr), 1, file))
        goto error;

    if (fclose(file))
        goto error;

    return header.e_entry;
error:
    if (file)
        fclose(file);

	return 0;
}

int main(int argc, char *argv[])
{
    printf("entry_point::%d\n", entry_point(argv[0]));
    return 0;
}
