import os
import glob
import re
import fileinput
this_dir=os.path.dirname(os.path.realpath(__file__))
top_dir=os.path.abspath(os.path.join(this_dir,'..','LibTaskForce'))

CurrentCLicense=r'^\/\*\s*\*\s*LibTaskForce: An open-source library for '\
                r'task-based parallelism\s*\*\s*\*\s*Copyright \(C\) 2016 '\
                r'Ryan M. Richard\s*\*\s*\*\s*This file is part of '\
                r'LibTaskForce.\s*\*\s*\*\s*LibTaskForce is free software: '\
                r'you can redistribute it and\/or modify\s*\*\s*it under the '\
                r'terms of the GNU General Public License as published '\
                r'by\s*\*\s*the Free Software Foundation, either version 3 of '\
                r'the License, or\s*\*\s*\(at your option\) any later '\
                r'version.\s*\*\s*\*\s*LibTaskForce is distributed in the hope'\
                r' that it will be useful,\s*\*\s*but WITHOUT ANY WARRANTY; '\
                r'without even the implied warranty of\s*\*\s*MERCHANTABILITY '\
                r'or FITNESS FOR A PARTICULAR PURPOSE.  See the\s*\*\s*GNU '\
                r'General Public License for more details.\s*\*\s*\*\s*You '\
                r'should have received a copy of the GNU General Public '\
                r'License\s*\*\s*along with LibTaskForce.  If not, '\
                r'see \<http:\/\/www.gnu.org\/licenses\/\>.\s*\*\/'

NewCLicense="""
/*  
 *   LibTaskForce: An open-source library for task-based parallelism
 * 
 *   Copyright (C) 2016 Ryan M. Richard
 * 
 *   This file is part of LibTaskForce.
 *
 *   LibTaskForce is free software: you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation, either version 3 of the License, or
 *   (at your option) any later version.
 *
 *   LibTaskForce is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with LibTaskForce.  If not, see <http://www.gnu.org/licenses/>.
 */ 
"""

header_files=glob.glob(os.path.join(top_dir,'**','*.hpp'),recursive=True)
source_files=glob.glob(os.path.join(top_dir,'**','*.cpp'),recursive=True)
for fname in [header_files,source_files]:
    f=open(fname,'r')
    filedata=f.read()
    f.close()
    newfile=re.sub(CurrentCLicense,NewCLicense,filedata)
    f=open(fname,'w')
    f.write(newfile)
    f.close()