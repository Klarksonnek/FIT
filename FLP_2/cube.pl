% Rubikova kostka
% xnecas24
% Klara Necasova

% checks if cube is solved (all cube faces contain the same items)
solved([[U,U,U,U,U,U,U,U,U],[F,F,F,F,F,F,F,F,F],[R,R,R,R,R,R,R,R,R],[B,B,B,B,B,B,B,B,B],[L,L,L,L,L,L,L,L,L],[D,D,D,D,D,D,D,D,D]]).

% clockwise rotations
% rotation around x-axis
xcw([[U1,U2,U3, U4,U5,U6, U7,U8,U9],
     [F1,F2,F3, F4,F5,F6, F7,F8,F9],
     [R1,R2,R3, R4,R5,R6, R7,R8,R9],
     [B1,B2,B3, B4,B5,B6, B7,B8,B9],
     [L1,L2,L3, L4,L5,L6, L7,L8,L9],
     [D1,D2,D3, D4,D5,D6, D7,D8,D9]],

    [[F1,F2,F3, F4,F5,F6, F7,F8,F9],
     [D1,D2,D3, D4,D5,D6, D7,D8,D9],
     [R7,R4,R1, R8,R5,R2, R9,R6,R3],
     [U9,U8,U7, U6,U5,U4, U3,U2,U1],
     [L3,L6,L9, L2,L5,L8, L1,L4,L7],
     [B9,B8,B7, B6,B5,B4, B3,B2,B1]]).
% rotation around y-axis
ycw([[U1,U2,U3, U4,U5,U6, U7,U8,U9],
     F,
     R,
     B,
     L,
     [D1,D2,D3, D4,D5,D6, D7,D8,D9]],

    [[U7,U4,U1, U8,U5,U2, U9,U6,U3],
     R,
     B,
     L,
     F,
     [D3,D6,D9, D2,D5,D8, D1,D4,D7]]).
% rotation around z-axis
zcw(From, To) :- xcw(From, Temp1), ycw(Temp1, Temp2), xccw(Temp2, To).

% clockwise moves
% left side
lcw([[U1,U2,U3, U4,U5,U6, U7,U8,U9],
     [F1,F2,F3, F4,F5,F6, F7,F8,F9],
     [R1,R2,R3, R4,R5,R6, R7,R8,R9],
     [B1,B2,B3, B4,B5,B6, B7,B8,B9],
     [L1,L2,L3, L4,L5,L6, L7,L8,L9],
     [D1,D2,D3, D4,D5,D6, D7,D8,D9]],

    [[B9,U2,U3, B6,U5,U6, B3,U8,U9],
     [U1,F2,F3, U4,F5,F6, U7,F8,F9],
     [R1,R2,R3, R4,R5,R6, R7,R8,R9],
     [B1,B2,D7, B4,B5,D4, B7,B8,D1],
     [L7,L4,L1, L8,L5,L2, L9,L6,L3],
     [F1,D2,D3, F4,D5,D6, F7,D8,D9]]).
% rigth side
rcw(From, To) :- y2(From, Temp1), lcw(Temp1, Temp2), y2(Temp2, To).
% front side
fcw(From, To) :- ycw(From, Temp1), lcw(Temp1, Temp2), yccw(Temp2, To).
% back side
bcw(From, To) :- yccw(From, Temp1), lcw(Temp1, Temp2), ycw(Temp2, To).
% up side
ucw(From, To) :- zccw(From, Temp1), lcw(Temp1, Temp2), zcw(Temp2, To).
% down side
dcw(From, To) :- zcw(From, Temp1), lcw(Temp1, Temp2), zccw(Temp2, To).
% middle slice - horizontally
ecw(From, To) :- ucw(From, Temp1), dccw(Temp1, Temp2), yccw(Temp2, To).
% middle slice - vertically
mcw(From, To) :- rcw(From, Temp1), lccw(Temp1, Temp2), xccw(Temp2, To).
% middle slice - vertically
scw(From, To) :- fccw(From, Temp1), bcw(Temp1, Temp2), zcw(Temp2, To).

% counterclockwise rotations
% rotation around x-axis
xccw(From, To) :- xcw(To, From).
% rotation around y-axis
yccw(From, To) :- ycw(To, From).
% rotation around z-axis
zccw(From, To) :- zcw(To, From).

% counterclockwise moves
% left side
lccw(From, To) :- lcw(To, From).
% rigth side
rccw(From, To) :- rcw(To, From).
% front side
fccw(From, To) :- fcw(To, From).
% back side
bccw(From, To) :- bcw(To, From).
% up side
uccw(From, To) :- ucw(To, From).
% down side
dccw(From, To) :- dcw(To, From).
% middle slice - horizontally
eccw(From, To) :- ecw(To, From).
% middle slice - vertically
mccw(From, To) :- mcw(To, From).
% middle slice - vertically
sccw(From, To) :- scw(To, From).

% 2 rotations
% rotation around x-axis
x2(From, To) :- xcw(From, Temp), xcw(Temp, To).
% rotation around y-axis
y2(From, To) :- ycw(From, Temp), ycw(Temp, To).
% rotation around z-axis
z2(From, To) :- zcw(From, Temp), zcw(Temp, To).

% 2 clockwise moves
% left side
l2(From, To) :- lcw(From, Temp), lcw(Temp, To).
% right side
r2(From, To) :- rcw(From, Temp), rcw(Temp, To).
% front side
f2(From, To) :- fcw(From, Temp), fcw(Temp, To).
% back side
b2(From, To) :- bcw(From, Temp), bcw(Temp, To).
% up side
u2(From, To) :- ucw(From, Temp), ucw(Temp, To).
% down side
d2(From, To) :- dcw(From, Temp), dcw(Temp, To).
% middle slice - horizontally
e2(From, To) :- ecw(From, Temp), ecw(Temp, To).
% middle slice - vertically
m2(From, To) :- mcw(From, Temp), mcw(Temp, To).
% middle slice - vertically
s2(From, To) :- scw(From, Temp), scw(Temp, To).

% algorithm (IDS)
% list of list of all moves
moves([[lcw, lccw, l2],
       [rcw, rccw, r2],
       [fcw, fccw, f2],
       [bcw, bccw, b2],
       [ucw, uccw, u2],
       [dcw, dccw, d2],
       [ecw, eccw, e2],
       [mcw, mccw, m2],
       [scw, sccw, s2]]).

% maximum count of moves
maxMoves(20).

% following part of code was taken from input2.pl file
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% reads line from stdin, terminates on LF or EOF
read_line(L,C) :-
	get_char(C),
	(isEOFEOL(C), L = [], !;
		read_line(LL,_),% atom_codes(C,[Cd]),
		[C|LL] = L).

% tests if character is EOF or LF
isEOFEOL(C) :-
	C == end_of_file;
	(char_code(C,Code), Code==10).

% reads lines from stdin
read_lines(Ls) :-
	read_line(L,C),
	( C == end_of_file, Ls = [] ;
	  read_lines(LLs), Ls = [L|LLs]
	).

% splits line to sublists
split_line([],[[]]) :- !.
split_line([' '|T], [[]|S1]) :- !, split_line(T,S1).
split_line([32|T], [[]|S1]) :- !, split_line(T,S1).
split_line([H|T], [[H|G]|S1]) :- split_line(T,[G|S1]).

% splits lines to sublists
split_lines([],[]).
split_lines([L|Ls],[H|T]) :- split_lines(Ls,T), split_line(L,H).
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

% changes input cube representation to internal representation - one face is represented as a list
changeCubeRepresentation(
    [
    [[U1,U2,U3]],
    [[U4,U5,U6]],
    [[U7,U8,U9]],
    [[F1,F2,F3],[R1,R2,R3],[B1,B2,B3],[L1,L2,L3]],
    [[F4,F5,F6],[R4,R5,R6],[B4,B5,B6],[L4,L5,L6]],
    [[F7,F8,F9],[R7,R8,R9],[B7,B8,B9],[L7,L8,L9]],
    [[D1,D2,D3]],
    [[D4,D5,D6]],
    [[D7,D8,D9]]
    ],
    [
    [U1,U2,U3,U4,U5,U6,U7,U8,U9],
    [F1,F2,F3,F4,F5,F6,F7,F8,F9],[R1,R2,R3,R4,R5,R6,R7,R8,R9],[B1,B2,B3,B4,B5,B6,B7,B8,B9],[L1,L2,L3,L4,L5,L6,L7,L8,L9],
    [D1,D2,D3,D4,D5,D6,D7,D8,D9]
    ]
    ).

% checks if algorithm solves cube successfully 
check(Start, []) :- solved(Start), !.
check(Start, [H|T]) :- call(H, Start, New), check(New, T).

% generates all algorithms of given length
algorithm([], 0, _) :- !.
algorithm([H|T], Len, LeftList) :- moves(MovesList), member(Moves, MovesList), Moves \= LeftList, Len1 is Len-1, member(H, Moves), algorithm(T, Len1, Moves).
algorithms(Alg) :- maxMoves(MaxMoves), between(0, MaxMoves, Len), algorithm(Alg, Len, []).

% tries to solve cube
solve(Start, Alg) :- algorithms(Alg), check(Start, Alg).

% writes line
writeLine([]) :- write('\n').
writeLine([H|T]) :- write(H), writeLine(T).  

% writes lines
writeLines([]).
writeLines([H|T]) :- writeLine(H), writeLines(T).

% reads cube from input and converts it to internal representation 
readCube(Cube) :- read_lines(Input), split_lines(Input,Data), changeCubeRepresentation(Data,Cube).

% converts cube representation to input representation and writes it
writeCube(Cube) :- changeCubeRepresentation(Data,Cube), split_lines(Output,Data), writeLines(Output).

% writes algorithm (move sequence)
writeAlgorithm(Start, []) :- writeCube(Start).
writeAlgorithm(Start, [H|T]) :- writeCube(Start), write('\n'), call(H, Start, New), writeAlgorithm(New, T).

main :-
      prompt(_, ''),
      % reads input cube representation and converts it to internal representation
      readCube(Start),
      % tries to solve cube
      solve(Start, Algorithm),
      % converts cube representation to input representation and writes it
      writeAlgorithm(Start, Algorithm),
      halt.
