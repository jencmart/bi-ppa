nalezi(X,[X|_]).
nalezi(X, [_|S]) :- nalezi(X,S).

%odeber(+el, +lst, -res)
odeber(_,[],[]).
odeber(A,[A|C], X) :- odeber(A,C,X).
odeber(A,[B|C], [B|X]) :- odeber(A,C, X).

%odeberVsechny(+S1,+S2,-S)
odeberVse([],S,S).
odeberVse([A|B], S2, X) :- odeber(A,S2, Y), odeberVse(B,Y,X).

%jeHrana
hrana(U,V,E) :- nalezi([U,V], E).

%neniHrana
nenihrana(U,V,E) :- nalezi([U,V],E), !, false.
nenihrana(_,_,_).


%cesta(+A,+B,+GRAF,-Cesta) vypise cestu z A do B if exist
cesta(A,B,[V,E],OUT) :- odeber(A,V,VNew), cesta(A,B,VNew, E, OUT).


cesta(A,B,_,E, [A,B]) :- nalezi([A,B], E).
cesta(A,B,V,E, [A|X]) :- nalezi([A,U], E), A \= U, A \= B, nalezi(U,V), odeber(U, V,V1), cesta(U,B,V1, E,X).

%cyklus(+A,+graf,-Out)
cyklus(A,[V|E],Out) :- cesta(A,A,V,E, Out).

%jeCyklus(+graf, -Out)
jeCyklus([[A|V],E], Out) :- cyklus(A,[V,E],Out), !.
jeCyklus([[_|V],E], Out) :-jeCyklus(V,E, Out).


%sousede(+A,+Graf, -Sous)



projdiSloupce(A,[B|R],G,Out) :- cesta(A,B,G,Out).
projdiSloupce(A,[B|R],G,Out) :- projdiSloupce(A,R,G,Out).

projdiRadky([A|R],S, G, Out) :- projdiSloupce(A,S,G,Out) ; projdiRadky(R,S,G,Out).

jeCyklusO(A,[V|E],Out) :- sousede(A,[V,E],S),  odeber(A,V,V1), odeber([A,_],E,E1), odeber([_,A], E1, E2), projdiRadky(S,S,[V1,E2],Out).



sousede(A,[_,E],S) :- sousede(A,E,S,_).

sousede(A,[],[],_).
sousede(A, [[A,B]|R] , [B|X],_) :- sousede(A,R,X,_).
sousede(A, [[C,_]|R] , X,_) :- A \= C, sousede(A,R,X,_).


bfs(_,[],_,[]).
bfs(W,V,E,[W]) :- nenihrana(W,_,E).
bfs(W,V,E,[W|Out]) :- sousede(W,E,Sous), smaz(Sous,V,V1), vymazvse([W,_],E,E1),bfs(Sous,V1,E1,Uzavrene), spoj(Sous,Uzavrene,Out).

%select sort



odstran([],_,[]).
odstran([A|R], A, R).
odstran([B|R], A, [B|X]) :- B \= A, odstran(R,A,X).

min([A], A).
min([A|R], Min) :- min(R,Min2), A < Min2, Min is A.
min([A|R], Min) :- min(R,Min2), A >= Min2, Min is Min2.


selectSort([], []).
selectSort(S1, [Min|R]) :- min(S1,Min), odstran(S1,Min,S2), selectSort(S2,R), !.
