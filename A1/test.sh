httperf --server localhost --port 18000 --uri /www.cs.toronto.edu/~ylzhang/csc258/memes.html --rate 15 --num-conn 10 --num-call 1 --timeout 5 > httperf-logs/simple/test1.out
httperf --server localhost --port 18000 --uri /www.cs.toronto.edu/~ylzhang/csc258/memes.html --rate 25 --num-conn 100 --num-call 1 --timeout 5 > httperf-logs/simple/test2.out
httperf --server localhost --port 18000 --uri /www.cs.toronto.edu/~ylzhang/csc258/memes.html --rate 35 --num-conn 1000 --num-call 1 --timeout 5 > httperf-logs/simple/test3.out
httperf --server localhost --port 18000 --uri /www.cs.toronto.edu/~ylzhang/csc258/memes.html --rate 45 --num-conn 10000 --num-call 1 --timeout 5 > httperf-logs/simple/test3.out



# httperf --server localhost --port 18000 --uri /www.cs.toronto.edu/~ylzhang/csc258/memes.html --rate 15 --num-conn 10 --num-call 1 --timeout 5 > httperf-logs/persistent/test1.out
# httperf --server localhost --port 18000 --uri /www.cs.toronto.edu/~ylzhang/csc258/memes.html --rate 25 --num-conn 100 --num-call 10 --timeout 5 > httperf-logs/persistent/test2.out
# httperf --server localhost --port 18000 --uri /www.cs.toronto.edu/~ylzhang/csc258/memes.html --rate 35 --num-conn 1000 --num-call 100 --timeout 5 > httperf-logs/persistent/test3.out


# httperf --server localhost --port 18000 --uri /www.cs.toronto.edu/~ylzhang/csc258/memes.html --rate 1 --num-conn 1 --num-call 1 --wsess=100,50,10 --burst-len 5 --timeout 5 > httperf-logs/pipelined/test1.out
# httperf --server localhost --port 18000 --uri /www.cs.toronto.edu/~ylzhang/csc258/memes.html --rate 25 --num-conn 100 --num-call 10 --wsess=100,50,10 --burst-len 5 --timeout 5 > httperf-logs/pipelined/test1.out
# httperf --server localhost --port 18000 --uri /www.cs.toronto.edu/~ylzhang/csc258/memes.html --rate 35 --num-conn 1000 --num-call 100 --wsess=100,50,10 --burst-len 5 --timeout 5 > httperf-logs/pipelined/test1.out


