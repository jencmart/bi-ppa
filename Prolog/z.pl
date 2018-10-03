%prvek(-Prvek,+Seznam)
prvek(X, [X|_]) .
prvek(X, [_|B]) :-  prvek(X,B).

%vypust(+Prvek,+Seznam,-Result)
vypust(_,[],[]).
vypust(X, [X|A], R) :- vypust(X,A,R).
vypust(X, [A|B], [A|R]) :- vypust(X,B,R).

%posledni(+list,-prvek)
posledni([A], A).
posledni([_|B], C) :- posledni(B,C).

%prvni(+list,-prvek)
prvni([],_) :- false.
prvni([A|_],A).

%prostredniR
prostredniR([A|_],[],A).
prostredniR([A|_],[_],A).
prostredniR(_,B|_,[_],B). 
prostredniR([_|B], [_,_|E], X) :- prostredniR(B,E,X).

%prostredni(+list,-prvek)
prostredni(A,X) :- prostredniR(A,A,X).

%sudeliche(+list,-sude,-liche)
sudeliche([],[],[]).
sudeliche([A], [], [A]).
sudeliche([A,B|C],[B|X],[A|Y]) :- sudeliche(C,X,Y).

%cons(+lstA, +lstB, -lstAB )

conc([], L2, L2).
conc([A|B], [A|X], L2) :- conc(B,X,L2).

%otoc(+A,-B)
otoc(A,X) :- otocR(A,[],X).

otocR([],X,X).
otocR([A|B], C, X) :- otocR(B, [A|C], X).

%otocme(+lst,-otoceny)
otocme(A,X) :- otocmeR(A,[],X).

otocmeR([],X,X).
otocmeR([A|B], C, X) :- otocmeR(B, [A|C], X).


%symsez
symsez([_]).
symsez([X,X]).
symsez([A|B]) :- otocme(B,[A|X]), symsez(X).


%spoj
spoj([],B,B).
spoj([A|B],C,[A|X]) :- spoj(B,C,X).


%symsezPod(+lst)
symsezPod([X]) :- atomic(X).
symsezPod([X,X]) :- atomic(X).
symsezPod([X,Y]) :- is_list(X), conc(X,H,Y), symsezPod(H).
symsezPod([X]) :- is_list(X), symsezPod(X).
symsezPod([A|B]) :- atomic(A), otocme(B,[A|X]), symsezPod(X).
symsezPod([A|B]) :-  otocme(B,[Y|X]), conc(A,H,Y), symsezPod(H) ,symsezPod(X).


%minMax(+lst,min,max)
min([X],X).
min([A|B], Min) :- min(B,Min2), A < Min2, Min is A.
min([A|B], Min) :- min(B,Min2), A >= Min2, Min is Min2.

max([X],X).
max([A|B], Max) :- max(B,Max2), A > Max2, Max is A.
max([A|B], Max) :- max(B,Max2), A =< Max2, Max is Max2.

minMax(A,Min,Max) :- min(A,Min), max(A,Max).

%contains(+lst,+elem)
contains([X|_],X).
contains([A|B], X) :- A =\= X , contains(B,X).

%zamen(+lst,+lstNaZamenu, -lst).
zamen([],_,[]).
zamen([A|B], C, [D|X]) :- contains(C,A), D is A*A, zamen(B,C,X), !.
zamen([A|B], C, [A|X]) :- zamen(B,C,X).

%naTriRozbalene(+lst, -S1, -S2, -S3)

natri([],[],[],[]).
natri([A|B], [], S2,S3 ) :- is_list(A), natri(A,S2, _, _), natri(B,S3, _,_).
natri([A|B],[A|X],S2,S3) :- natri(B,X,S2,S3).

%rekur(+A,-B)
rekur(1,5).
rekur(2,4).

rekur(A,B) :- A1 is A - 2, rekur(A1,X), A2 is A - 1, rekur(A2,Y), B is( (X*X) - Y).

%rekurS(+A,-B)

rekurS(1,[5]).
rekurS(2,[4,5]).
rekurS(A, [C,X,Y|T]) :- A1 is A - 1,  rekurS(A1, [X,Y|T]), C is ((Y*Y) - X).

%odectiM(+A,+B,-C)

odectiM([],[],[]).
odectiM([A|AR],[B|BR],[C|X]) :- odectiRadek(A,B,C), odectiM(AR,BR,X).


%vynasobM(+A,+B,-C)

vynasob([],[],[]).
vynasob([A|B], [C|D], [E|F]) :- E is A*C, vynasob(B,D,F).

vynasobM([[]|_],_,[]).

zs([],[],[]).
zs([[]|_],[],[]).
zs([[A|B]|R], [B|Y], [A|X]) :- zs(R, Y ,X).

vynasobM([A1|AR],B, [X|Y] ) :- zs(B,BR,B1), vynasob(A1,B1,X), vynasobM(AR,BR,Y).
