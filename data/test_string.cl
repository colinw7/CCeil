assert("Hello" == "Hello")

#+"a"
#-"a"
#~"b"

assert("a" + "b" == "ab")
assert("abcde" - "bc" == "ade")
assert("abc" * 3 == "abcabcabc")
assert("--%s--" % "c" == "--c--")
assert("abbcccddde" / "c" == 3)
"a" ** "b"

assert(("a" == "a") == 1)
assert(("a" ~= "a") == 1)
assert(("a" != "a") == 0)
assert(("a" <  "b") == 1)
assert(("a" <= "b") == 1)
assert(("a" >  "b") == 0)
assert(("a" >= "b") == 0)

#"d" & "f"
#"d" | "f"
#"d" ^ "f"

assert("abcd" << 1 == "bcda")
assert("abcd" >> 3 == "bcda")

#cos("b")
assert(cos("1.2") ~= 0.362358)
#sin("a")
assert(sin("1.2") ~= 0.932039)
#tan("a")
assert(tan("1.2") ~= 2.572152)

#acos("a")
assert(acos("0.5") ~= 1.047198)
#asin("a")
assert(asin("0.5") ~= 0.523599)
#atan("a")
assert(atan("0.5") ~= 0.463648)
#atan2("a", "b")
assert(atan2("1.2", 1.2) ~= 0.785398)

#cosh("b")
assert(cosh("1.2") ~= 1.810656)
#sinh("a")
assert(sinh("1.2") ~= 1.509461)
#tanh("a")
assert(tanh("1.2") ~= 0.833655)

#exp("a")
assert(exp("1.2") ~= 3.320117)
#log("a")
assert(log("1.2") ~= 0.182322)
#log10("a")
assert(log10("1.2") ~= 0.079181)

#abs("abc")
assert(abs("-1.2") ~= 1.2)
#ceil("d")
assert(ceil("1.2") ~= 2.0)
#floor("a")
assert(floor("1.2") ~= 1.0)
#sign("a")
assert(sign("1.2") ~= 1.0)

#sqr("ab")
assert(sqr("1.2") ~= 1.44)
#sqrt("abcd")
assert(sqrt("1.2") ~= 1.095445)

assert(char("a") == "a")
assert(int("b") == 0)
assert(int("45") == 45)
assert(int("a45") == 0)
assert(int("45a") == 0)
assert(real("a") != 0)
assert(real("1.23") ~= 1.23)
assert(string("abcde") == "abcde")

#arrayI("a")
#arrayR("a")
#arrayS("a")
#indarrI("a")
#indarrR("a")
indarrS("a")

assert(len("abcde") == 5)
assert(index("abcdc","c") == 3)
assert(rindex("abca","a") == 4)
assert(max("abdec") == "e")
assert(min("dajd") == "a")
assert(isctype("a","print") == 1)
assert(tolower("AbC") == "abc")
assert(toupper("aBc") == "ABC")
assert(sort("agdefb") == "abdefg")

#addr("a")
#arrcat
#isNaN("aa")
#isNaN("")
assert(isNaN("NaN") == 1)
assert(dim("abc") == 1)
assert(ndim("ab") == 1)
#subarr
assert(sum("abcde") == 495)
#typearr
assert(valtyp("aa") == 4)
#where
