#!/usr/bin/env python
#
# __COPYRIGHT__
#
# Permission is hereby granted, free of charge, to any person obtaining
# a copy of this software and associated documentation files (the
# "Software"), to deal in the Software without restriction, including
# without limitation the rights to use, copy, modify, merge, publish,
# distribute, sublicense, and/or sell copies of the Software, and to
# permit persons to whom the Software is furnished to do so, subject to
# the following conditions:
#
# The above copyright notice and this permission notice shall be included
# in all copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY
# KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
# WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
# NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE
# LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION
# OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
# WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
#

__revision__ = "__FILE__ __REVISION__ __DATE__ __DEVELOPER__"

"""
Verify that the time subcommand only prints results for the last number
of files specified with the -t and --tail options.
"""

import TestSCons_time

test = TestSCons_time.TestSCons_time()


header = '       Total  SConscripts        SCons     commands\n'

lines = []

line_fmt = '   11.123456    22.234567    33.345678    44.456789    %s\n'

for i in range(9):
    logfile_name = 'foo-%s.log' % i
    test.fake_logfile(logfile_name)
    lines.append(line_fmt % logfile_name)

expect3 = [header] + lines[-3:]
expect5 = [header] + lines[-5:]

test.run(arguments = 'time -t 3 foo-*.log', stdout = ''.join(expect3))

test.run(arguments = 'time --tail 5 foo-*.log', stdout = ''.join(expect5))

test.pass_test()

# Local Variables:
# tab-width:4
# indent-tabs-mode:nil
# End:
# vim: set expandtab tabstop=4 shiftwidth=4:
