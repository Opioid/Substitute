SOURCES += Image/Colorspace.cpp \
	Image/Image.cpp \
	Image/Image_buffer.cpp \
	Image/Image_provider.cpp \
	Image/Mip_map_generator.cpp \
	Image/NVTT_output_handler.cpp \
	Image/Thumbnail_provider.cpp \

HEADERS += Image/Colorspace.hpp \
	Image/Image.hpp \
	Image/Image_buffer.hpp \
	Image/Image_provider.hpp \
	Image/Mip_map_generator.hpp \
	Image/NVTT_output_handler.hpp \
	Image/Thumbnail_provider.hpp \

include(Filter/Filter.pri)
include(Sampler/Sampler.pri)
