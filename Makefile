# Copyright (C) 2020 Jarosław Rymut
SHELL := /bin/sh

# pliki
FILES := $(wildcard *.cpp)

####################
# flagi
CFLAGS := -O3 -march=native -pipe -Wall -Wextra -Weffc++ -Wconversion -Wpedantic -Wno-sign-compare
# PIC > PIE > no-pie
CFLAGS += -fPIC
# and for linker:
LDFLAGS += -pie
# ścieżka do bibliotek
LDLIBS += -Wl,-R.
# link time optimization
CFLAGS += -flto
# debugowanie
# CFLAGS += -ggdb3
# standardy
CXXFLAGS := -std=c++17 $(CFLAGS)
CFLAGS += -std=c11

CPPFLAGS := -MMD -MP

####################
# private jest dostępne od GNU make 3.82
ifneq (3.82,$(firstword $(sort $(MAKE_VERSION) 3.82)))
  $(error "*** PLEASE USE AT LEAST GNU MAKE 3.82 FROM 2010 ***")
endif

####################
# wszystko zależy od Makefile
$(FILES:.cpp=.o): Makefile

# make może automatycznie usunąć te pliki
.INTERMEDIATE: $(FILES:.cpp=.o)

####################
# domyślne cele
.DEFAULT_GOAL := all
.PHONY: all
all: $(FILES:.cpp=.x)

.PHONY: clean
clean: mostlyclean
	$(RM) $(FILES:.cpp=.x)

.PHONY: mostlyclean
mostlyclean:
	$(RM) $(FILES:.cpp=.d)

# kopia wbudowanej reguły, z nowym rozszerzeniem
%.x: %.o
	$(LINK.cpp) $^ $(LOADLIBES) $(LDLIBS) $(OUTPUT_OPTION)

lib%.so: private LDFLAGS += -shared
lib%.so: %.o
	$(LINK.o) $^ $(LOADLIBES) $(LDLIBS) $(OUTPUT_OPTION)

%.d: %.cpp
	$(CXX) $(CPPFLAGS:MD=M) -MF $@ $<

%: %.x
	./$^

ifneq (,$(wildcard $(FILES:.cpp=.d)))
-include $(wildcard $(FILES:.cpp=.d))
endif

# usuń domyślne reguły
%: %.o
%: %.cpp
.SUFFIXES:
.SUFFIXES: .o .cpp

####################
# inne cele
.PHONY: check
check:
	@clang-tidy \
		-checks="bugprone-*,\
	cppcoreguidelines-*,\
	clang-analyzer-*,\
	misc-*,\
	modernize-*,\
	performance-*,\
	portability-*,\
	readability-*,\
	-clang-analyzer-cplusplus.NewDeleteLeaks,\
	-misc-unused-parameters,\
	-readability-braces-around-statements,\
	-readability-named-parameter,\
	-readability-implicit-bool-conversion,\
	-cppcoreguidelines-no-malloc,\
	-cppcoreguidelines-owning-memory,\
	-cppcoreguidelines-pro-bounds-*"\
		-header-filter='.*,-rapidcheck.hpp' \
		-config="{CheckOptions: [ \
			{ key: readability-identifier-naming.NamespaceCase, value: CamelCase }, \
			{ key: readability-identifier-naming.ClassCase, value: CamelCase  }, \
			{ key: readability-identifier-naming.StructCase, value: CamelCase  }, \
			{ key: readability-identifier-naming.FunctionCase, value: lower_case }, \
			{ key: readability-identifier-naming.VariableCase, value: lower_case }, \
			{ key: readability-identifier-naming.GlobalConstantCase, value: UPPER_CASE } \
		]}" \
		$(FILES) \
		-- $(filter-out -flto,$(CXXFLAGS))
# lto does something bad to clang-tidy

.PHONY: format
format:
	@clang-format -i -style=file $(FILES) $(HELPERS)

####################
# testy

.PHONY: test
test:
	@printf "\033"'[32m   /`"'\''-,__'"\n"
	@printf '   \/\)`   `'\''-.'"\n"
	@printf '  // \ .--.\   '\''.'"\n"
	@printf ' //|-.  \_o `-.  \---._'"\n"
	@printf ' || \_o\  _.-.\`'\''-'\''    `-.'"\n"
	@printf ' || |\.-'\''`    |           `.'"\n"
	@printf ' || | \  \    |             `\\'"\n"
	@printf ' \| /  \ ,\'\'' /                \\'"\n"
	@printf '  `'\''    `---'\''                  ;'"\n"
	@printf '         `))          .-'\''      |'"\n"
	@printf '      .-.// .-.     .'\''        ,;=D'"\n"
	@printf '     /  // /   \  .'\''          ||'"\n"
	@printf '    |..-'\'' |     '\''-'\''          //'"\n"
	@printf '    ((    \         .===. _,//                    No tests yet!'"\n"
	@printf '     '\''`'\''--`'\''---'\'''\'''\'',--\_/-;-'\''`'"\n"
	@printf 'jgs                `~/^\`'"\n"
	@printf '                    '\''==='\'"\033"'[0m'"\n"

