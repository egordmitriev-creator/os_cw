#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mach-o/loader.h>
#include <mach-o/nlist.h>

// Функция для записи заголовка Mach-O
void write_header(FILE *file) {
    struct mach_header_64 header;
    memset(&header, 0, sizeof(header));

    header.magic = MH_MAGIC_64;              // Магическое число
    header.cputype = CPU_TYPE_X86_64;        // Тип процессора (x86_64)
    header.cpusubtype = CPU_SUBTYPE_X86_64_ALL;
    header.filetype = MH_EXECUTE;            // Тип файла: исполняемый
    header.ncmds = 1;                        // Количество команд загрузки
    header.sizeofcmds = sizeof(struct segment_command_64);
    header.flags = 0;                        // Флаги

    fwrite(&header, sizeof(header), 1, file);
}

// Функция для записи Load Command (сегмента __TEXT)
void write_load_command(FILE *file) {
    struct segment_command_64 segment;
    memset(&segment, 0, sizeof(segment));

    strncpy(segment.segname, SEG_TEXT, 16);  // Имя сегмента: __TEXT
    segment.cmd = LC_SEGMENT_64;             // Команда загрузки
    segment.cmdsize = sizeof(segment);
    segment.vmaddr = 0x1000;                 // Виртуальный адрес
    segment.vmsize = 0x1000;                 // Размер в памяти
    segment.fileoff = 0;                     // Смещение в файле
    segment.filesize = 0x1000;               // Размер в файле
    segment.maxprot = VM_PROT_READ | VM_PROT_EXECUTE; // Максимальные права
    segment.initprot = VM_PROT_READ | VM_PROT_EXECUTE; // Начальные права

    fwrite(&segment, sizeof(segment), 1, file);
}

// Функция для записи машинного кода в секцию .text
void write_text_section(FILE *file) {
    // x86_64 машинный код для 2^5:
    const unsigned char code[] = {
        0x48, 0xC7, 0xC0, 0x02, 0x00, 0x00, 0x00, // mov rax, 2      (rax = 2)
        0x48, 0xC7, 0xC1, 0x05, 0x00, 0x00, 0x00, // mov rcx, 5      (rcx = 5)
        0x48, 0x89, 0xD1,                         // mov rdx, rcx    (rdx = rcx)
        0x48, 0xFF, 0xC1,                         // loop: inc rcx   (увеличиваем rcx)
        0x48, 0x0F, 0xAF, 0xC0,                   // imul rax, rax   (rax *= 2)
        0x48, 0xFF, 0xCA,                         // dec rdx         (уменьшаем rdx)
        0x75, 0xF4,                               // jne loop        (если rdx != 0, перейти в loop)
        0x48, 0x89, 0xC7,                         // mov rdi, rax    (поместить результат в rdi)
        0xB8, 0x02, 0x00, 0x00, 0x02,             // mov eax, 0x2000002 (syscall write)
        0x48, 0x31, 0xD2,                         // xor rdx, rdx    (обнулить rdx)
        0x48, 0xC7, 0xC6, 0x01, 0x00, 0x00, 0x00, // mov rsi, 1      (stdout)
        0x0F, 0x05,                               // syscall         (выполнить syscall)
        0xC3                                      // ret             (возврат из функции)
    };

    fwrite(code, sizeof(code), 1, file);
}

int main() {
    FILE *file = fopen("simple_macho", "wb");
    if (!file) {
        perror("Failed to create file");
        return 1;
    }

    // 1. Записываем заголовок Mach-O
    write_header(file);

    // 2. Записываем команду загрузки
    write_load_command(file);

    // 3. Записываем машинный код
    write_text_section(file);

    fclose(file);

    // Делаем файл исполняемым
    printf("Mach-O file created as 'simple_macho'\n");
    return 0;
}