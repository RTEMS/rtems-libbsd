#!/usr/bin/env python
# -*- coding: utf-8 -*-

# Copyright (c) 2016 embedded brains GmbH.  All rights reserved.
# 
#  embedded brains GmbH
#  Dornierstr. 4
#  82178 Puchheim
#  Germany
#  <rtems@embedded-brains.de>
# 
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions
# are met:
# 1. Redistributions of source code must retain the above copyright
#    notice, this list of conditions and the following disclaimer.
# 2. Redistributions in binary form must reproduce the above copyright
#    notice, this list of conditions and the following disclaimer in the
#    documentation and/or other materials provided with the distribution.
# 
# THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
# ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
# IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
# ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
# FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
# DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
# OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
# HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
# LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
# OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
# SUCH DAMAGE.

from __future__ import print_function, division
import argparse
import sys
from elftools.elf.elffile import ELFFile
import re
import copy
import os

VERBOSE_SOME = 1
VERBOSE_MORE = 2
VERBOSE_MOST = 3

class Error(Exception):
    """Base class for exceptions in this module."""
    pass


class NoDwarfInfoError(Error):
    """Exception raised in case there is no DWARF information."""

    def __init__(self):
        super(NoDwarfInfoError, self).__init__("Input file has no DWARF info.")


class TypenameNotFoundError(Error):
    """Exception raised in case a die is not found like expected."""
    pass


class AnonymousStructureError(Error):
    """Exception raised in case a die is not found like expected."""
    pass


class VarnameNotFoundError(Error):
    """Exception raised in case a die is not found like expected."""

    def __init__(self):
        super(VarnameNotFoundError, self).__init__("Couldn't find the variables name.")


class HeaderGenCU:
    """Process a single CU"""

    def __init__(self, cu, progname, lineprog, err = sys.stderr, verbose = 0,
                 filterre = re.compile('.*')):
        self._rtems_port_names = []
        self._rtems_port_names.append("_Linker_set_bsd_prog_%s_begin" % progname)
        self._rtems_port_names.append("_Linker_set_bsd_prog_%s_end" % progname)
        self._rtems_port_names.append("rtems_bsd_command_%s" % progname)

        self._filter_special_vars = []
        # Take some special care for some yacc variables. This matches the yyval
        # and yylval. These two always make trouble in the generated headers.
        # yyval is initialized by Yacc generated code so it's not
        # necessary to move them into the copy back region. yylval is used only
        # for transporting a value. It will be set when used.
        self._filter_special_vars.append({
            "re":re.compile('extern YYSTYPE .*val'),
            "reason":"Lex / Yacc variable initialized by generated code",
            "action":"no_section"
            })
        # Lex generates an external variable that shouldn't be extern. Move it
        # to the current data header file.
        self._filter_special_vars.append({
            "re":re.compile('extern yy_size_t .*len'),
            "reason":"Lex adds an extern to this variable that is not necessary.",
            "action":"ignore_extern"
            })

        self._err = err
        self._verbose = verbose
        self._cu = cu
        self._progname = progname
        self._die_by_offset = {}
        self._lineprogram = lineprog
        self._filterre = filterre
        self._namespace_prefix = "_bsd_%s_" % (self._progname)

        self._fill_die_list()

        if self._verbose >= VERBOSE_MOST:
            print('DIE list: \n', self._die_by_offset)

    def _fill_die_list(self, die = None):
        if die is None:
            die = self._cu.get_top_DIE()
        # Use relative indices for the keys like they are used to reference
        # inside one cu
        offset = die.offset - self._cu.cu_offset
        self._die_by_offset[offset] = die
        for child in die.iter_children():
            self._fill_die_list(child)

    def _die_is_var(self, die):
        return (die.tag == "DW_TAG_variable")

    def _die_is_function(self, die):
        return (die.tag == "DW_TAG_subprogram")

    def _get_type(self, die, first_array = True):
        """Get the type of a variable DIE.
        Returns two strings: one prefix and one postfix for the variable name"""
        typepre = ""
        typepost = ""

        if self._verbose >= VERBOSE_MOST:
            self._err.write('Search type for DIE with offset=%d\n' % \
                            (die.offset))

        try:
            typedie_offset = die.attributes["DW_AT_type"].value
        except KeyError:
            raise TypenameNotFoundError('Couldn\'t find the offset of the type DIE\n')

        try:
            typedie = self._die_by_offset[typedie_offset]
        except KeyError:
            raise TypenameNotFoundError('Couldn\'t find the DIE at offset %d\n' % \
                                        (typedie_offset))

        last = False
        if (typedie.tag == "DW_TAG_const_type"):
            typepre += "const "

        elif (typedie.tag == "DW_TAG_array_type"):
            for child in typedie.iter_children():
                if child.tag == "DW_TAG_subrange_type":
                    if first_array == True:
                        arraysize = ""
                        first_array = False
                    else:
                        try:
                            upper_bound = child.attributes["DW_AT_upper_bound"].value
                            arraysize = "%d" % (upper_bound + 1)
                        except KeyError:
                            arraysize = ""
                    typepost += "[%s]" % arraysize

        elif (typedie.tag == "DW_TAG_volatile_type"):
            typepre += "volatile "

        elif (typedie.tag == "DW_TAG_pointer_type"):
            typepre += "*"

        elif (typedie.tag == "DW_TAG_structure_type"):
            typepre += "struct "

        elif (typedie.tag == "DW_TAG_enumeration_type"):
            typepre += "enum "

        elif (typedie.tag == "DW_TAG_subroutine_type"):
            typepre = "("
            typepost = ")("
            current_child = 0
            for child in typedie.iter_children():
                pre, post = self._get_type(child)
                if (current_child > 0):
                    typepost += ", "
                typepost += pre + post
                current_child += 1
            if current_child == 0:
                typepost += "void"
            typepost += ")"
            if not "DW_AT_type" in typedie.attributes.keys():
                typepre = "void " + typepre
                last = True

        elif (typedie.tag == "DW_TAG_typedef") or \
             (typedie.tag == "DW_TAG_base_type"):
            # nothing to do here than prevent the error
            pass

        else:
            raise TypenameNotFoundError('Unknown tag: %s\n' % (typedie.tag))

        if (typedie.tag == "DW_TAG_typedef") or \
           (typedie.tag == "DW_TAG_base_type") or \
           (typedie.tag == "DW_TAG_structure_type") or \
           (typedie.tag == "DW_TAG_enumeration_type"):
            last = True
            try:
                typepre += "%s " % \
                          typedie.attributes["DW_AT_name"].value.decode('ascii')
            except KeyError:
                if typedie.has_children:
                    message = 'Found an anonymous structure'
                    raise AnonymousStructureError(message)
                else:
                    message = 'Couldn\'t get type name from DIE'
                    raise TypenameNotFoundError(message)

        if last == False:
            addpre, addpost = self._get_type(typedie, first_array)
            typepre = addpre + typepre
            typepost = typepost + addpost

        if self._verbose >= VERBOSE_MOST:
            self._err.write('Add prefix="%s", postfix="%s" for DIE with offset=%d\n' % \
                            (typepre, typepost, die.offset))

        return typepre, typepost

    def generate_header(self, data_out_filename, glob_data_out, namesp_out):
        """Find all top level (global) variables in the ELF file and generate
        output that can be written in a header.
        """

        top_die = self._cu.get_top_DIE()
        try:
            filename = top_die.attributes["DW_AT_name"].value.decode('ascii')
        except KeyError:
            filename = top_die.get_full_path()
            self._err.write("WARNING: getting the filename failed. Use fallback.")

        basename = os.path.basename(filename)
        modulename = os.path.splitext(basename)[0]
        my_data_out_filename = data_out_filename.replace("#MODULE#", modulename)
        my_data_out = open(my_data_out_filename, "w")

        glob_data_out.write("/* %s */\n" % (basename))

        namesp_out.write("/* %s */\n" % (basename))

        my_data_out.write("/* generated by userspace-header-gen.py */\n")
        my_data_out.write("#include <rtems/linkersets.h>\n")
        my_data_out.write('#include "%s"\n' % (glob_data_out.name))
        my_data_out.write("/* %s */\n" % (basename))

        self._process_die(top_die, my_data_out, glob_data_out, namesp_out)

    def _is_constant(self, die):
        is_constant = False
        try:
            type_offset = die.attributes["DW_AT_type"].value
            typedie = self._die_by_offset[type_offset]
        except KeyError:
            self._err.write("WARNING: Could not find out whether DIE %d is const.\n" % \
                            die.offset)
            pass
        else:
            if typedie.tag == "DW_TAG_const_type":
                is_constant = True
        return is_constant

    def _write_list_to_file(self, l, f):
        if l:
            l.sort()
            f.write('\n'.join(l) + '\n')

    def _process_die(self, die, data_out, glob_data_out, namesp_out):
        data_out_items = []
        glob_data_out_items = []
        namesp_out_items = []

        for child in die.iter_children():
            specdie = child
            # get the name of the DIE
            try:
                varname = child.attributes["DW_AT_name"].value.decode('ascii')
            except KeyError:
                # this might is an external variable with a specification
                # located elsewhere
                try:
                    specification = child.attributes["DW_AT_specification"]\
                                         .value
                    specdie = self._die_by_offset[specification]
                    varname = specdie.attributes["DW_AT_name"].value\
                                     .decode('ascii')
                except KeyError:
                    varname = None

            # filter all none variable or function DIEs
            is_function = False
            if self._die_is_var(child):
                if self._verbose >= VERBOSE_MORE:
                    self._err.write('Process variable DIE: tag=%s, name=%s\n' % \
                                    (child.tag, varname))
            elif self._die_is_function(child):
                if self._verbose >= VERBOSE_MORE:
                    self._err.write('Process function DIE: tag=%s, name=%s\n' % \
                                    (child.tag, varname))
                if varname is None:
                    if self._verbose >= VERBOSE_MORE:
                        self._err.write('Skip function with no name.\n')
                    continue
                is_function = True
            else:
                if self._verbose >= VERBOSE_MORE:
                    self._err.write('DIE is no variable or function: tag=%s, name=%s\n' % \
                                    (child.tag, varname))
                    # FIXME: Check if this die has children and if one of the
                    # children is a function static variable
                continue

            # filter some special names that are used for porting
            if varname in self._rtems_port_names:
                self._err.write('Skip %s. It is a special object for porting.\n' % \
                                (varname))
                continue

            # check if it is an external variable
            is_extern = False
            try:
                is_extern = (specdie.attributes["DW_AT_external"].value != 0)
            except KeyError:
                # if the key is not there it is not extern
                is_extern = False

            # check if it is an declaration
            is_decl = False
            try:
                is_decl = (specdie.attributes["DW_AT_declaration"].value != 0)
            except KeyError:
                # if the key is not there it is not an declaration
                is_decl = False

            # filter declaration only lines (we only want the definitions)
            if is_decl and specdie == child:
                if self._verbose >= VERBOSE_MORE:
                    self._err.write('Skip extern variable "%s" because it is only a declaration.\n' % \
                    (varname))
                continue

            # filter constants
            if (not is_function) and self._is_constant(specdie):
                if self._verbose >= VERBOSE_SOME:
                    self._err.write('Skip const variable "%s" because it is a const.\n' % (varname))
                continue

            # Check if we haven't found a name earlier
            if varname is None:
                raise VarnameNotFoundError

            # Fixup name (necessary if the script runs a second time)
            varname = varname.replace(self._namespace_prefix, "")

            # get file and line
            try:
                decl_file_idx = child.attributes["DW_AT_decl_file"].value - 1
                decl_file = self._lineprogram['file_entry'][decl_file_idx].name.decode('ascii')
            except KeyError:
                decl_file = "<unknown>"
            try:
                decl_line = child.attributes["DW_AT_decl_line"].value
            except KeyError:
                decl_line = "<unknown>"
            var_decl = "%s:%s" % (decl_file, decl_line)

            if self._filterre.match(decl_file) is None:
                if self._verbose >= VERBOSE_SOME:
                    self._err.write('Skip variable "%s" because it\'s declaration file (%s) doesn\'t match the filter\n' % \
                                    (varname, var_decl))
                continue

            # get type for the variable
            if not is_function:
                try:
                    typepre, typepost = self._get_type(specdie)
                except TypenameNotFoundError:
                    self._err.write('Couldn\'t find type for "%s" at %s\n' %
                                    (varname, var_decl))
                    raise
                except AnonymousStructureError:
                    self._err.write('ERROR: anonymous structure "%s" at %s\n' % \
                                    (varname, var_decl))
                    raise
                var_with_type = "%s%s%s" % (typepre, varname, typepost)

                # check if it is a static or a extern
                if not is_extern:
                    var_with_type = "static " + var_with_type
                    out_items = data_out_items
                else:
                    self._err.write('WARNING: variable is not static: "%s" at %s\n' % \
                                    (var_with_type, var_decl))
                    var_with_type = "extern " + var_with_type
                    out_items = glob_data_out_items

                for flt in self._filter_special_vars:
                    if flt["re"].match(var_with_type) is not None:
                        if flt["action"] == "no_section":
                            self._err.write('Don\'t put "%s" into section. Reason: %s.\n' % \
                                            (var_with_type, flt["reason"]))
                            out_items = None
                        if flt["action"] == "ignore_extern":
                            self._err.write('Ignore extern of variable "%s". Reason: %s.\n' % \
                                            (var_with_type, flt["reason"]))
                            out_items = data_out_items

            # write output
            if self._verbose >= VERBOSE_SOME:
                if not is_function:
                    self._err.write('Found a variable "%s" at %s (DIE offset %s); extern: %r\n' % \
                                    (var_with_type, var_decl, child.offset, is_extern))
                else:
                    self._err.write('Found a function "%s" at %s (DIE offset %s); extern: %r\n' % \
                                    (varname, var_decl, child.offset, is_extern))
            if (not is_function) and (out_items is not None):
                out_items.append("RTEMS_LINKER_RWSET_CONTENT(bsd_prog_%s, %s);" % \
                        (self._progname, var_with_type))
            if is_extern:
                namesp_out_items.append("#define %s %s%s" % \
                                 (varname, self._namespace_prefix, varname))

        self._write_list_to_file(data_out_items, data_out)
        self._write_list_to_file(glob_data_out_items, glob_data_out)
        self._write_list_to_file(namesp_out_items, namesp_out)


class UserspaceHeaderGen:
    def __init__(self, objfiles, progname, err = sys.stderr, verbose = 0,
                 filterre = re.compile(".*")):
        self._err = err
        self._verbose = verbose
        self._objfiles = objfiles
        self._progname = progname
        self._filterre = filterre

    def generate_header(self, data_out_filename, glob_data_out, namesp_out):
        """Find all top level (global) variables in the ELF file and generate
        a header.
        """
        glob_data_out.write("/* generated by userspace-header-gen.py */\n")
        glob_data_out.write("#include <rtems/linkersets.h>\n")

        namesp_out.write("/* generated by userspace-header-gen.py */\n")

        for objfile in self._objfiles:
            elffile = ELFFile(objfile)
            if not elffile.has_dwarf_info():
                raise NoDwarfInfoError()

            # Don't relocate DWARF sections. This is not necessary for us but
            # makes problems on ARM with current pyelftools (version 0.24)
            dwarfinfo = elffile.get_dwarf_info(relocate_dwarf_sections=False)

            for cu in dwarfinfo.iter_CUs():
                if self._verbose >= VERBOSE_SOME:
                    self._err.write('Found a CU at offset %s, length %s\n' % \
                                    (cu.cu_offset, cu['unit_length']))

                lineprog = dwarfinfo.line_program_for_CU(cu)
                headergen = HeaderGenCU(cu, self._progname, lineprog, self._err,
                                        self._verbose, self._filterre);
                headergen.generate_header(data_out_filename, glob_data_out,
                                          namesp_out);


if __name__ == '__main__':
    default_filter = '.*'
    default_dataout = 'rtems-bsd-#PROGNAME#-#MODULE#-data.h'
    default_globdataout = 'rtems-bsd-#PROGNAME#-data.h'
    default_namespaceout = 'rtems-bsd-#PROGNAME#-namespace.h'
    parser = argparse.ArgumentParser(
        description=(
            "Generate header files for porting FreeBSD user space tools to RTEMS."
            "Takes an object file as input."
        ))
    parser.add_argument(
        "objfile",
        help="Text arguments. One or more can be appended to the call.",
        type=argparse.FileType("rb"),
        nargs='+'
    )
    parser.add_argument(
        "-f", "--filter",
        help="Only process variables that are defined in files with a name " \
             "matching the given regular expression. " \
             "Default: '%s'" % default_filter,
        dest="filter_string",
        default=default_filter
    )
    parser.add_argument(
        "-p", "--progname",
        help="Name of the program. Default: MYPROG",
        default="MYPROG"
    )
    parser.add_argument(
        "-d", "--dataout",
        help="Name of the output files where the section attributes will be " \
             "added. '#PROGNAME#' will be replaced by the program name " \
             "(set by parameter -p). '#MODULE#' will be replaced by the "
             "current c modules base name. " \
             "Default: '%s'" % (default_dataout),
        default=default_dataout,
        nargs="?"
    )
    parser.add_argument(
        "-g", "--globdataout",
        help="Name of the output files where the section attributes for " \
             "global variables will be added. " \
             "Default: '%s'" % (default_globdataout),
        default=default_globdataout,
        nargs="?"
    )
    parser.add_argument(
        "-n", "--namespaceout",
        help="Name of the output file where namespace definitions will be " \
             "added. Default: '%s'" % (default_namespaceout),
        default=default_namespaceout,
        nargs="?"
    )
    parser.add_argument(
        "-v", "--verbose",
        help="Be more verbose. Can be used multiple times.",
        default=0,
        action="count"
    )
    args = parser.parse_args()

    filterre = re.compile(args.filter_string)

    globdataoutfilename = args.globdataout.replace("#PROGNAME#", args.progname)
    globdataoutfile = open(globdataoutfilename, 'w')

    namespaceoutfilename = args.namespaceout.replace("#PROGNAME#", args.progname)
    namespaceoutfile = open(namespaceoutfilename, 'w')

    dataoutfilename = args.dataout.replace("#PROGNAME#", args.progname)

    uhg = UserspaceHeaderGen(objfiles = args.objfile,
                             verbose = args.verbose,
                             progname = args.progname,
                             filterre = filterre)
    uhg.generate_header(dataoutfilename, globdataoutfile, namespaceoutfile)

# vim: set ts=4 sw=4 et:
