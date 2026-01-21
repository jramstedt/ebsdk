#!/usr/bin/sed -f

# Convert comments
s~^;~#~
s~;~#~

# Convert directives
# TODO argument(s)
# TODO Logical OR (! in MACRO64)
s~\.title\s(.*)~.title \"\1\"~
s~\.if\s+(EQ|EQUAL)\s(.*)~.ifeq \2~I
s~\.if\s+(NE|NOT_EQUAL)\s(.*)~.if \2~I
s~\.if\s+(GT|GREATER)\s(.*)~.ifgt \2~I
s~\.if\s+(LE|LESS_EQUAL)\s(.*)~.ifle \2~I
s~\.if\s+(LT|LESS_THAN)\s(.*)~.iflt \2~I
s~\.if\s+(GE|GREATER_EQUAL)\s(.*)~.ifge \2~I
s~\.if\s+(DF|DEFINED)\s(.*)~.ifdef \2~I
s~\.if\s+(NDF|NOT_DEFINED)\s(.*)~.ifndef \2~I
s~\.if\s+(B|BLANK)\s(.*)~.ifb \2~I
s~\.if\s+(NB|NOT_BLANK)\s(.*)~.ifnb \2~I

# TODO .iif

# TODO
#s~.if\s(IDN|IDENTICAL)\s(.*)~.if? \2~I
#s~.if\s(DIF|DIFFERENT)\s(.*)~.if? \2~I

# Not exactly correct. Should actually be .elseif prev equation != zero
s~\.(IFF|IF_FALSE)~.else #.IFF~I
# Not exactly correct. Should actually be .elseif prev equation == zero
s~\.(IFT|IF_TRUE)~.else #.IFT~I
# code block should always be included
s~\.(IFTF|IF_TRUE_FALSE)~.if 1 #.IFTF~I

s~\.ENDC~.endif~I

# TODO alignment (https://sourceware.org/binutils/docs/as/Section.html), attributes (https://docs.vmssoftware.com/vsi-openvms-vax-macro-and-instruction-set-reference-manual/#PROGSEC_ATTR)
# .psect\s+(\w+\$?\w+)(?:,\s([0-9]|exe|mix))
s~\.PSECT\s+(\w+\$?\w+)~.section \1~I
s~\.RESTORE_PSECT~.previous~I

# TODO ?? .vtable_entry ??
s~\.ADDRESS\s+(.*)\s+~.long \1~I

s~\.ASCII\s?(.)(.*)\1~.ascii \"\2\"~I
s~\.ASCIZ\s?(.)(.*)\1~.asciz \"\2\"~I

# Convert constants
s~\^B([01]+)~0b\1~I
s~\^D([0-9]+)~\1~I
s~\^O([0-7]+)~0\1~I
s~\^X([0-9A-F]+)~0x\1~I

# Convert registers
s~([,(\s])(fp|sp|pc)([\s),\s#\n])~\1$\2\3~Ig
s~r15~$fp~Ig
s~r28~$at~Ig
s~r29~$gp~Ig
s~r30~$sp~Ig
s~r[1-3]?[0-9]~$&~Ig
s~f[1-3]?[0-9]~$&~Ig

# Macro
# TODO https://docs.vmssoftware.com/vsi-openvms-vax-macro-and-instruction-set-reference-manual/#_0_ARGUMENTSINMACROS https://sourceware.org/binutils/docs/as/Macro.html

s~(.*)::~.global \1\n\1:~