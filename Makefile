CXX = g++
CXXFLAGS = -Wall -Wextra -std=c++17 -O2 -IHead
SRCS = main.cpp $(wildcard Source/*.cpp)
OBJDIR = obj
OBJS = $(patsubst %.cpp,$(OBJDIR)/%.o,$(notdir $(SRCS)))
TARGET = his.exe

.PHONY: all debug clean run

all: $(OBJDIR) $(TARGET)

ifeq ($(OS),Windows_NT)
$(OBJDIR):
	@if not exist $(OBJDIR) mkdir $(OBJDIR)
else
$(OBJDIR):
	mkdir -p $(OBJDIR)
endif

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^

$(OBJDIR)/main.o: main.cpp | $(OBJDIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(OBJDIR)/%.o: Source/%.cpp | $(OBJDIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

debug: CXXFLAGS += -g -O0
debug: clean all

ifeq ($(OS),Windows_NT)
clean:
	-@if exist $(OBJDIR) rmdir /S /Q $(OBJDIR)
	-@if exist $(TARGET) del /Q $(TARGET)
else
clean:
	rm -rf $(OBJDIR) $(TARGET)
endif

run: all
	./$(TARGET)