assert(4.567 ~= 4.567)

assert(+1.4 ~= 1.4)
assert(-2.3 ~= 0.0 - 2.3)
assert(~3.1 != 4)

assert(1.1 + 2.7 ~= 3.8)
assert(5.3 - 3.4 ~= 1.9)
assert(3.3 * 5.1 ~= 16.83)
assert(4.5 / 2.5 ~= 1.8)
assert(5.4 % 3.1 ~= 2.3)
assert(3.3 ** 3.7 ~= 82.889893)

assert(! (1.1 == 1.2))
assert(1.1 ~= 1.100001)
assert(! (1.2 != 1.2))
assert(1.3 <  2.7)
assert(1.5 <= 2.5)
assert(2.5 <= 2.5)
assert(! (1.7 >  2.3))
assert(! (1.9 >= 2.1))
assert((1.9 >= 1.9))

assert((3.2 & 4.1) == 0)
assert((3.2 | 4.1) == 7)
assert((3.2 ^ 4.1) == 7)

assert((2 << 2) == 8)
assert((8 >> 2) == 2)
assert((3.1 << 2.2) ~= 14.243860)
assert((7.2 >> 1.3) ~= 2.924109)

assert(abs(-6.3) ~= 6.3)
assert(ceil(-3.4) ~= -3.0)
assert(floor(2.1) ~= 2.0)

assert(acos(0.1) ~= 1.470629)
assert(asin(0.1) ~= 0.100167)
assert(atan(0.2) ~= 0.197396)

assert(atan2(1.1, 1.2) ~= 0.741947)

#addr(2.1)

assert(arrayI(10.1) == [0,0,0,0,0,0,0,0,0,0])
assert(arrayR(10.2) ~= [0,0,0,0,0,0,0,0,0,0])
assert(arrayS(10.3) == ["","","","","","","","","",""])

#arrcat

assert(char(43.4) == "+")

assert(cos(23.1) ~= -0.445690)
assert(sin(21.1) ~= 0.777794)
assert(tan(34.9) ~= 0.356531)

assert(cosh(1.1) ~= 1.668519)
assert(sinh(1.2) ~= 1.509461)
assert(tanh(1.8) ~= 0.946806)

assert(exp(4.7) ~= 109.947172)
assert(log(11.1) ~= 2.406945)
assert(log10(12.123) ~= 1.083610)

assert(indarrI(5.1) == [1,2,3,4,5])
assert(indarrR(5.1) ~= [1,2,3,4,5])
assert(indarrS(5.1) == ["1","2","3","4","5"])

assert(isNaN(1.34) == 0)
assert(isNaN("NaN") == 1)

assert(int(24.2) == 24)
assert(real(12.12) ~= 12.12)
assert(string(123.12) == "123.120000")

#index(1.1,1.1)

assert(isctype(6.5,"print") == 0)

assert(len(123.21) == 1)
assert(dim(10.3) == 1)
assert(ndim(11.64) == 1)

assert(max(2.23) ~= 2.23)
assert(min(1.64) ~= 1.64)
assert(sum(3.9) ~= 3.9)

#rindex(.41,.51)

assert(sign(-3.323) ~= -1.0)

assert(sort(1.2) ~= 1.2)

assert(sqr(23.2) ~= 538.240000)
assert(sqrt(45.2) ~= 6.723095)

#subarr

assert(tolower(65.2) == 97)
assert(toupper(43.212) == 43)

#typearr

assert(valtyp(12.12) == 2)

#where
