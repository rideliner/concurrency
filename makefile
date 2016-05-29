INCLUDEDIR = include
LIBDIR = lib
BUILDDIR = build
SRCDIR = src

CXX = clang++
CXXFLAGS = -pthread -std=c++14 -I$(INCLUDEDIR) -fPIC -Wall
LINKFLAGS = -L$(LIBDIR) -lconcurrency
OUTPUT_OPTION = -o $@

TESTDIR = test
TESTBUILDDIR = $(BUILDDIR)/$(TESTDIR)
EXAMPLESDIR = examples
EXAMPLESBUILDDIR = $(BUILDDIR)/$(EXAMPLESDIR)

DIRS = $(LIBDIR) $(BUILDDIR) $(TESTBUILDDIR) $(EXAMPLESBUILDDIR)

LIB_OUT = $(LIBDIR)/libconcurrency.so
TEST_OUT = $(BUILDDIR)/run_tests
TEST_MAIN = $(TESTDIR)/main.cpp

SRC_FILES = $(wildcard $(SRCDIR)/*.cpp)
SRC_BASE_FILENAMES = $(basename $(SRC_FILES))
OBJ_FILES = $(patsubst $(SRCDIR)/%,$(BUILDDIR)/%.o,$(SRC_BASE_FILENAMES))
DEP_FILES = $(patsubst $(SRCDIR)/%,$(BUILDDIR)/%.d,$(SRC_BASE_FILENAMES))

TEST_FILES = $(filter-out $(TEST_MAIN),$(wildcard $(TESTDIR)/*.cpp))
TEST_BASE_FILENAMES = $(basename $(TEST_FILES))
TEST_OBJ_FILES = $(patsubst $(TESTDIR)/%,$(TESTBUILDDIR)/%.o,$(TEST_BASE_FILENAMES))
TEST_DEP_FILES = $(patsubst $(TESTDIR)/%,$(TESTBUILDDIR)/%.d,$(TEST_BASE_FILENAMES))

EXAMPLE_FILES = $(wildcard $(EXAMPLESDIR)/*.cpp)
EXAMPLE_BASE_FILENAMES = $(basename $(EXAMPLE_FILES))
EXAMPLE_OBJ_FILES = $(patsubst $(EXAMPLESDIR)/%,$(EXAMPLESBUILDDIR)/%.o,$(EXAMPLE_BASE_FILENAMES))
EXAMPLE_DEP_FILES = $(patsubst $(EXAMPLESDIR)/%,$(EXAMPLESBUILDDIR)/%.d,$(EXAMPLE_BASE_FILENAMES))
EXAMPLE_OUT_FILES = $(patsubst $(EXAMPLESDIR)/%,$(EXAMPLESBUILDDIR)/%,$(EXAMPLE_BASE_FILENAMES))

define dependency-flags
-MT $@ -MMD -MP -MF $1/$*.dTemp
endef

define compile
	$(CXX) $(call dependency-flags,$1) $(CXXFLAGS) -c $(OUTPUT_OPTION) $<
	@-mv -f $1/$*.dTemp $1/$*.d
endef

LINK = $(CXX) $(CXXFLAGS) $^ $(OUTPUT_OPTION) $(LINKFLAGS)
LIBRARY = $(CXX) -fPIC -shared $^ $(OUTPUT_OPTION)

default: all

$(BUILDDIR)/%.o : $(SRCDIR)/%.cpp | $(BUILDDIR)/%.d
	$(call compile,$(BUILDDIR))

$(LIB_OUT): $(OBJ_FILES) | $(LIBDIR)
	$(LIBRARY)

$(TEST_OBJ_FILES): $(TESTBUILDDIR)/%.o: $(TESTDIR)/%.cpp | $(TESTBUILDDIR)/%.d
	$(call compile,$(TESTBUILDDIR))

$(TEST_OUT): $(TEST_MAIN) $(TEST_OBJ_FILES) | $(LIB_OUT)
	$(LINK) -lgtest

$(EXAMPLE_OBJ_FILES): $(EXAMPLESBUILDDIR)/%.o: $(EXAMPLESDIR)/%.cpp | $(EXAMPLESBUILDDIR)/%.d
	$(call compile,$(EXAMPLESBUILDDIR))

$(EXAMPLE_OUT_FILES): $(EXAMPLESBUILDDIR)/%: $(EXAMPLESBUILDDIR)/%.o | $(LIB_OUT)
	$(LINK)

$(BUILDDIR)/%.d: | $(BUILDDIR) ;
$(TESTBUILDDIR)/%.d: | $(TESTBUILDDIR) ;
$(EXAMPLESBUILDDIR)/%.d: | $(EXAMPLESBUILDDIR) ;
.PRECIOUS: $(BUILDDIR)/%.d $(TESTBUILDDIR)/%.d $(EXAMPLESBUILDDIR)/%.d

-include $(DEP_FILES) $(TEST_DEP_FILES) $(EXAMPLE_DEP_FILES)

all: $(LIB_OUT) tests

examples: $(EXAMPLE_OUT_FILES)

tests: $(TEST_OUT)
	$(TEST_OUT)

clean:
	-rm -rf $(DIRS)

$(DIRS):
	@-mkdir -p $@

.PHONY: all clean default examples tests
