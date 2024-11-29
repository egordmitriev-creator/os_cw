# Компилятор и флаги
CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -O2 -I/opt/homebrew/opt/boost/include -I/opt/homebrew/include
LDFLAGS = -L/opt/homebrew/opt/boost/lib -lboost_system

# Исходные файлы
SRC_DIR = src
BUILD_DIR = build
SRCS = $(wildcard $(SRC_DIR)/*.cpp)
OBJS = $(SRCS:$(SRC_DIR)/%.cpp=$(BUILD_DIR)/%.o)

# Исполняемый файл
TARGET = $(BUILD_DIR)/network_task_service

# Правило по умолчанию
all: $(TARGET)

# Правило сборки исполняемого файла
$(TARGET): $(OBJS)
	@mkdir -p $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LDFLAGS)

# Правило сборки объектных файлов
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

run: 
	cd $(BUILD_DIR) && ./network_task_service
# Очистка
clean:
	rm -rf $(BUILD_DIR)

.PHONY: all clean
