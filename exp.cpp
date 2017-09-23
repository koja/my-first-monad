#include <boost/optional.hpp>
#include <functional>
#include <iostream>

using boost::optional;
using std::function;

/*
auto bind(optional<int> a) -> 
    function<   
        optional<int>
        (
            function< optional<int>( int ) >
        )   
    >
{
    if(!a) { 
        return 
            [](function< optional<int>(int) > ) -> optional<int> { 
                return boost::none;
            };
    } else {
    
        const int a_val = a.value();

        return [a_val](function< optional<int>(int) > f ) -> optional<int> {
            return f(a_val);
        };
    }
}
*/

template<typename TUnder, template<typename> class TMonad>
struct DefaultMonadPierce;
    
template<typename TUnder>
struct DefaultMonadPierce<TUnder, optional> {
    static TUnder pierce(const optional<TUnder>& in) { return in.value(); }
};

template<typename TUnder, template<typename> class TMonad, typename TMonadPierce = DefaultMonadPierce<TUnder, TMonad> /* how to get underlying type */>
auto bind( TMonad<TUnder> a ) -> 
    function<   
        TMonad<TUnder>
        (
            function< TMonad<TUnder>( TUnder ) >
        )   
    >
{
    if(!a) { 
        return 
            [](function< TMonad<TUnder>(TUnder) > /*ignore*/) -> TMonad<TUnder> { 
                return TMonad<TUnder>{};
            };
    } else {
    
        const TUnder a_val = TMonadPierce::pierce(a);

        return [a_val](function< TMonad<TUnder>(TUnder) > f ) -> TMonad<TUnder> {
            return f(a_val);
        };
    }
}

auto add(optional<int> lhs) -> function< optional<int>(optional<int>) > {
    return 
        [lhs](optional<int> rhs) -> optional<int> {
            return bind(lhs)(
                [rhs](int x) -> optional<int> {
                    return bind(rhs)(   
                        [x](int y) -> optional<int> { 
                            return x + y; 
                        }
                    );
                }
            );
        };
};   

/*
 1. type constructor <==> template optional<class T>
 2. unit function <==> constructor optional<T>::optional<T>(const T)
 3. binding operator <==> specialization of bind() function above
*/

int main() {

    optional<int> a = 5;
    optional<int> b = 42;
    optional<int> c;
    
    std::cout << add(a)(b).value();
    //std::cout << add(a)(c).value();

    return 42;
}

