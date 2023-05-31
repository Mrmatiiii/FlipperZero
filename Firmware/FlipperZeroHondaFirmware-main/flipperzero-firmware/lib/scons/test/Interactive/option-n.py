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
Verify that the -n option, specified on the build command, reports
what would be built but doesn't actually build anything.
"""

import TestSCons

test = TestSCons.TestSCons()

test.write('SConstruct', """\
Command('foo.out', 'foo.in', Copy('$TARGET', '$SOURCE'))
Command('1', [], Touch('$TARGET'))
Command('2', [], Touch('$TARGET'))
""")

test.write('foo.in', "foo.in\n")



scons = test.start(arguments = '-Q --interactive')

scons.send("build -n foo.out\n")

scons.send("build 1\n")

test.wait_for(test.workpath('1'), popen=scons)

test.must_not_exist(test.workpath('foo.out'))



scons.send("build foo.out\n")

scons.send("build 2\n")

test.wait_for(test.workpath('2'), popen=scons)

test.must_match(test.workpath('foo.out'), "foo.in\n")



expect_stdout = """\
scons>>> Copy("foo.out", "foo.in")
scons>>> Touch("1")
scons>>> Copy("foo.out", "foo.in")
scons>>> Touch("2")
scons>>> 
"""

test.finish(scons, stdout = expect_stdout)



test.pass_test()

# Local Variables:
# tab-width:4
# indent-tabs-mode:nil
# End:
# vim: set expandtab tabstop=4 shiftwidth=4:
