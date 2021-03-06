##############################################################################
#copyright 2012, Hamid MEDJAHED & Elyes ZEKRI (hmedjahed@prologue.fr)        #
# Prologue                                                                   # 
#Licensed under the Apache License, Version 2.0 (the "License");             #
#you may not use this file except in compliance with the License.            #
#You may obtain a copy of the License at                                     #
#                                                                            #
#       http://www.apache.org/licenses/LICENSE-2.0                           #
#                                                                            #
#Unless required by applicable law or agreed to in writing, software         #
#distributed under the License is distributed on an "AS IS" BASIS,           #
#WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.    #
#See the License for the specific language governing permissions and         #
#limitations under the License.                                              #
##############################################################################
#!/usr/bin/env python
# -*- coding: latin-1 -*-

# Implementation of category CRUD functions
import sys
import pycompdev
import pypacksrc
srcdirectory=pypacksrc.srcpydir+"/pyaccords/pygen/"
sys.path.append(srcdirectory)
from amazonEc2Class import *
""" Note: amazonEc2 is a python class to interface the accords category :amazonEc2.
	-Attributes of this category are members of this class.
	-List of attributes:

		- name 
		- flavor 
		- image 
		- original 
		- profile 
		- node 
		- price 
		- account 
		- number 
		- rootpass 
		- reference 
		- network 
		- access 
		- accessip 
		- floating 
		- floatingid 
		- publicaddr 
		- privateaddr 
		- firewall 
		- group 
		- zone 
		- hostname 
		- workload 
		- when 
		- state
		- keyfile
		- agentstatus 
"""

def amazonEc2_create(amazonEc2):
	"""Implement here your function"""
	return amazonEc2

def amazonEc2_retrieve(amazonEc2):
	"""Implement here your function"""

	return amazonEc2

def amazonEc2_update(amazonEc2):
	"""Implement here your function"""

	return amazonEc2

def amazonEc2_delete(amazonEc2):
	"""Implement here your function"""

	return amazonEc2

