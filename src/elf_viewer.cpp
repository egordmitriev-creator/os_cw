#include "elf_viewer.h"
#include <iostream>
#include <fstream>
#include <mach-o/loader.h>
#include <mach-o/fat.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sstream>  // Для std::ostringstream
#include <iomanip>  // Для форматирования вывода

// Объявление функции dump_content
std::string dump_content(const char* data, size_t size);

std::string parse_elf_file(const std::string& file_path) {
    std::string result;

    // Открытие файла
    int fd = open(file_path.c_str(), O_RDONLY);
    if (fd == -1) {
        return "Failed to open file.";
    }

    // Получение информации о файле
    struct stat statbuf;
    if (fstat(fd, &statbuf) == -1) {
        close(fd);
        return "Failed to stat file.";
    }

    size_t file_size = statbuf.st_size;

    // Отображение файла в память
    void* file_map = mmap(NULL, file_size, PROT_READ, MAP_PRIVATE, fd, 0);
    if (file_map == MAP_FAILED) {
        close(fd);
        return "Failed to map file into memory.";
    }

    // Проверка заголовка для определения fat binary
    struct fat_header* fat_header = (struct fat_header*)file_map;

    // Конвертация из big-endian в host-endian
    uint32_t magic = ntohl(fat_header->magic);
    if (magic == FAT_MAGIC || magic == FAT_MAGIC_64) {
        result += "Fat binary detected\n";

        // Получение количества архитектур
        uint32_t nfat_arch = ntohl(fat_header->nfat_arch);
        struct fat_arch* archs = (struct fat_arch*)((char*)file_map + sizeof(struct fat_header));

        for (uint32_t i = 0; i < nfat_arch; i++) {
            uint32_t offset = ntohl(archs[i].offset);

            // Считываем заголовок Mach-O из этого смещения
            struct mach_header_64* header = (struct mach_header_64*)((char*)file_map + offset);
            if (header->magic == MH_MAGIC_64) {
                result += "  Mach-O 64-bit architecture detected at offset " + std::to_string(offset) + "\n";
            } else if (header->magic == MH_MAGIC) {
                result += "  Mach-O 32-bit architecture detected at offset " + std::to_string(offset) + "\n";
            } else {
                result += "  Unknown architecture at offset " + std::to_string(offset) + "\n";
            }

            // Вывод содержимого соответствующего сегмента
            result += "  Content dump:\n";
            result += dump_content((char*)file_map + offset, std::min<size_t>(512, file_size - offset));
        }
    } else if (magic == MH_MAGIC_64) {
        result += "Mach-O 64-bit binary detected\n";
        result += "Content dump:\n";
        result += dump_content((char*)file_map, std::min<size_t>(512, file_size));
    } else if (magic == MH_MAGIC) {
        result += "Mach-O 32-bit binary detected\n";
        result += "Content dump:\n";
        result += dump_content((char*)file_map, std::min<size_t>(512, file_size));
    } else {
        result += "Unknown Mach-O format\n";
        result += "Magic number: 0x" + std::to_string(magic) + "\n";
        result += "Content dump:\n";
        result += dump_content((char*)file_map, std::min<size_t>(512, file_size));
    }

    // Завершение работы с файлом
    munmap(file_map, file_size);
    close(fd);

    return result;
}

// Реализация функции dump_content
std::string dump_content(const char* data, size_t size) {
    std::ostringstream oss;
    oss << std::hex << std::setfill('0'); // Установка формата для шестнадцатеричного вывода

    for (size_t i = 0; i < size; i++) {
        if (i % 16 == 0) {
            oss << "\n  " << std::setw(8) << i << ": "; // Адрес начала строки
        }
        oss << std::setw(2) << (static_cast<unsigned char>(data[i])) << " ";
    }

    oss << "\n";
    return oss.str();
}