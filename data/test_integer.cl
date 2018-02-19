# normal value
assert(1 == 1)

# unary ops
assert(+1 == 1)
assert(-2 == -2)
assert(~3 == -4)

# binary ops
assert(1 + 2 == 3)
assert(5 - 3 == 2)
assert(3 * 5 == 15)
assert(4 / 2 == 2)
assert(5 % 3 == 2)
assert(3 ** 3 == 27)

assert((1 == 1) == 1)
assert((1 ~= 1) == 1)
assert((1 != 1) == 0)
assert((1 <  2) == 1)
assert((1 <= 2) == 1)
assert((1 >  2) == 0)
assert((1 >= 2) == 0)

assert((3 & 4) == 0)
assert((3 | 4) == 7)
assert((3 ^ 4) == 7)

assert((3 << 2) == 12)
assert((7 >> 1) == 3)

# internal functions
assert(abs(-6) == 6)
assert(ceil(-3) == -3)
assert(floor(2) == 2)
assert(sign(-3) == -1)

assert(sqr(23) == 529)
assert(sqrt(49) == 7)

assert(cos(23) ~= -0.532833)
assert(sin(21) ~= 0.836656)
assert(tan(34) ~= -0.623499)
assert(acos(0) ~= 1.570796)
assert(asin(1) ~= 1.570796)
assert(atan(1) ~= 0.785398)
assert(atan2(1, 1) ~= 0.785398)

assert(exp(4) ~= 54.598150)
assert(log(11) ~= 2.397895)
assert(log10(12) ~= 1.079181)

assert(cosh(1) ~= 1.543081)
assert(sinh(1) ~= 1.175201)
assert(tanh(1) ~= 0.761594)

assert(char(43) == "+")
assert(int(24) == 24)
assert(real(12) ~= 12)
assert(string(123) == "123")

assert(isNaN(1) == 0)

assert(tolower(65) == 97)
assert(toupper(43) == 43)

assert(min(1) == 1)
assert(max(2) == 2)
assert(sum(3) == 3)

#index(1,1)
#rindex(1,1)

assert(sort(1) == 1)

#addr(1)

assert(arrayI(10) == [0,0,0,0,0,0,0,0,0,0])
assert(arrayR(10) ~= [0,0,0,0,0,0,0,0,0,0])
assert(arrayS(10) == ["","","","","","","","","",""])

assert(indarrI(5) == [1,2,3,4,5])
assert(indarrR(5) ~= [1,2,3,4,5])
assert(indarrS(5) == ["1","2","3","4","5"])

#arrcat

assert(dim(10) == 1)
assert(len(123) == 1)
assert(ndim(11) == 1)

assert(isctype(65,"print") == 1)

#subarr
#typearr

assert(valtyp(12) == 1)

#where
