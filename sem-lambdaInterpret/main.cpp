#include <iostream>
#include <utility>
#include <vector>
#include <set>
#include <sstream>

using namespace std;
class Alpha{};
class UnreductableException {
};
class UnexpectedCharacter
{
    char m_x;
public:

    UnexpectedCharacter(char x ) : m_x(x) {}

    friend ostream &operator<<(ostream &os, const UnexpectedCharacter &character) {
        os << "m_x: " << ">" << character.m_x <<  "<"<< endl;
        return os;
    }
};


class EmptyExpression
{

};

class UnexpectedEndOfExpression
{

};

class Unextractable {
};

enum reducType {
    NORMAL, APPLICATIVE
};

class Lambda;

class Variable ;

/// ********************************************************************************************************************
/// ************************************** EXPRESSION ******************************************************************
/// ********************************************************************************************************************
class Expression {
    virtual std::ostream &doprint(std::ostream &) const = 0;

    virtual bool reductionInit(reducType x) { throw UnreductableException(); }

public:
    Expression() = default;

    virtual ~Expression() = default;

    friend ostream &operator<<(ostream &os, const Expression &vyraz) {
        return vyraz.doprint(os);
    }

    void print() {
        cout << "  \u2192";
        doprint(cout);
        cout << endl;
    }

    ///******************** BOUND DETECTION ********************
    virtual void detectFreeBoundVariables(vector<string> &) const = 0;

    virtual void getAllNames(set<string> &) const = 0;

    virtual void detecFreeBoundVariablesAlpha(vector<pair<Variable*, vector<Variable*>>> & variables, vector<pair<Variable*, vector<Variable*>>> & results) = 0;

    virtual Expression *clone() = 0;

    void detectBound() const {
        vector<string> tmpVector;
        detectFreeBoundVariables(tmpVector);
    }

    vector<pair<Variable*,vector<Variable*>>> boundForAlpha()
    {
        vector<pair<Variable*,vector<Variable*>>> variables;
        vector<pair<Variable*,vector<Variable*>>> results;
        detecFreeBoundVariablesAlpha(variables, results);
        return results;
    }


    ///******************** REDUCTION ********************
    bool reductStep(reducType x) {
        try {
            if( reductionInit(x) )
                cout << "\u03B2 \u2192" << *this << endl;
            else
                cout << "\u03B1 \u2192" << *this << endl;
            return true;
        }
        catch (UnreductableException &x) {
           cout   << endl;
            return false;
        }
    }

    void reduct(reducType x) {
        while (true) {
            try {
                if( reductionInit(x) )
                    cout << "\u03B2 \u2192" << *this << endl;
                else
                    cout << "\u03B1 \u2192" << *this << endl;
            }
            catch (UnreductableException &x) {
                cout   << endl;
                return;
            }
        }
    }

    virtual Expression *replaceBound(string s, Expression *v) = 0;

    virtual Expression *replaceInit(Expression *v) = 0;

    virtual std::vector<Expression *> tryExtract() { throw Unextractable(); }

    virtual void applicativeReduction() { throw UnreductableException(); }

    virtual bool normalReduction() { throw UnreductableException(); }

    virtual bool isLambda() const { return false; }

};

/// ********************************************************************************************************************
/// ************************************** VARIABLE ********************************************************************
/// ********************************************************************************************************************
class Variable : public Expression {
    string m_name;

    std::ostream &doprint(std::ostream &os) const override {
        cout << m_name;
        return os;
    }

    Expression *clone() override {
        return new Variable(*this);
    }

    void detectFreeBoundVariables(vector<string> &parametres) const override {
        cout << m_name << " ";

        for (auto &str : parametres) {
            if (str == m_name) {
                cout << "bound\n";
                return;
            }
        }
        cout << "free\n";
    }

    void getAllNames(set<string> & x) const override
    {
        x.insert(m_name);
    }

    void detecFreeBoundVariablesAlpha(vector<pair<Variable*, vector<Variable*>>> & variables, vector<pair<Variable*, vector<Variable*>>> & results) override  /// TODO 1.
    {
        for (auto it = variables.begin(); it != variables.end(); ++it)
        {
            if( it->first->getName() ==  m_name )
            {
                it->second.emplace_back(this);
                return;
            }
        }
    }

public:

    Variable(Variable &x) : m_name(x.m_name) {}

    explicit Variable(string jm) : m_name(std::move(jm)) {}

    string getName() const { return m_name; }

    void rename(string & jmeno) { m_name = jmeno;}
    Expression *replaceInit(Expression *v) override {
        throw UnreductableException();
    }

    /// **** REPLACE BOUND ****
    Expression *replaceBound(string s, Expression *v) override {
        if (s == m_name)
            return v->clone();
        else
            return this;
    }

    ~Variable() override = default;
};


/// ********************************************************************************************************************
/// **************************************** LAMBDA ********************************************************************
/// ********************************************************************************************************************
class Lambda : public Expression {
    Variable m_argument;
    Expression *m_body;

    std::ostream &doprint(std::ostream &os) const override {
        cout << "(" << "\u03BB" << m_argument << "." << *m_body << ")";
        return os;
    }


    Lambda(Lambda &x) : m_argument(x.m_argument) {
        m_body = x.m_body->clone();
    }

    Expression *clone() override {
        return new Lambda(*this);
    }

    void detectFreeBoundVariables(vector<string> &parametres) const override {
        parametres.push_back(m_argument.getName());
        m_body->detectFreeBoundVariables(parametres);
        parametres.pop_back();
    }
    void getAllNames(set<string> & x) const override
    {
        x.insert(m_argument.getName());
    }


    void detecFreeBoundVariablesAlpha(vector<pair<Variable*, vector<Variable*>>> & variables, vector<pair<Variable*, vector<Variable*>>> & results) override /// TODO 2.
    {
        variables.emplace_back(&m_argument,vector<Variable*>()); // pridej do variables
        m_body->detecFreeBoundVariablesAlpha(variables, results); // nech vypocitat

        results.emplace_back(variables.back());  // pridej vysledek do results...
        variables.pop_back();
    }


    //// ***** REPLACE INIT V LAMBDE ********
    Expression *replaceInit(Expression *v) override {
        Expression *tmpBody = m_body;
        m_body = m_body->replaceBound(m_argument.getName(), v);

        if (tmpBody != m_body)
            delete tmpBody;

        return m_body;
    }

    bool isLambda() const override { return true; }

    Expression *replaceBound(string s, Expression *v) override {
        if(s == m_argument.getName()) // TODO --------------------------------------------------- VERY MUCH IMPORTANT TO HAVE IT HERE !!!!!!!!!!!!!!!!!!!!!!!!!!
            return this;
        Expression *tmpBody = m_body;
        m_body = m_body->replaceBound(s, v);
        if (tmpBody != m_body)
            delete tmpBody;
        return this;
    }

public:
    Lambda(string jm, Expression *vyr) : m_argument(std::move(jm)), m_body(vyr) {}

    string getName() const
    {
        return m_argument.getName();
    }

    ~Lambda() override {
        delete m_body;
    }
};


/// ********************************************************************************************************************
/// *********************************** APPLICATION ********************************************************************
/// ********************************************************************************************************************
class Application : public Expression {
private:
    std::vector<Expression *> m_expressions;

    std::ostream &doprint(std::ostream &os) const override;

    Expression *clone() override {
        return new Application(*this);
    }

    /// DETECT BOUND
    void detectFreeBoundVariables(vector<string> &parametres) const override {
        for (Expression *v : m_expressions)
            v->detectFreeBoundVariables(parametres);
    }

    void getAllNames(set<string> & x) const override
    {
        for (Expression *v : m_expressions)
            v->getAllNames(x);
    }


    void detecFreeBoundVariablesAlpha(vector<pair<Variable*, vector<Variable*>>> & variables, vector<pair<Variable*, vector<Variable*>>> & results) override    //// TODO 3.
    {
        for (Expression *v : m_expressions)
            v->detecFreeBoundVariablesAlpha(variables, results);
    }

    /// *****REDUCTION******
    Expression *
    replaceInit(Expression *v) override  ///  todo. tadyto replacovani musi nastat pro vsechny prvky?? ...nope.....
    {
        for (auto &m_expression : m_expressions) {
            try {
                m_expression = m_expression->replaceInit(v);
                return this;
            }
            catch (UnreductableException &x) {}
        }

        throw UnreductableException();
    }

    bool isLambda() const override {
        for (auto m_expression : m_expressions)
            if (m_expression->isLambda())
                return true;
            else
                return false;    ///////////////////////////////////////////////// TODO TO JSEM PRIDAL TA TO TED POJEBE

        return false;
    }

    Expression *replaceBound(string s, Expression *v) override {
        for (auto &m_expression : m_expressions) {
            Expression *tmpExpression = m_expression;
            m_expression = m_expression->replaceBound(s, v);
            if (tmpExpression != m_expression)
                delete tmpExpression;
        }

        return this;
    }

    std::vector<Expression *> tryExtract() override {
        if (m_expressions.empty())
            throw Unextractable();

        if (m_expressions.size() == 1)
            return m_expressions[0]->tryExtract();  // todo asi kdyz se mi povede z toho neco vratit, tak ten vector musim okopirovat, a tu aplikaci nasledne deletnout....

        return m_expressions;
    }

    void unwrap() {
        std::vector<Expression *> tmp;
        try { tmp = m_expressions[0]->tryExtract(); }
        catch (Unextractable &x) {
            throw UnreductableException();
        }

        Expression *toDelete = m_expressions[0];

        std::vector<Expression *> newExpressions;

        for (auto &i : tmp)
            newExpressions.emplace_back(i->clone());

        m_expressions = newExpressions;

        delete toDelete;
    }


    /// *********************************** APPLICATIVE REDUCTION *********************************
    void applicativeReduction() override {
        if (m_expressions.empty())
            throw UnreductableException();

        if (m_expressions.size() == 1)
            unwrap();

        for (int i = 0; i < (int) m_expressions.size() - 1; ++i)    /// tzn prvni lambda nemusi byt nutne na zacatku !!
        {
            if (m_expressions[i]->isLambda()) {
                try /// zkus jit vic do hlobky...
                {
                    m_expressions[i + 1]->applicativeReduction();
                }
                catch (UnreductableException &x)  // normalni beta redukce....
                {
                    reductionInit(NORMAL);   /// ! NEMUZE hodit, protoze predtim mame, ze replace init possible.....
                    return;
                }
                return;
            }
        }

        throw UnreductableException();
    }

    bool rename(std::vector<pair<Variable * , vector<Variable*>>> &, std::vector<pair<Variable * , vector<Variable*>>>&, std::vector<string> &);

    /// *********************************** NORMAL REDUCTION *********************************
    bool normalReduction() override {
        if (m_expressions.empty())
            throw UnreductableException();

        if (m_expressions.size() == 1)
            unwrap();

        for (int i = 0; i < (int ) m_expressions.size() - 1; ++i) {
            if (m_expressions[i]->isLambda()) {


                try /// zkus jit vice do hloubky, ackoli zde vime ze to uz je v poradku....
                {
                    if(m_expressions[i]->normalReduction())
                        return true;
                    else
                        throw UnreductableException(); //  TODO OOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOO
                }
                catch (UnreductableException &x) {}


                Expression *forSubsutitution = m_expressions[i + 1];

                Expression *subsituter = m_expressions[i];

                std::vector<pair<Variable * , vector<Variable*>>> forSubVariables =  forSubsutitution->boundForAlpha();
                std::vector<pair<Variable * , vector<Variable*>>> substituerVariables = subsituter->boundForAlpha();


                std::set<string> names;
                forSubsutitution->getAllNames(names);
                subsituter->getAllNames(names);

                std::vector<string> namesV(names.begin(), names.end());

                if( rename(forSubVariables, substituerVariables, namesV ))
                    throw Alpha();

                // TODO *** TADY SE ROZHODNE O ALPHA REDUKCI - A NEBO NE ???

                Expression *tmpExpression = m_expressions[i]->replaceInit(
                        forSubsutitution); /// toto NEMUZE hodit unreductable, protoze tam je labda....

                Expression *copiedExpression = tmpExpression->clone();
                delete m_expressions[i + 1];
                delete m_expressions[i];
                m_expressions.erase(m_expressions.begin() + i + 1);
                m_expressions[i] = copiedExpression;

                return true;
            }


        }

        return false;
    }

    bool reductionInit(reducType reduction) override {

        try
        {
            if (reduction == NORMAL) {
                if (normalReduction())
                    return true;
                else
                    throw UnreductableException();
            } else
            {
                applicativeReduction();
                return true;
            }

        }catch( Alpha & x)
        {
            return false;
        }

    }

public:
    Application(initializer_list<Expression *> vyr) : m_expressions(vyr) {}

    Application(vector<Expression *> vyr) : m_expressions(std::move(vyr)) {}

    Application(Application &x) {
        for (auto &m_expression : x.m_expressions)
            m_expressions.emplace_back(m_expression->clone());
    }

    ~Application() override {
        for (auto &m_expression : m_expressions) {
            delete m_expression;
        }
    }
};


class ExpressionGenerator
{
private:
    void parseToList(string & source, vector<string> & myVector)
    {

        for(size_t i = 0 ; i < source.size() ; ++i)
        {
            if( isspace( source[i]) )
                continue;

            if(source[i] == '(') // od zavorky k zavorce
            {
                string expr;
                vector<char> buff;
                buff.push_back('(');
                size_t j;
                expr.push_back(source[i]);

                for(j = ++i; j < source.size() ; ++j)
                {
                    if(buff.empty())
                        break;

                    if(source[j] == '(')
                        buff.push_back('(');

                    else if(source[j] == ')')
                        buff.pop_back();

                    expr += source[j];
                }

                if( ! buff.empty())
                    throw UnexpectedCharacter(source.back());

                i = j - 1;

                myVector.emplace_back(expr);

                //cout << expr << endl;
                continue;
            }


            else if( isalpha(source[i]) )
            {
                string var;
                var += source[i];

                size_t j;
                for(j = i+1; j < source.size() ; j++)
                {
                    if (isalpha(source[j]))
                    {
                        var += source[j];
                        if(j + 1 == source.size())
                        {
                            myVector.emplace_back(var);
                            i = j;
                            break;
                        }
                    }



                    else
                    {
                        i = --j; // vratime se na posledni ok character
                        myVector.emplace_back(var);
                        break;
                    }
                }

            }

            else
                throw UnexpectedCharacter(source[i]);
        }
    }

    void skipWhiteSpaces(string::iterator &it, const string::iterator &end)
    {
        for( ; it !=  end; ++it)if(! isspace ( *it )) break;
        if(it == end) throw UnexpectedEndOfExpression();
    }

    void skipWhiteSpacesFromBack(const string::iterator &begin, const string::iterator &end, string::iterator &newEnd)
    {
        newEnd = end;
        newEnd--;

        for( ; newEnd !=  begin; --newEnd)if(! isspace ( *newEnd )) break;
        if(newEnd == begin) throw UnexpectedEndOfExpression();

    }

    string loadWord(string::iterator & it, const string::iterator & end )
    {
        string varName;
        skipWhiteSpaces(it, end);
        for( ; it !=  end; ++it)
            if(('a' <= *it &&  *it <= 'z') || ('0' <= *it &&  *it <= '9') || ('A' <= *it &&  *it <= 'Z'))
                varName += *it;
            else
                break;


        return varName;
    }

public:
    Expression * generateExpression(string s)   /// ten mystream neak posere mezery
    {
        auto s_it = s.begin();
        for( ; s_it !=  s.end(); ++s_it)if(! isspace ( *s_it )) break;
        if(s_it == s.end()) throw EmptyExpression();

        /// STRUCTURE
        if(*s_it == '(')
        {
            skipWhiteSpaces(++s_it, s.end());

            /// LAMBDA
            if(*s_it == '\\')
            {
                /// nactiJmenoPromenne
                string varName = loadWord(++s_it, s.end());

                /// nacti tecku
                skipWhiteSpaces(s_it, s.end());
                if(*s_it != '.') throw UnexpectedCharacter(*s_it);

                /// odmaz mezery od konce
                string::iterator newEnd;
                skipWhiteSpacesFromBack(s_it, s.end(), newEnd);

                ///zkontroluj zavorku
                if(*newEnd != ')') throw UnexpectedCharacter(*s_it);

                ///a vyrob novy string
                string newString(++s_it, newEnd);

                ///naparsuj ho
                vector<string> parsedString;
                parseToList(newString, parsedString);

                if(parsedString.size() == 1)
                    return new Lambda(varName, generateExpression(  parsedString[0] ));
                else
                {
                    ///z jednotlivych stringu vyrob expressiony
                    vector<Expression *> expressions;
                    for (const auto &j : parsedString)
                        expressions.push_back(generateExpression(j));

                    return new Lambda(varName, new Application(expressions));
                }
            }

            /// APPLICATION
            else
            {
                /// odkroj mezery od konce
                string::iterator newEnd;
                skipWhiteSpacesFromBack(s_it, s.end(), newEnd);
                /// TODO CHECK ZAVORKA

                ///a vyrob novy string
                string newString(s_it, newEnd);

                ///naparsuj ho
                vector<string> parsedString;
                parseToList(newString, parsedString);

                ///z jednotlivych stringu vyrob expressiony
                vector<Expression *> expressions;
                for (const auto &j : parsedString)
                    expressions.push_back(generateExpression(j));

                return new Application(expressions);
            }
        }

        /// VARIABLE
        else
        {
            string varName = loadWord(s_it, s.end());
            return new Variable(varName);
        }
    }
};




/// ********************************************************************************************************************
/// **************************************** MAIN **********************************************************************
/// ********************************************************************************************************************
int main() {

    cout << "SIMPLE LABDA CALCULUS INTERPRET" << endl;
    cout << "syntax example : ((\\z . z ) (\\q. q q ) (\\s. s a ))" << endl;
    cout << "normal / app evaluatin" << endl;

    /// MAIN LOOP
    ExpressionGenerator exprGen;

    reducType  reduction = NORMAL;
    while(true)
    {
        string expression;
        getline(cin, expression);

        if(expression == "normal")
        {
            reduction = NORMAL;
            continue;
        }

        if(expression == "app")
        {
            reduction = APPLICATIVE;
            continue;
        }

        if(expression == "exit")
            break;

        try
        {
            Expression * a = exprGen.generateExpression(expression);
            a->print();
        //    a->detectBound();
            a->reduct(reduction);
        }catch (UnexpectedCharacter & x)
        {
            cout << x;
            cout << "CHYBNY VSTUP" << endl;
        }
        catch (...)
        {
            cout << "CHYBNY VSTUP" << endl;
        }
    }

    return 0;

    //string newExpression = "((\\z . z ) (\\q. q q ) (\\s. s a ))";
    //string expr2 = "( (\z .z z) (\z .z ) (\z .z q) )";


    //  Expression *chyba = new Application({ new Lambda("x", new Variable("x")),  new Application({ new Lambda("c", new Application({ new Variable("c"), new Variable("c")})) , new Lambda("c", new Application({ new Variable("c"), new Variable("c")})) })});

    Expression * lab = new Lambda("x", new Variable("X"));
    lab->print();
    lab->reduct(NORMAL);

    Expression *zasek = new Application({ new Lambda("x", new Variable("a")),  new Application({ new Lambda("c", new Application({ new Variable("c"), new Variable("c")})) , new Lambda("c", new Application({ new Variable("c"), new Variable("c")})) })});
    zasek->print();
    zasek->reduct(NORMAL);


    Expression *naAlpha = new Application({ new Lambda("x",  new Application ({new Variable("x"), new Lambda("b", new Application({new Variable("b"), new Variable("x"), new Variable("a")}))  } )  ),  new Lambda("b", new Variable("b"))});
    naAlpha->print();
    naAlpha->reduct(NORMAL);


    Expression *a = new Application({new Application({new Application({new Application(
            {new Lambda("x", new Application({new Variable("x"), new Variable("x")})), new Variable("y")})})})});
    a->print();
    a->reduct(NORMAL);

    Expression *b = new Application({new Lambda("x", new Application(
            {new  Lambda("y", new Application({new Variable("y"), new Variable("x")})), new Variable("x"),
             new Variable("y")})), new Variable("z")});
    b->print();
    b->reduct(NORMAL);

    Expression *c = new Application({new Lambda("x", new Application({new Application(
            {new Variable("x"), new Variable("x"), new Application({new Variable("x"), new Variable("x")})}),
                                                                      new Application(
                                                                              {new Variable("x"), new Variable("x")}),
                                                                      new Variable("x"), new Variable("x")})),
                                     new Variable("y")});
    c->print();
    c->reduct(NORMAL);

    // (lambda x . (x x)) y
    Expression *d = new Application({new Application({new Application({new Application(
            {new Lambda("x", new Application({new Variable("x"), new Variable("x")})), new Variable("y")})})})});
    d->print();
    d->reduct(NORMAL);

    //(λ. x  (λy. y x) x y) z
    Expression *e = new Application({new Lambda("x", new Application(
            {new Lambda("y", new Application({new Variable("y"), new Variable("x")})), new Variable("x"),
             new Variable("y")})), new Variable("z")});
    e->print();
    e->reduct(NORMAL);

    Expression *f = new Application({new Lambda("a", new Application({new Lambda("b", new Application(
            {new Variable("a"), new Variable("a"), new Variable("b"), new Variable("b"),}))})), new Application(
            {new Lambda("x", new Application({new Lambda("a", new Application({new Variable("a")}))})),
             new Lambda("x", new Application({new Variable("x"), new Variable("x"),})), new Variable("a")}),
                                     new Variable("d")});
    f->print();
    f->reduct(NORMAL);

    delete zasek;
    delete naAlpha;
    delete a;
    delete b;
    delete c;
    delete d;
    delete e;
    delete f;

    return 0;
}


///*********************************************************************************************************************
///*********************************************************************************************************************
///*********************************************************************************************************************

std::ostream &Application::doprint(std::ostream &os) const {
    bool space = true;

    if (m_expressions.size() > 1)
        cout << "(";

    for (Expression *v : m_expressions) {
        if (!space)
            os << " ";

        if (space)
            space = false;

        os << *v;
    }

    if (m_expressions.size() > 1)
        cout << ")";
    return os;
}

int getIndex(int indexMin, vector<string> & names)
{
    for(int j = indexMin ; ; ++j)
    {
        string n = "i";
        n += to_string(j);
        bool found = false;
        for(auto & x : names)
        {
            if(x == n)
            {
                found = true;
                break;
            }
        }

        if( ! found)
            return j;
    }
}
bool Application::rename(std::vector<pair<Variable * , vector<Variable*>>> &forSub, std::vector<pair<Variable * , vector<Variable*>>> &renameThis, std::vector<string> & names)
{
    ///if u find that forSub does contain variable with name that is bound in renameThis (except replacing one...)
        /// find new name (must differ from all names in both categories -- add dots??? )

    bool renamed = false;

    int i = getIndex(0,names);

    for ( auto & x : forSub)
    {
        for (auto toRename = renameThis.begin(); toRename != renameThis.end(); ++toRename)
        {
            auto tmp = toRename;

            if(++tmp == renameThis.end())
               continue;

            if(x.first->getName() == toRename->first->getName() ) // nazvy se shoduji a je tam vazana promenna
            {
                renamed = true;
                string newJmeno =  "i";

                newJmeno += to_string(i++);
                i = getIndex(i, names);
                toRename->first->rename(newJmeno);

                for (auto z : toRename->second)
                    z->rename(newJmeno);
            }
        }
    }
    return  renamed;
}
