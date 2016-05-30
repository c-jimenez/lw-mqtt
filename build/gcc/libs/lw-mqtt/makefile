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
PROJECT_NAME := liblw-mqtt

# Build type
BUILD_TYPE := LIB

# Projects that need to be build before the project or containing necessary include paths
PROJECT_DEPENDENCIES := 

# Including common makefile definitions
include $(ROOT_DIR)/build/gcc/makedefs



# Rules for building the source files
$(BIN_DIR)/$(OUTPUT_NAME): $(OBJECT_FILES)
	@echo "Linking $(notdir $@)..."
	$(DISP)$(AR) $(ARFLAGS) $@ $(OBJECT_FILES)

	

