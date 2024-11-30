# Компилятор и флаги
CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -O2 -I/opt/homebrew/opt/boost/include -I/opt/homebrew/include
LDFLAGS = -L/opt/homebrew/opt/boost/lib -lboost_system

# Директории и файлы
SRC_DIR = src
BUILD_DIR = build
CLIENT_SRC = $(SRC_DIR)/client.cpp
CLIENT_OBJ = $(BUILD_DIR)/client.o
CLIENT_TARGET = $(BUILD_DIR)/client

# Правило по умолчанию
all: $(CLIENT_TARGET)

# Сборка клиентского исполняемого файла
$(CLIENT_TARGET): $(CLIENT_OBJ)
	@mkdir -p $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LDFLAGS)

# Сборка объектного файла для клиента
$(BUILD_DIR)/client.o: $(CLIENT_SRC)
	@mkdir -p $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Правило для запуска клиента
run: $(CLIENT_TARGET)
	cd $(BUILD_DIR) && ./client

# Очистка
clean:
	rm -rf $(BUILD_DIR)

.PHONY: all clean run
