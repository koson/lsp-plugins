FILES                   = $(addprefix $(OBJDIR)/, $(patsubst %.cpp, %.o, $(wildcard *.cpp)))
FILE                    = $(@:$(OBJDIR)/%.o=%.cpp)
INCLUDE                += $(XLIB_HEADERS) $(CAIRO_HEADERS)

.PHONY: all

all: $(OBJ_WS_X11_CORE)

$(OBJ_WS_X11_CORE): $(FILES)
	@echo "  $(LD) $(notdir $(@))"
	@$(LD) -o $(OBJ_WS_X11_CORE) $(LDFLAGS) -r $(FILES)

$(FILES):
	@echo "  $(CC) $(FILE)"
	@$(CC) -c $(CPPFLAGS) $(CFLAGS) $(INCLUDE) $(FILE) -o $(@)

