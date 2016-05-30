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



# Source directories
SOURCE_DIR := $(ROOT_DIR)/src
SOURCE_DIRS := $(SOURCE_DIR) \
               $(SOURCE_DIR)/broker \
               $(SOURCE_DIR)/client \
               $(SOURCE_DIR)/config \
               $(SOURCE_DIR)/log \
               $(SOURCE_DIR)/oal \
               $(SOURCE_DIR)/oal/$(TARGET_OS) \
               $(SOURCE_DIR)/packet \
               $(SOURCE_DIR)/socket \
               $(SOURCE_DIR)/socket/$(TARGET_OS) \
               $(SOURCE_DIR)/stream \
               $(SOURCE_DIR)/time \
               $(SOURCE_DIR)/time/$(TARGET_OS)

              
# Project specific include directories
PROJECT_INC_DIRS := $(PROJECT_INC_DIRS) \
                    $(SOURCE_DIRS)


