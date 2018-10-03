%zamen(+In,+Val,-Out)

%zamen([],_,[]).
%zamen([Head|Tail],Val,[ResHead|ResTail]) :- atomic(Head), zamen(Tail, Val, ResTail), Head =:= Val, ResHead is Head * Head.
%zamen([Head|Tail],Val,[ResHead|ResTail]) :- atomic(Head), zamen(Tail, Val, ResTail), Head =\= Val, ResHead is Head.
%zamen([Head|Tail],Val,[ResHead|ResTail]) :- is_list(Head), zamen(Tail, Val, ResTail), zamen(Head, Val, ResHead).

zamen([], _, []).
zamen([H|T], E, [R|L]):- atomic(H), H =:= E, zamen(T, E, L), R is H*H.
zamen([H|T], E, [R|L]):- atomic(H), H =\= E, zamen(T, E, L), R is H.
zamen([H|T], E, [L1|L2]):- is_list(H), zamen(H, E, L1), zamen(T, E, L2).


minmax([],0,0).
minmax(A,Min,Max) :- min(A,Min), max(A, Max).

min([H],H) :- !.
min([A|B],Min) :- min(B,Min1),A < Min1 , Min = A.
min([A|B],Min) :- min(B,Min1),A >= Min1 , Min = Min1.

max([H],H) :- !.
max([A|B], Max) :- max(B,Max2), A > Max2, Max = A.
max([A|B], Max) :- max(B,Max2), A =< Max2, Max = Max2.


spoj([],L2,L2).
spoj([H1|T1], L2, [H1|R]) :- spoj(T1,L2, R).

minmaxlst(L1,L2,MIN,MAX) :- spoj(L1,L2,R), minmax(R,MIN,MAX).
