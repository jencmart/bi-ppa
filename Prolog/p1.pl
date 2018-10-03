prostredni(S,X) :- prostredni(S,S,X).
prostredni([],[],[]).
prostredni([X|_],[_,_B],X). % levy stred v sude delce
prostredni([X|_],[],X). % pravy stred v sude delce
prostredni([X|_],[_],X). % stred lichy
prostredni([_|S],[_,_|C],X) :- prostredni(S,C,X).

otoc(S,Z) :- otoc(S,[],Z).
otoc([],Ak,Ak).
otoc([X|S],Ak,S2) :- otoc(S, [X|Ak],S2).

symsez([X|[]]).
symsez([X,X]).
symsez([A|B]) :- atomic(A), otoc(B,[A|X]), otoc(X,Y), symsez(Y).
	
symsez([A|B]) :- is_list(A), otoc(B,[Y|Z]), otoc(A,Y), otoc(Z,R), symsez(R).

vloz_na_konec(X,[],[X]).
vloz_na_konec(X,[Y|S],[Y|S2]) :- vloz_na_konec(X,S,S2).
