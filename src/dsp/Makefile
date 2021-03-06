NATIVE_IMPL             = $(OBJDIR)/native.o

X86_IMPL                = $(OBJDIR)/x86.o
SSE_IMPL                = $(OBJDIR)/sse.o
SSE3_IMPL               = $(OBJDIR)/sse3.o
SSE4_IMPL               = $(OBJDIR)/sse4.o
AVX_IMPL                = $(OBJDIR)/avx.o
NEON_D32_IMPL           = $(OBJDIR)/neon-d32.o
DSP_IMPL                = $(OBJDIR)/dsp.o
BITS_IMPL               = $(OBJDIR)/bits.o
ARM_IMPL                = $(OBJDIR)/arm.o

SSE_INSTR_SET           = -mmmx -m3dnow -msse -msse2
SSE3_INSTR_SET          = $(SSE_INSTR_SET) -msse3 -mssse3
SSE4_INSTR_SET          = $(SSE_INSTR_SET) -msse4.1 -msse4.2
AVX_INSTR_SET           = -mavx
NEON_D32_INSTR_SET      = -mfpu=neon-vfpv4

FILE                    = $(@:$(OBJDIR)/%.o=%.cpp)
LINK_OBJECTS            = $(DSP_IMPL) $(BITS_IMPL) $(NATIVE_IMPL)

# Build different DSP modules dependent on architecture
ifeq ($(BUILD_PROFILE), i586)
LINK_OBJECTS           += $(X86_IMPL) $(SSE_IMPL) $(SSE3_IMPL) $(SSE4_IMPL) $(AVX_IMPL)
endif
ifeq ($(BUILD_PROFILE), x86_64)
LINK_OBJECTS           += $(X86_IMPL) $(SSE_IMPL) $(SSE3_IMPL) $(SSE4_IMPL) $(AVX_IMPL)
endif
ifeq ($(BUILD_PROFILE), armv7a)
LINK_OBJECTS           += $(ARM_IMPL) $(NEON_D32_IMPL)
endif

.PHONY: all target

target: all

all: $(OBJ_DSP)

$(OBJ_DSP): $(LINK_OBJECTS)
	@echo "  $(LD) $(notdir $(OBJ_DSP))"
	@$(LD) -r $(LDFLAGS) -o $(OBJ_DSP) $(LINK_OBJECTS)

$(DSP_IMPL) $(BITS_IMPL) $(NATIVE_IMPL) $(X86_IMPL) $(ARM_IMPL):
	@echo "  $(CC) $(FILE)"
	@$(CC) -c $(CPPFLAGS) $(CFLAGS) $(INCLUDE) $(FILE) -o $(@)
	
$(SSE_IMPL):
	@echo "  $(CC) $(FILE)"
	@$(CC) -c $(CPPFLAGS) $(CFLAGS) $(SSE_INSTR_SET) $(INCLUDE) $(FILE) -o $(@)
	
$(SSE3_IMPL):
	@echo "  $(CC) $(FILE)"
	@$(CC) -c $(CPPFLAGS) $(CFLAGS) $(SSE3_INSTR_SET) $(INCLUDE) $(FILE) -o $(@)
	
$(SSE4_IMPL):
	@echo "  $(CC) $(FILE)"
	@$(CC) -c $(CPPFLAGS) $(CFLAGS) $(SSE4_INSTR_SET) $(INCLUDE) $(FILE) -o $(@)

$(AVX_IMPL):
	@echo "  $(CC) $(FILE)"
	@$(CC) -c $(CPPFLAGS) $(CFLAGS) $(AVX_INSTR_SET) $(INCLUDE) $(FILE) -o $(@)
	
$(NEON_D32_IMPL):
	@echo "  $(CC) $(FILE)"
	@$(CC) -c $(CPPFLAGS) $(CFLAGS) $(NEON_D32_INSTR_SET) $(INCLUDE) $(FILE) -o $(@)
