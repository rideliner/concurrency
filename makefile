INCLUDEDIR = include
LIBDIR = lib
BUILDDIR = build
SRCDIR = src

CXX = clang++
CXXFLAGS = -pthread -std=c++14 -I$(INCLUDEDIR) -Wall
LINKFLAGS = -L$(LIBDIR) -lconcurrency
OUTPUT_OPTION = -o $@

TESTDIR = test
TESTBUILDDIR = $(BUILDDIR)/$(TESTDIR)

DIRS = $(LIBDIR) $(BUILDDIR) $(TESTBUILDDIR)

LIB_OUT = $(LIBDIR)/libconcurrency.a
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

define dependency-flags
-MT $@ -MMD -MP -MF $1/$*.dTemp
endef

define compile
	$(CXX) $(call dependency-flags,$1) $(CXXFLAGS) -c $(OUTPUT_OPTION) $<
	@-mv -f $1/$*.dTemp $1/$*.d
endef

define link
	$(CXX) $(CXXFLAGS) $(LINKFLAGS) $(OUTPUT_OPTION)
endef

define static-build
	ar crf $@ $^
endef

default: all

$(BUILDDIR)/%.o : $(SRCDIR)/%.cpp $(BUILDDIR)/%.d | $(BUILDDIR)
	$(call compile,$(BUILDDIR))

$(LIB_OUT): $(OBJ_FILES) | $(LIBDIR)
	$(call static-build)

$(TEST_OBJ_FILES): $(TESTBUILDDIR)/%.o: $(TESTDIR)/%.cpp $(TESTBUILDDIR)/%.d | $(TESTBUILDDIR)
	$(call compile,$(TESTBUILDDIR))

$(TEST_OUT): $(TEST_MAIN) $(TEST_OBJ_FILES) | $(TESTDIR) $(LIB_OUT)
	$(call link) -lgtest $^

$(BUILDDIR)/%.d: | $(BUILDDIR) ;
$(TESTBUILDDIR)/%.d: | $(TESTBUILDDIR) ;
.PRECIOUS: $(BUILDDIR)/%.d $(TESTBUILDDIR)/%.d

-include $(DEP_FILES) $(TEST_DEP_FILES)

clean:
	-rm -rf $(DIRS)

$(DIRS):
	@-mkdir -p $@

all: $(LIB_OUT) $(TEST_OUT)

.PHONY: all clean default
