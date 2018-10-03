
otoc(S,Z) :- otoc(S,[],Z).
otoc([],Ak,Ak).
otoc([X|S],Ak,S2) :- otoc(S, [X|Ak],S2).

rekur(A,B) :- rekurR(A,C), otoc(C,B).

rekurR(1,[3]).
rekurR(2,[2,3]).
rekurR(N,[X,XA,XB|R]) :- N1 is N-1, rekurR(N1,[XA,XB|R]) , X is XA*XB. 
