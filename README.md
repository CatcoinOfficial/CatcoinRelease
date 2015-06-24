Catcoin integration/staging tree
================================

http://www.catcoins.org

Copyright (c) 2009-2013 Bitcoin Developers
Copyright (c) 2011-2013 Litecoin Developers
Copyright (c) 2013-2014 Catcoin Developers
See the file CREDITS for further info on the cats responsible for this

What is Catcoin?
----------------

Catcoin is a version of the Litecoin cryptographic currency, using scrypt
as a proof-of-work algorithm, and using substantially similiar blockchain
parameters to Bitcoin:
 - 50 coins per block
 - 10 minutes block targets
 - Subsidy halves in 210k blocks
 - 21 million total coins
 - 1 block difficulty adjustment, based on average time of last 36 blocks

For more information, as well as an immediately useable, binary version of
the Catcoin client sofware, see http://www.catcoins.org.

License
-------

Catcoin is released under the terms of the MIT license. See `COPYING` for more
information or see http://opensource.org/licenses/MIT.

Development process
-------------------

Developers work in their own trees, then submit pull requests when they think
their feature or bug fix is ready.

If it is a simple/trivial/non-controversial change, then one of the Catcoin
development team members simply pulls it.

If it is a *more complicated or potentially controversial* change, then the
patch submitter will be asked to start a discussion (if they haven't already)
on #catcoin-dev on IRC. (on the [freenode](http://www.freenode.net) network) 

The patch will be accepted if there is broad consensus that it is a good thing.
Developers should expect to rework and resubmit patches if the code doesn't
match the project's coding conventions (see `doc/coding.txt`) or are
controversial.

Testing
-------

Testing and code review is the bottleneck for development; we get more pull
requests than we can review and test. Please be patient and help out, and
remember this is a security-critical project where any mistake might cost people
lots of money.

### Automated Testing

Developers are strongly encouraged to write unit tests for new code, and to
submit new unit tests for old code.

Unit tests for the core code are in `src/test/`. To compile and run them:

    cd src; make -f makefile.unix test

Unit tests for the GUI code are in `src/qt/test/`. To compile and run them:

    qmake BITCOIN_QT_TEST=1 -o Makefile.test catcoin-qt.pro
    make -f Makefile.test
    ./catcoin-qt_test

