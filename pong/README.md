# pong

## Description
Pong game

# Issue 
abcd key pad value become wrong when there is i2c output
A : ~166 (orginal ~680)
B : ~240 (orginal ~496)
C : ~74  (original ~320)
D : ~165 (original ~160)
Probably due to clock input changes when i2c is setup

# current way to fix
1. use values from D3 to help for detection when i2c is being used