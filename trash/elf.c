#define ELF_NIDENT	16
#define PT_LOAD		1

#include <stdio.h>
#include <stdint.h>
#include <string.h>

struct elf_hdr {
	uint8_t e_ident[ELF_NIDENT];
	uint16_t e_type;
	uint16_t e_machine;
	uint32_t e_version;
	uint64_t e_entry;
	uint64_t e_phoff;
	uint64_t e_shoff;
	uint32_t e_flags;
	uint16_t e_ehsize;
	uint16_t e_phentsize;
	uint16_t e_phnum;
	uint16_t e_shentsize;
	uint16_t e_shnum;
	uint16_t e_shstrndx;
} __attribute__((packed));

struct elf_phdr {
	uint32_t p_type;
	uint32_t p_flags;
	uint64_t p_offset;
	uint64_t p_vaddr;
	uint64_t p_paddr;
	uint64_t p_filesz;
	uint64_t p_memsz;
	uint64_t p_align;
} __attribute__((packed));

static int readelf_hdr(const char *name, struct elf_hdr *dst);

static int readelf_hdr(const char *name, struct elf_hdr *dst)
{
    FILE *file = fopen(name, "r");
    if (!file)
        goto error;

    memset(dst, 0, sizeof(struct elf_hdr));
    if (!fread(dst, sizeof(struct elf_hdr), 1, file))
        goto error;

    if (fclose(file))
        goto error;

	return 0;
error:
    if (file)
        fclose(file);

    return -1;
}

size_t space(const char *name)
{
    int err = 0;

    struct elf_hdr header;
    if ((err = readelf_hdr(name, &header)) == -1)
        return -1;

    FILE *file = fopen(name, "r");
    if (!file)
        return -1;

    if (fseek(file, header.e_phoff, SEEK_SET)) {
        if (file)
            fclose(file);
        return -1;
    }

    struct elf_phdr entries[header.e_phnum];
    memset(entries, 0, header.e_phnum);

    struct elf_phdr hdr;
    int ret = 0;
    int n = 0;

    for (int n = 0; n < header.e_phnum; n++) {
        memset(&hdr, 0, sizeof(struct elf_phdr));

        if (!fread(&hdr, sizeof(struct elf_phdr), 1, file)) {
            fclose(file);
            return -1;
        }

        if (hdr.p_type == PT_LOAD)
            ret += hdr.p_memsz;
    }

    return ret;
}

uintptr_t entry_point(const char *name)
{
    struct elf_hdr header;
    int ret = readelf_hdr(name, &header);
    return (-1 == ret) ? ret : header.e_entry;
}

int main(int argc, char *argv[])
{
    printf("entry_point::%d\n", entry_point(argv[0]));
    printf("space::%d\n", space(argv[0]));
    return 0;
}
