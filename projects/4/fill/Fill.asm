// This file is part of www.nand2tetris.org
// and the book "The Elements of Computing Systems"
// by Nisan and Schocken, MIT Press.
// File name: projects/4/Fill.asm

// Runs an infinite loop that listens to the keyboard input. 
// When a key is pressed (any key), the program blackens the screen,
// i.e. writes "black" in every pixel. When no key is pressed, 
// the screen should be cleared.

//// Replace this comment with your code.

(START)
    @KBD
    D=M
    @WHITE
    D;JEQ


    @BLACK
    0;JMP

    (RETBLACK)
    @START
    0;JMP

(WHITE)
    @SCREEN
    D=A

    @ADDR
    M=D

    @8192
    D=A

    @ADDR
    D=D+M

    @LAST
    M=D

    (LOOPW)
        @LAST
        D=M

        @ADDR
        D=D-M
        @START
        D;JLE

        @ADDR
        A=M
        M=0

        @ADDR
        M=M+1
        @LOOPW
        0;JMP



(BLACK)
    @SCREEN
    D=A

    @ADDR
    M=D

    @8192
    D=A

    @ADDR
    D=D+M

    @LAST
    M=D

    (LOOPB)
        @LAST
        D=M

        @ADDR
        D=D-M
        @RETBLACK
        D;JLE

        @ADDR
        A=M
        M=-1

        @ADDR
        M=M+1
        @LOOPB
        0;JMP


    





