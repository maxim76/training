#!/usr/bin/python
#from conio_lnx import *
#from conio_win import *
from conio import *

cprintf( Colors.Black, '%-10s','Black');
cprintf( Colors.Grey, 'Grey\n');
cprintf( Colors.Red, '%-10s','Red');
cprintf( Colors.LRed, 'LRed\n');
cprintf( Colors.Green, '%-10s','Green');
cprintf( Colors.LGreen, 'LGreen\n');
cprintf( Colors.Yellow, '%-10s','Yellow');
cprintf( Colors.LYellow, 'LYellow\n');
cprintf( Colors.Blue, '%-10s','Blue');
cprintf( Colors.LBlue, 'LBlue\n');
cprintf( Colors.Magenta, '%-10s','Magenta');
cprintf( Colors.LMagenta, 'LMagenta\n');
cprintf( Colors.Cyan, '%-10s','Cyan');
cprintf( Colors.LCyan, 'LCyan\n');
cprintf( Colors.LGrey, '%-10s','LGrey');
cprintf( Colors.White, 'White\n');

cprintf( Colors.Header, '\nExample of test something\n' )
printf( '%-40s', 'Doing task1...' )
printf( 'done\n' )
printf( '%-40s', 'Doing task2...' )
printf( 'done\n' )

cprintf( Colors.OK, 'Example test passed\n' )
