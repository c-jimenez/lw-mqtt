####################################################################################################
#Copyright(c) 2016 Cedric Jimenez
#
#This file is part of lw-mqtt.
#
#lw-mqtt is free software: you can redistribute it and/or modify
#it under the terms of the GNU Lesser General Public License as published by
#the Free Software Foundation, either version 3 of the License, or
#(at your option) any later version.
#
#lw-mqtt is distributed in the hope that it will be useful,
#but WITHOUT ANY WARRANTY; without even the implied warranty of
#MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#GNU Lesser General Public License for more details.
#
#You should have received a copy of the GNU Lesser General Public License
#along with lw-mqtt.  If not, see <http://www.gnu.org/licenses/>.
####################################################################################################



# Locating the root directory
ROOT_DIR := ../../../..

# Project name
PROJECT_NAME := lw-mqtt-borker

# Build type
BUILD_TYPE := APP

# Projects that need to be build before the project or containing necessary include paths
PROJECT_DEPENDENCIES := 

# Librairies needed by the project
PROJECT_LIBS := libs/lw-mqtt

# Including common makefile definitions
include $(ROOT_DIR)/build/gcc/makedefs			 

# Additionnal librairies
ifeq ($(TARGET_OS), windows)
	LIBS := $(LIBS) -lws2_32
endif
ifeq ($(TARGET_OS), posix)
	LIBS := $(LIBS)
endif

# Rules for building the source files
$(BIN_DIR)/$(OUTPUT_NAME): $(OBJECT_FILES)
	@echo "Linking $(notdir $@)..."
	$(DISP)$(LD) $(LINK_OUTPUT_CMD) $@ $(LDFLAGS) $(OBJECT_FILES) $(LIBS)
	
	
	
	
