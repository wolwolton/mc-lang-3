def f(x)
    x * x - 6 * x + 9

def solve(x)
 if f( x ) < 1 then
 x
 else
 solve(x + 1)