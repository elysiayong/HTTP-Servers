# httperf --server localhost --port 18000 --uri /www.cs.toronto.edu/~ylzhang/csc258/memes.html --rate 10 --num-conn 100 --num-call 10 --timeout 5 > httperf-logs/persistent/test1.txt
# httperf --server localhost --port 18000 --uri /www.cs.toronto.edu/~ylzhang/csc258/memes.html --rate 10 --num-conn 100 --num-call 20 --timeout 5 > httperf-logs/persistent/test2.txt
# httperf --server localhost --port 18000 --uri /www.cs.toronto.edu/~ylzhang/csc258/memes.html --rate 10 --num-conn 100 --num-call 30 --timeout 5 > httperf-logs/persistent/test3.txt

# httperf --server localhost --port 18000 --uri /www.cs.toronto.edu/~ylzhang/csc258/memes.html --rate 10 --num-conn 100 --num-call 10 --timeout 5 > httperf-logs/pipelined/test1.txt
# httperf --server localhost --port 18000 --uri /www.cs.toronto.edu/~ylzhang/csc258/memes.html --rate 10 --num-conn 100 --num-call 20 --timeout 5 > httperf-logs/pipelined/test2.txt
# httperf --server localhost --port 18000 --uri /www.cs.toronto.edu/~ylzhang/csc258/memes.html --rate 10 --num-conn 100 --num-call 30 --timeout 5 > httperf-logs/pipelined/test3.txt

# httperf --server 192.168.0.32 --port 80 --uri /www.cs.toronto.edu/~ylzhang/csc258/memes.html --rate 10 --num-conn 100 --num-call 10 --timeout 5 > httperf-logs/apache/test1.txt
# httperf --server 192.168.0.32 --port 80 --uri /www.cs.toronto.edu/~ylzhang/csc258/memes.html --rate 10 --num-conn 100 --num-call 20 --timeout 5 > httperf-logs/apache/test2.txt
# httperf --server 192.168.0.32 --port 80 --uri /www.cs.toronto.edu/~ylzhang/csc258/memes.html --rate 10 --num-conn 100 --num-call 30 --timeout 5 > httperf-logs/apache/test3.txt

